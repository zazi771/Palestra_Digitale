/* =========================================================
   area_cliente.js — Area Cliente collegata al backend REST
   ========================================================= */

/* ---------- STATO ---------- */
let utente = null; // dati utente dal backend
let clinicalRecords = [];
let pianoAlimentare = null;  // primo piano alimentare del cliente
let programmaAllenamento = null;
let sessioni = [];
let utenteCliente = null; // profilo dal backend

/* ---------- UTIL ---------- */
function getUtente(){
    const raw = localStorage.getItem('utente');
    if(!raw) return null;
    try { return JSON.parse(raw); } catch(_){ return null; }
}

function redirectHome(){ window.location.href = '/'; }

function formatDate(iso){
    if(!iso) return "—";
    const d = new Date(iso + "T00:00:00");
    return d.toLocaleDateString("it-IT", { day: "2-digit", month: "short", year: "numeric" });
}
function initials(nome, cognome){
    return (nome[0] || "").toUpperCase() + (cognome[0] || "").toUpperCase();
}
const giorniOrdine = ["Lunedì","Martedì","Mercoledì","Giovedì","Venerdì","Sabato","Domenica"];
const giornoDaNumero = n => giorniOrdine[n-1] || giorniOrdine[0];
function groupByGiorno(items){
    const groups = {};
    items.forEach(it => {
        const g = it.giorno_name || giornoDaNumero(it.giorno) || it.giorno;
        if(!groups[g]) groups[g] = [];
        groups[g].push(it);
    });
    return giorniOrdine
        .filter(g => groups[g])
        .map(g => ({ giorno: g, items: groups[g] }));
}

async function api(path, opts){
    const res = await fetch(path, opts);
    const data = await res.json();
    return { ok: res.ok, status: res.status, data };
}

/* =========================================================
   INIT
   ========================================================= */
utente = getUtente();
if(!utente || !utente.id) redirectHome();
else init();

async function init(){
    // carica profilo cliente
    const r = await api(`/api/cliente/${utente.id}`);
    if(!r.ok){ redirectHome(); return; }
    utenteCliente = r.data;

    // aggiorna chip header
    const chip = document.getElementById("clientChip");
    const chipText = document.getElementById("clientChipText");
    chipText.textContent = `${utenteCliente.nome} ${utenteCliente.cognome} · Cliente`;
    chip.hidden = false;

    // carica tutti i dati
    await Promise.all([caricaCartella(), caricaPianoAlimentare(), caricaProgramma(), caricaSessioni()]);

    // se non ha cartella clinica, mostra gate
    if(clinicalRecords.length === 0){
        document.getElementById("gate").style.display = "";
        document.getElementById("dashboard").hidden = true;
    } else {
        document.getElementById("gate").style.display = "none";
        document.getElementById("dashboard").hidden = false;
    }
    renderAll();
}

/* ---------- CARICAMENTO DATI ---------- */
async function caricaCartella(){
    const r = await api(`/api/cliente/${utente.id}/cartella-clinica`);
    if(!r.ok) return;
    clinicalRecords = r.data.cartelle || [];
}

async function caricaPianoAlimentare(){
    const r = await api(`/api/cliente/${utente.id}/piano-alimentare`);
    if(!r.ok) return;
    const piani = r.data.piani || [];
    pianoAlimentare = piani.length > 0 ? piani[0] : null;
}

async function caricaProgramma(){
    const r = await api(`/api/cliente/${utente.id}/programma-allenamento`);
    if(!r.ok) return;
    const programmi = r.data.programmi || [];
    programmaAllenamento = programmi.length > 0 ? programmi[0] : null;
}

async function caricaSessioni(){
    const r = await api(`/api/cliente/${utente.id}/sessioni`);
    if(!r.ok) return;
    sessioni = r.data.sessioni || [];
}

/* =========================================================
   CAMPI CARTELLA CLINICA (condivisi tra gate e modale)
   ========================================================= */
