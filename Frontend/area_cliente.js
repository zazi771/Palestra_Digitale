/* =========================================================
   DEMO STATICO — nessun dato viene inviato a un server.
   Tutti i dati (cliente, cartella clinica, piano, programma, sessioni)
   vivono solo in memoria e verranno sostituiti in seguito da
   chiamate al backend/DB.
   ========================================================= */

/* ---------- STATO IN MEMORIA ---------- */
const cliente = { nome: "Marco", cognome: "Ricci", email: "marco.ricci@example.it" };

let clinicalRecords = []; // cartella clinica: si popola quando il cliente compila il gate

// piano alimentare assegnato dal nutrizionista (mock, in attesa del backend)
const pianoAlimentare = {
    nutrizionista: "Dott.ssa Elena Bianchi",
    nome: "Piano bilanciato 2000 kcal",
    descrizione: "Piano orientato al mantenimento della massa magra con leggero deficit calorico. Bere almeno 2L di acqua al giorno.",
    creato: "2026-07-14",
    pasti: [
        { ordine: 1, giorno: "Lunedì", tipo: "Colazione", alimenti: [ { cibo: "Fiocchi d'avena", quantita: 60 }, { cibo: "Latte parzialmente scremato", quantita: 200 }, { cibo: "Frutta fresca mista", quantita: 150 } ] },
        { ordine: 2, giorno: "Lunedì", tipo: "Pranzo", alimenti: [ { cibo: "Petto di pollo", quantita: 150 }, { cibo: "Riso basmati", quantita: 80 }, { cibo: "Verdure miste al vapore", quantita: 200 } ] },
        { ordine: 3, giorno: "Lunedì", tipo: "Cena", alimenti: [ { cibo: "Filetto di merluzzo", quantita: 180 }, { cibo: "Patate al forno", quantita: 150 }, { cibo: "Insalata verde", quantita: 100 } ] },
        { ordine: 4, giorno: "Mercoledì", tipo: "Colazione", alimenti: [ { cibo: "Uova intere", quantita: 100 }, { cibo: "Pane integrale", quantita: 50 } ] },
        { ordine: 5, giorno: "Mercoledì", tipo: "Pranzo", alimenti: [ { cibo: "Pasta integrale", quantita: 80 }, { cibo: "Ceci", quantita: 100 }, { cibo: "Pomodorini", quantita: 100 } ] },
        { ordine: 6, giorno: "Venerdì", tipo: "Cena", alimenti: [ { cibo: "Tacchino a fette", quantita: 150 }, { cibo: "Quinoa", quantita: 70 }, { cibo: "Broccoli", quantita: 200 } ] }
    ]
};

// programma di allenamento assegnato dal trainer (mock, in attesa del backend)
const programmaAllenamento = {
    trainer: "Coach Luca Rossi",
    nome: "Programma Full Body 4 giorni",
    descrizione: "Programma orientato all'ipertrofia con progressione di carico settimanale. Recupero 90-120s tra le serie multi-articolari.",
    creato: "2026-07-14",
    esercizi: [
        { ordine: 1, giorno: "Lunedì", nome: "Squat con bilanciere", serie: 4, ripetizioni: "8-10", recupero: "120s" },
        { ordine: 2, giorno: "Lunedì", nome: "Panca piana", serie: 4, ripetizioni: "8-10", recupero: "120s" },
        { ordine: 3, giorno: "Lunedì", nome: "Rematore con manubrio", serie: 3, ripetizioni: "10-12", recupero: "90s" },
        { ordine: 4, giorno: "Mercoledì", nome: "Stacco da terra rumeno", serie: 4, ripetizioni: "8-10", recupero: "120s" },
        { ordine: 5, giorno: "Mercoledì", nome: "Trazioni alla sbarra", serie: 3, ripetizioni: "max", recupero: "90s" },
        { ordine: 6, giorno: "Mercoledì", nome: "Military press", serie: 3, ripetizioni: "8-10", recupero: "90s" },
        { ordine: 7, giorno: "Venerdì", nome: "Affondi con manubri", serie: 3, ripetizioni: "12/gamba", recupero: "75s" },
        { ordine: 8, giorno: "Venerdì", nome: "Plank", serie: 3, ripetizioni: "45s", recupero: "45s" }
    ]
};