const cartellaFields = [
    { id: "data", label: "Data rilevazione", type: "date", required: true, api: "data_rilevazione" },
    { id: "altezza", label: "Altezza (cm)", type: "number", required: true, min: 0, step: 1, api: "altezza_cm" },
    { id: "peso", label: "Peso (kg)", type: "number", required: true, min: 0, step: 0.1, api: "peso_kg" },
    { id: "circVita", label: "Circonferenza vita (cm)", type: "number", required: true, min: 0, step: 0.5, api: "circonferenza_vita_cm" },
    { id: "circFianchi", label: "Circonferenza fianchi (cm)", type: "number", required: true, min: 0, step: 0.5, api: "circonferenza_fianchi_cm" },
    { id: "massaGrassa", label: "Massa grassa (%)", type: "number", required: true, min: 0, max: 100, step: 0.1, api: "massa_grassa_percentuale" },
    { id: "massaMagra", label: "Massa magra (kg)", type: "number", required: true, min: 0, step: 0.1, api: "massa_magra_kg" },
    { id: "livelloAttivita", label: "Livello attività fisica", type: "select", required: true, api: "livello_attivita_fisica",
        options: [
            { value: "sedentario", label: "Sedentario" },
            { value: "leggero", label: "Leggero (1-2 sessioni/sett.)" },
            { value: "moderato", label: "Moderato (3-4 sessioni/sett.)" },
            { value: "intenso", label: "Intenso (5+ sessioni/sett.)" },
            { value: "molto_intenso", label: "Atleta agonista" }
        ] },
    { id: "obiettivo", label: "Obiettivo", type: "select", required: true, api: "obiettivo",
        options: ["Perdita di peso", "Aumento massa muscolare", "Mantenimento", "Ricomposizione corporea", "Performance sportiva", "Salute generale"] },
    { id: "patologie", label: "Patologie", type: "textarea", wide: true, placeholder: "Es. Nessuna, oppure specifica (ipertensione, diabete...)", api: "patologie" },
    { id: "allergie", label: "Allergie", type: "textarea", wide: true, placeholder: "Es. Nessuna, oppure specifica", api: "allergie" },
    { id: "intolleranze", label: "Intolleranze alimentari", type: "textarea", wide: true, placeholder: "Es. Nessuna, lattosio, glutine...", api: "intolleranze_alimentari" },
    { id: "infortuni", label: "Infortuni pregressi", type: "textarea", wide: true, placeholder: "Es. Nessuno, oppure specifica", api: "infortuni_pregressi" },
    { id: "farmaci", label: "Farmaci assunti", type: "textarea", wide: true, placeholder: "Es. Nessuno, oppure specifica", api: "farmaci_assunti" },
    { id: "noteMediche", label: "Note mediche", type: "textarea", wide: true, placeholder: "Altre informazioni utili per nutrizionista e trainer", api: "note_mediche" }
];

function fieldId(field, suffix){ return `cc-${field.id}-${suffix}`; }

function buildCartellaFieldsHTML(suffix){
    return cartellaFields.map(f => {
        const id = fieldId(f, suffix);
        const wideClass = f.wide ? " field-wide" : "";
        let control;
        if(f.type === "select"){
            control = `<select id="${id}" ${f.required ? "required" : ""}>
                <option value="" disabled selected>Seleziona</option>
                ${f.options.map(o => typeof o === "string" ? `<option>${o}</option>` : `<option value="${o.value}">${o.label}</option>`).join("")}
            </select>`;
        } else if(f.type === "textarea"){
            control = `<textarea id="${id}" rows="2" placeholder="${f.placeholder || ""}"></textarea>`;
        } else {
            control = `<input id="${id}" type="${f.type}" ${f.min !== undefined ? `min="${f.min}"` : ""} ${f.max !== undefined ? `max="${f.max}"` : ""} ${f.step !== undefined ? `step="${f.step}"` : ""} ${f.required ? "required" : ""}>`;
        }
        return `<div class="field${wideClass}"><label for="${id}">${f.label}</label>${control}</div>`;
    }).join("");
}

function readCartellaFields(suffix){
    const values = {};
    const missing = [];
    cartellaFields.forEach(f => {
        const el = document.getElementById(fieldId(f, suffix));
        const val = el.value.trim ? el.value.trim() : el.value;
        if(f.required && !val) missing.push(f.label);
        values[f.api] = val || (f.type === "textarea" ? "" : "0");
    });
    return { values, missing };
}