// sessioni di allenamento svolte dal cliente (mock, in attesa del backend)
const sessioni = [
    {
        id: 1, data: "2026-08-11", nota: "Buona sessione, carichi aumentati sullo squat.",
        esercizi: [
            { nome: "Squat con bilanciere", fatto: true },
            { nome: "Panca piana", fatto: true },
            { nome: "Rematore con manubrio", fatto: true }
        ]
    },
    {
        id: 2, data: "2026-08-13", nota: "Poco tempo, saltata la military press.",
        esercizi: [
            { nome: "Stacco da terra rumeno", fatto: true },
            { nome: "Trazioni alla sbarra", fatto: true },
            { nome: "Military press", fatto: false }
        ]
    },
    {
        id: 3, data: "2026-08-15", nota: "Sessione completata interamente.",
        esercizi: [
            { nome: "Affondi con manubri", fatto: true },
            { nome: "Plank", fatto: true }
        ]
    },
    {
        id: 4, data: "2026-08-18", nota: "Fastidio alla spalla, saltata la panca.",
        esercizi: [
            { nome: "Squat con bilanciere", fatto: true },
            { nome: "Panca piana", fatto: false },
            { nome: "Rematore con manubrio", fatto: true }
        ]
    }
];

/* ---------- CAMPI CARTELLA CLINICA (condivisi tra gate e modale) ---------- */
const cartellaFields = [
    { id: "data", label: "Data rilevazione", type: "date", required: true },
    { id: "altezza", label: "Altezza (cm)", type: "number", required: true, min: 0, step: 1 },
    { id: "peso", label: "Peso (kg)", type: "number", required: true, min: 0, step: 0.1 },
    { id: "circVita", label: "Circonferenza vita (cm)", type: "number", required: true, min: 0, step: 0.5 },
    { id: "circFianchi", label: "Circonferenza fianchi (cm)", type: "number", required: true, min: 0, step: 0.5 },
    { id: "massaGrassa", label: "Massa grassa (%)", type: "number", required: true, min: 0, max: 100, step: 0.1 },
    { id: "massaMagra", label: "Massa magra (kg)", type: "number", required: true, min: 0, step: 0.1 },
    { id: "livelloAttivita", label: "Livello attività fisica", type: "select", required: true,
        options: ["Sedentario", "Leggero (1-2 sessioni/sett.)", "Moderato (3-4 sessioni/sett.)", "Intenso (5+ sessioni/sett.)", "Atleta agonista"] },
    { id: "obiettivo", label: "Obiettivo", type: "select", required: true,
        options: ["Perdita di peso", "Aumento massa muscolare", "Mantenimento", "Ricomposizione corporea", "Performance sportiva", "Salute generale"] },
    { id: "patologie", label: "Patologie", type: "textarea", wide: true, placeholder: "Es. Nessuna, oppure specifica (ipertensione, diabete...)" },
    { id: "allergie", label: "Allergie", type: "textarea", wide: true, placeholder: "Es. Nessuna, oppure specifica" },
    { id: "intolleranze", label: "Intolleranze alimentari", type: "textarea", wide: true, placeholder: "Es. Nessuna, lattosio, glutine..." },
    { id: "infortuni", label: "Infortuni pregressi", type: "textarea", wide: true, placeholder: "Es. Nessuno, oppure specifica" },
    { id: "farmaci", label: "Farmaci assunti", type: "textarea", wide: true, placeholder: "Es. Nessuno, oppure specifica" },
    { id: "noteMediche", label: "Note mediche", type: "textarea", wide: true, placeholder: "Altre informazioni utili per nutrizionista e trainer" }
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
                ${f.options.map(o => `<option>${o}</option>`).join("")}
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
        if(f.required && !val){
            missing.push(f.label);
        }
        values[f.id] = val || (f.type === "textarea" ? "Nessuna segnalazione" : "");
    });
    return { values, missing };
}

function clearCartellaFields(suffix){
    cartellaFields.forEach(f => {
        const el = document.getElementById(fieldId(f, suffix));
        el.value = "";
    });
}