function clearCartellaFields(suffix){
    cartellaFields.forEach(f => {
        const el = document.getElementById(fieldId(f, suffix));
        if(el) el.value = "";
    });
}

/* =========================================================
   GATE — CARTELLA CLINICA INIZIALE
   ========================================================= */
document.getElementById("cartellaFieldsGate").innerHTML = buildCartellaFieldsHTML("gate");
const cartellaForm = document.getElementById("cartellaForm");
const cartellaError = document.getElementById("cartellaError");

cartellaForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    cartellaError.hidden = true;
    const { values, missing } = readCartellaFields("gate");
    if(missing.length > 0){
        cartellaError.textContent = "Controlla questi campi: " + missing.join(", ");
        cartellaError.hidden = false;
        return;
    }
    const r = await api(`/api/cliente/${utente.id}/cartella-clinica`, {
        method: 'POST', headers: {'Content-Type':'application/json'}, body: JSON.stringify(values)
    });
    if(!r.ok){ cartellaError.textContent = r.data.errore || "Errore salvataggio."; cartellaError.hidden = false; return; }
    await caricaCartella();
    document.getElementById("gate").style.display = "none";
    document.getElementById("dashboard").hidden = false;
    renderRecords();
});

/* =========================================================
   TABS
   ========================================================= */
document.querySelectorAll(".tab-btn").forEach(btn => {
    btn.addEventListener("click", () => {
        document.querySelectorAll(".tab-btn").forEach(b => b.classList.remove("active"));
        document.querySelectorAll(".tab-panel").forEach(p => p.classList.remove("active"));
        btn.classList.add("active");
        document.getElementById(`tab-${btn.dataset.tab}`).classList.add("active");
    });
});

/* =========================================================
   TAB: CARTELLA CLINICA
   ========================================================= */
document.getElementById("cartellaFieldsModal").innerHTML = buildCartellaFieldsHTML("modal");
const recordOverlay = document.getElementById("recordOverlay");
const recordForm = document.getElementById("recordForm");
const recordError = document.getElementById("recordError");

document.getElementById("addRecordBtn").addEventListener("click", () => {
    clearCartellaFields("modal");
    recordError.hidden = true;
    recordOverlay.classList.add("open");
    document.body.style.overflow = "hidden";
});
document.getElementById("recordClose").addEventListener("click", closeRecordModal);
recordOverlay.addEventListener("click", (e) => { if(e.target === recordOverlay) closeRecordModal(); });
function closeRecordModal(){ recordOverlay.classList.remove("open"); document.body.style.overflow = ""; }

recordForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    recordError.hidden = true;
    const { values, missing } = readCartellaFields("modal");
    if(missing.length > 0){
        recordError.textContent = "Controlla questi campi: " + missing.join(", ");
        recordError.hidden = false;
        return;
    }
    const r = await api(`/api/cliente/${utente.id}/cartella-clinica`, {
        method: 'POST', headers: {'Content-Type':'application/json'}, body: JSON.stringify(values)
    });
    if(!r.ok){ recordError.textContent = r.data.errore || "Errore salvataggio."; recordError.hidden = false; return; }
    await caricaCartella();
    closeRecordModal();
    renderRecords();
});

function trendArrow(curr, prev, field, lowerIsBetter){
    if(prev === undefined) return "";
    const c = parseFloat(curr[field]);
    const p = parseFloat(prev[field]);
    if(isNaN(c) || isNaN(p) || c === p) return `<span class="trend trend-flat">→</span>`;
    const up = c > p;
    const good = lowerIsBetter ? !up : up;
    return `<span class="trend ${good ? "trend-up" : "trend-down"}">${up ? "↑" : "↓"} ${Math.abs(c - p).toFixed(1)}</span>`;
}