/* ---------- UTIL ---------- */
function formatDate(iso){
    if(!iso) return "—";
    const d = new Date(iso + "T00:00:00");
    return d.toLocaleDateString("it-IT", { day: "2-digit", month: "short", year: "numeric" });
}
function initials(nome, cognome){
    return (nome[0] || "").toUpperCase() + (cognome[0] || "").toUpperCase();
}
const giorniOrdine = ["Lunedì","Martedì","Mercoledì","Giovedì","Venerdì","Sabato","Domenica"];
function groupByGiorno(items){
    const groups = {};
    items.forEach(it => {
        if(!groups[it.giorno]) groups[it.giorno] = [];
        groups[it.giorno].push(it);
    });
    return giorniOrdine
        .filter(g => groups[g])
        .map(g => ({ giorno: g, items: groups[g] }));
}

/* =========================================================
   GATE — CARTELLA CLINICA INIZIALE
   ========================================================= */
document.getElementById("cartellaFieldsGate").innerHTML = buildCartellaFieldsHTML("gate");

const cartellaForm = document.getElementById("cartellaForm");
const cartellaError = document.getElementById("cartellaError");

cartellaForm.addEventListener("submit", (e) => {
    e.preventDefault();
    cartellaError.hidden = true;

    const { values, missing } = readCartellaFields("gate");
    if(missing.length > 0){
        cartellaError.textContent = "Controlla questi campi: " + missing.join(", ");
        cartellaError.hidden = false;
        return;
    }

    clinicalRecords.push({ id: Date.now(), ...values });

    const chip = document.getElementById("clientChip");
    const chipText = document.getElementById("clientChipText");
    chipText.textContent = `${cliente.nome} ${cliente.cognome} · Cliente`;
    chip.hidden = false;

    document.getElementById("gate").style.display = "none";
    document.getElementById("dashboard").hidden = false;

    renderAll();
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
   TAB: CARTELLA CLINICA (storico + nuova rilevazione)
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
function closeRecordModal(){
    recordOverlay.classList.remove("open");
    document.body.style.overflow = "";
}

recordForm.addEventListener("submit", (e) => {
    e.preventDefault();
    recordError.hidden = true;

    const { values, missing } = readCartellaFields("modal");
    if(missing.length > 0){
        recordError.textContent = "Controlla questi campi: " + missing.join(", ");
        recordError.hidden = false;
        return;
    }

    clinicalRecords.push({ id: Date.now(), ...values });
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

    const sorted = [...clinicalRecords].sort((a, b) => new Date(b.data) - new Date(a.data));

    sorted.forEach((rec, i) => {
        const prev = sorted[i + 1]; // rilevazione precedente (più vecchia)
        const div = document.createElement("div");
        div.className = "record-card";
        div.innerHTML = `
            <div class="record-head">
                <strong>${formatDate(rec.data)}</strong>
                <span class="plan-tag">${rec.obiettivo}</span>
            </div>
            <div class="record-grid">
                <div class="record-metric"><span class="metric-label">Peso</span><span class="metric-value">${rec.peso} kg ${trendArrow(rec, prev, "peso", true)}</span></div>
                <div class="record-metric"><span class="metric-label">Altezza</span><span class="metric-value">${rec.altezza} cm</span></div>
                <div class="record-metric"><span class="metric-label">Vita</span><span class="metric-value">${rec.circVita} cm ${trendArrow(rec, prev, "circVita", true)}</span></div>
                <div class="record-metric"><span class="metric-label">Fianchi</span><span class="metric-value">${rec.circFianchi} cm</span></div>
                <div class="record-metric"><span class="metric-label">Massa grassa</span><span class="metric-value">${rec.massaGrassa}% ${trendArrow(rec, prev, "massaGrassa", true)}</span></div>
                <div class="record-metric"><span class="metric-label">Massa magra</span><span class="metric-value">${rec.massaMagra} kg ${trendArrow(rec, prev, "massaMagra", false)}</span></div>
                <div class="record-metric"><span class="metric-label">Attività fisica</span><span class="metric-value">${rec.livelloAttivita}</span></div>
            </div>
            <div class="record-notes">
                <div><span class="metric-label">Patologie</span><p>${rec.patologie}</p></div>
                <div><span class="metric-label">Allergie</span><p>${rec.allergie}</p></div>
                <div><span class="metric-label">Intolleranze alimentari</span><p>${rec.intolleranze}</p></div>
                <div><span class="metric-label">Infortuni pregressi</span><p>${rec.infortuni}</p></div>
                <div><span class="metric-label">Farmaci assunti</span><p>${rec.farmaci}</p></div>
                <div><span class="metric-label">Note mediche</span><p>${rec.noteMediche}</p></div>
            </div>
        `;
        container.appendChild(div);
    });
}

/* =========================================================
   TAB: PIANO ALIMENTARE (assegnato dal nutrizionista)
   ========================================================= */
function renderPianoAlimentare(){
    const container = document.getElementById("pianoAlimentareView");
    const grouped = groupByGiorno(pianoAlimentare.pasti);

    container.innerHTML = `
        <div class="plan-card">
            <div class="plan-card-head">
                <h5>${pianoAlimentare.nome}</h5>
                <div class="plan-tags">
                    <span class="plan-tag">Assegnato da ${pianoAlimentare.nutrizionista}</span>
                    <span class="plan-tag">${formatDate(pianoAlimentare.creato)}</span>
                </div>
            </div>
            <p class="plan-desc">${pianoAlimentare.descrizione}</p>
            ${grouped.map(g => `
                <div class="day-block">
                    <h4 class="day-title">${g.giorno}</h4>
                    ${g.items.map(pasto => `
                        <div class="plan-ex-row">
                            <div class="plan-ex-body">
                                <strong>${pasto.tipo}</strong>
                                <span>${pasto.alimenti.map(a => `${a.cibo} (${a.quantita} g)`).join(" · ")}</span>
                            </div>
                        </div>
                    `).join("")}
                </div>
            `).join("")}
        </div>
    `;
}

/* =========================================================
   TAB: ALLENAMENTO (assegnato dal trainer)
   ========================================================= */
function renderProgramma(){
    const container = document.getElementById("programmaView");
    const grouped = groupByGiorno(programmaAllenamento.esercizi);

    container.innerHTML = `
        <div class="plan-card">
            <div class="plan-card-head">
                <h5>${programmaAllenamento.nome}</h5>
                <div class="plan-tags">
                    <span class="plan-tag">Assegnato da ${programmaAllenamento.trainer}</span>
                    <span class="plan-tag">${formatDate(programmaAllenamento.creato)}</span>
                </div>
            </div>
            <p class="plan-desc">${programmaAllenamento.descrizione}</p>
            ${grouped.map(g => `
                <div class="day-block">
                    <h4 class="day-title">${g.giorno}</h4>
                    ${g.items.map(es => `
                        <div class="plan-ex-row">
                            <div class="plan-ex-body">
                                <strong>${es.nome}</strong>
                                <span>${es.serie} serie × ${es.ripetizioni} ripetizioni · recupero ${es.recupero}</span>
                            </div>
                        </div>
                    `).join("")}
                </div>
            `).join("")}
        </div>
    `;
}

/* =========================================================
   TAB: I MIEI PROGRESSI (sessioni svolte, esercizi fatti/non fatti)
   ========================================================= */
function renderStats(){
    const totaleEsercizi = sessioni.reduce((sum, s) => sum + s.esercizi.length, 0);
    const eserciziFatti = sessioni.reduce((sum, s) => sum + s.esercizi.filter(e => e.fatto).length, 0);
    const percentuale = totaleEsercizi === 0 ? 0 : Math.round((eserciziFatti / totaleEsercizi) * 100);
    const ultima = [...sessioni].sort((a, b) => new Date(b.data) - new Date(a.data))[0];

    const container = document.getElementById("statsRow");
    container.innerHTML = `
        <div class="stat-card">
            <span class="stat-value">${sessioni.length}</span>
            <span class="stat-label">Sessioni registrate</span>
        </div>
        <div class="stat-card">
            <span class="stat-value">${eserciziFatti}/${totaleEsercizi}</span>
            <span class="stat-label">Esercizi completati</span>
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
        const fatti = s.esercizi.filter(e => e.fatto).length;
        const div = document.createElement("div");
        div.className = "sessione-card";
        div.innerHTML = `
            <div class="sessione-head">
                <strong>${formatDate(s.data)}</strong>
                <span class="plan-tag">${fatti}/${s.esercizi.length} esercizi svolti</span>
            </div>
            <p class="plan-desc">${s.nota}</p>
            <div class="exercise-check-list">
                ${s.esercizi.map(e => `
                    <div class="exercise-check-item ${e.fatto ? "check-done" : "check-missed"}">
                        <span class="check-icon">${e.fatto ? "✓" : "✕"}</span>
                        <span>${e.nome}</span>
                    </div>
                `).join("")}
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
    if(e.key === "Escape"){
        closeRecordModal();
    }
});