function renderRecords(){
    const container = document.getElementById("recordsList");
    container.innerHTML = "";
    if(clinicalRecords.length === 0){
        container.innerHTML = `<p class="plan-empty">Nessuna rilevazione ancora registrata.</p>`;
        return;
    }
    const sorted = [...clinicalRecords].sort((a, b) => new Date(b.data_rilevazione) - new Date(a.data_rilevazione));
    sorted.forEach((rec, i) => {
        const prev = sorted[i + 1];
        const div = document.createElement("div");
        div.className = "record-card";
        div.innerHTML = `
            <div class="record-head">
                <strong>${formatDate(rec.data_rilevazione)}</strong>
                <span class="plan-tag">${rec.obiettivo}</span>
            </div>
            <div class="record-grid">
                <div class="record-metric"><span class="metric-label">Peso</span><span class="metric-value">${rec.peso_kg} kg ${trendArrow(rec, prev, "peso_kg", true)}</span></div>
                <div class="record-metric"><span class="metric-label">Altezza</span><span class="metric-value">${rec.altezza_cm} cm</span></div>
                <div class="record-metric"><span class="metric-label">Vita</span><span class="metric-value">${rec.circonferenza_vita_cm} cm ${trendArrow(rec, prev, "circonferenza_vita_cm", true)}</span></div>
                <div class="record-metric"><span class="metric-label">Fianchi</span><span class="metric-value">${rec.circonferenza_fianchi_cm} cm</span></div>
                <div class="record-metric"><span class="metric-label">Massa grassa</span><span class="metric-value">${rec.massa_grassa_percentuale}% ${trendArrow(rec, prev, "massa_grassa_percentuale", true)}</span></div>
                <div class="record-metric"><span class="metric-label">Massa magra</span><span class="metric-value">${rec.massa_magra_kg} kg ${trendArrow(rec, prev, "massa_magra_kg", false)}</span></div>
                <div class="record-metric"><span class="metric-label">Attività fisica</span><span class="metric-value">${rec.livello_attivita_fisica}</span></div>
            </div>
            <div class="record-notes">
                <div><span class="metric-label">Patologie</span><p>${rec.patologie || "—"}</p></div>
                <div><span class="metric-label">Allergie</span><p>${rec.allergie || "—"}</p></div>
                <div><span class="metric-label">Intolleranze alimentari</span><p>${rec.intolleranze_alimentari || "—"}</p></div>
                <div><span class="metric-label">Infortuni pregressi</span><p>${rec.infortuni_pregressi || "—"}</p></div>
                <div><span class="metric-label">Farmaci assunti</span><p>${rec.farmaci_assunti || "—"}</p></div>
                <div><span class="metric-label">Note mediche</span><p>${rec.note_mediche || "—"}</p></div>
            </div>
        `;
        container.appendChild(div);
    });
}

/* =========================================================
   TAB: PIANO ALIMENTARE (dal backend)
   ========================================================= */
function renderPianoAlimentare(){
    const container = document.getElementById("pianoAlimentareView");
    if(!pianoAlimentare){
        container.innerHTML = `<p class="plan-empty">Nessun piano alimentare assegnato dal nutrizionista.</p>`;
        return;
    }
    const pasti = pianoAlimentare.pasti || [];
    // i pasti hanno: giorno (1-7), tipo_pasto, alimenti [{cibo, quantita_gr}]
    const pastiRender = pasti.map(p => ({
        giorno: giornoDaNumero(p.giorno),
        tipo: p.tipo_pasto,
        alimenti: (p.alimenti || [])
    }));
    const grouped = groupByGiorno(pastiRender);
    container.innerHTML = `
        <div class="plan-card">
            <div class="plan-card-head">
                <h5>${pianoAlimentare.nome}</h5>
                <div class="plan-tags">
                    <span class="plan-tag">Piano alimentare</span>
                </div>
            </div>
            <p class="plan-desc">${pianoAlimentare.descrizione || ""}</p>
            ${grouped.map(g => `
                <div class="day-block">
                    <h4 class="day-title">${g.giorno}</h4>
                    ${g.items.map(pasto => `
                        <div class="plan-ex-row">
                            <div class="plan-ex-body">
                                <strong>${pasto.tipo}</strong>
                                <span>${(pasto.alimenti || []).map(a => `${a.cibo} (${a.quantita_gr} g)`).join(" · ") || "—"}</span>
                            </div>
                        </div>
                    `).join("")}
                </div>
            `).join("")}
        </div>
    `;
}

/* =========================================================
   TAB: ALLENAMENTO (dal backend)
   ========================================================= */
function renderProgramma(){
    const container = document.getElementById("programmaView");
    if(!programmaAllenamento){
        container.innerHTML = `<p class="plan-empty">Nessun programma di allenamento assegnato dal trainer.</p>`;
        return;
    }
    const esercizi = (programmaAllenamento.esercizi || []).map(e => ({
        nome: e.nome || "Esercizio",
        serie: e.serie,
        ripetizioni: e.ripetizioni,
        recupero: e.recupero_sec + "s"
    }));
    // raggruppa per ordine come proxy (non abbiamo il giorno nel programma)
    const grouped = [{ giorno: programmaAllenamento.nome, items: esercizi }];
    container.innerHTML = `
        <div class="plan-card">
            <div class="plan-card-head">
                <h5>${programmaAllenamento.nome}</h5>
                <div class="plan-tags">
                    <span class="plan-tag">Programma allenamento</span>
                </div>
            </div>
            <p class="plan-desc">${programmaAllenamento.descrizione || ""}</p>
            ${esercizi.map((es, i) => `
                <div class="plan-ex-row">
                    <span class="ex-order">${i+1}</span>
                    <div class="plan-ex-body">
                        <strong>${es.nome}</strong>
                        <span>${es.serie} serie × ${es.ripetizioni} · recupero ${es.recupero}</span>
                    </div>
                </div>
            `).join("")}
        </div>
    `;
}

/* =========================================================
   TAB: I MIEI PROGRESSI (sessioni dal backend)
   ========================================================= */
function renderStats(){
    const container = document.getElementById("statsRow");
    const totaleTempo = sessioni.reduce((s,x) => s + (x.tempo_minuti||0), 0);
    const completate = sessioni.filter(x => x.completato).length;
    const percentuale = sessioni.length === 0 ? 0 : Math.round((completate / sessioni.length) * 100);
    const sorted = [...sessioni].sort((a, b) => new Date(b.data) - new Date(a.data));
    const ultima = sorted[0];
    container.innerHTML = `
        <div class="stat-card">
            <span class="stat-value">${sessioni.length}</span>
            <span class="stat-label">Sessioni registrate</span>
        </div>
        <div class="stat-card">
            <span class="stat-value">${completate}/${sessioni.length}</span>
            <span class="stat-label">Sessioni completate</span>
        </div>
        <div class="stat-card">
            <span class="stat-value">${percentuale}%</span>
            <span class="stat-label">Aderenza al programma</span>
        </div>
        <div class="stat-card">
            <span class="stat-value">${ultima ? formatDate(ultima.data) : "—"}</span>
            <span class="stat-label">Ultimo allenamento</span>
        </div>
    `;
}

function renderSessioni(){
    const container = document.getElementById("sessioniList");
    container.innerHTML = "";
    if(sessioni.length === 0){
        container.innerHTML = `<p class="plan-empty">Nessuna sessione registrata ancora.</p>`;
        return;
    }
    const sorted = [...sessioni].sort((a, b) => new Date(b.data) - new Date(a.data));
    sorted.forEach(s => {
        const div = document.createElement("div");
        div.className = "sessione-card";
        div.innerHTML = `
            <div class="sessione-head">
                <strong>${formatDate(s.data)}</strong>
                <span class="plan-tag">${s.completato ? "✓ Completata" : "Non completata"}</span>
            </div>
            <div class="record-grid" style="margin-top:8px;">
                <div class="record-metric"><span class="metric-label">Tempo</span><span class="metric-value">${s.tempo_minuti} min</span></div>
            </div>
        `;
        container.appendChild(div);
    });
}

/* =========================================================
   RENDER GLOBALE
   ========================================================= */
function renderAll(){
    renderRecords();
    renderPianoAlimentare();
    renderProgramma();
    renderStats();
    renderSessioni();
}

document.addEventListener("keydown", (e) => {
    if(e.key === "Escape") closeRecordModal();
});
