/* =========================================================
   area_trainer.js — Area Trainer collegata al backend REST
   ========================================================= */

/* ---------- BFCACHE: se ripristinato dalla cache dopo logout, torna alla home ---------- */
window.addEventListener('pageshow', (event) => {
    if (event.persisted && !localStorage.getItem('utente')) {
        window.location.href = '/';
    }
});

let utente = null;
let trainerProfile = null;
let clients = [];
let activeClientId = null;
let exerciseCounter = 0;
let editingPlanId = null;

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
    return d.toLocaleDateString("it-IT", { day:"2-digit", month:"short", year:"numeric" });
}
function initials(nome, cognome){ return (nome[0]||"").toUpperCase() + (cognome[0]||"").toUpperCase(); }
function getClient(id){ return clients.find(c => c.id === id); }

async function api(path, opts){
    const res = await fetch(path, opts);
    if(res.status === 204) return { ok: res.ok, status: res.status, data: {} };
    const ct = res.headers.get('content-type') || '';
    const data = ct.includes('json') ? await res.json() : {};
    return { ok: res.ok, status: res.status, data };
}

/* =========================================================
   INIT
   ========================================================= */
utente = getUtente();
if(!utente || !utente.id) redirectHome();
else init();

async function init(){
    const r = await api(`/api/trainer/${utente.id}`);
    if(!r.ok){ redirectHome(); return; }
    trainerProfile = r.data;

    const chip = document.getElementById("trainerChip");
    const chipText = document.getElementById("trainerChipText");
    chipText.textContent = `${trainerProfile.nome} ${trainerProfile.cognome} · Trainer`;
    chip.hidden = false;

    const logoutBtn = document.getElementById("logoutBtn");
    logoutBtn.hidden = false;
    logoutBtn.addEventListener("click", logout);

    // ATTendi la risposta del server prima di decidere la vista
    try {
        const cert = await api(`/api/trainer/${utente.id}/certificazione`);
        console.log("Risposta certificazione:", cert);

        // Verifica se la certificazione esiste (controlla lo status HTTP 200 e la presenza di dati)
        if(cert.ok && cert.data && (cert.data.id || cert.data.codice)){
            // Ha già la certificazione -> salta il gate e mostra la dashboard
            document.getElementById("gate").hidden = true;
            document.getElementById("dashboard").hidden = false;
            await caricaClienti();
            renderClients();
        } else {
            // Non ha la certificazione -> mostra il gate
            document.getElementById("gate").hidden = false;
            document.getElementById("dashboard").hidden = true;
        }
    } catch (error) {
        console.error("Errore durante il controllo della certificazione:", error);
        document.getElementById("gate").hidden = false;
        document.getElementById("dashboard").hidden = true;
    }
}

function logout(){
    localStorage.removeItem('utente');
    window.location.href = '/home.html';
}

async function caricaClienti(){
    const r = await api(`/api/trainer/${utente.id}/clienti`);
    if(!r.ok) return;
    clients = (r.data.clienti || []).map(c => ({ ...c, piani: [] }));
}

/* =========================================================
   GATE — CERTIFICAZIONE TRAINER
   ========================================================= */
const certForm = document.getElementById("certForm");
const certError = document.getElementById("certError");
const certificazioneInput = document.getElementById("c-certificazione");
const certificazioneDrop = document.getElementById("certificazioneDrop");
const certificazioneDropText = document.getElementById("certificazioneDropText");
const cvInput = document.getElementById("c-cv");
const cvDrop = document.getElementById("cvDrop");
const cvDropText = document.getElementById("cvDropText");

certificazioneInput.addEventListener("change", () => {
    if(certificazioneInput.files && certificazioneInput.files.length > 0){
        certificazioneDropText.textContent = certificazioneInput.files[0].name;
        certificazioneDrop.classList.add("has-file");
    } else {
        certificazioneDropText.textContent = "Trascina qui la tua certificazione o clicca per selezionarla";
        certificazioneDrop.classList.remove("has-file");
    }
});

cvInput.addEventListener("change", () => {
    if(cvInput.files && cvInput.files.length > 0){
        cvDropText.textContent = cvInput.files[0].name;
        cvDrop.classList.add("has-file");
    } else {
        cvDropText.textContent = "Trascina qui il tuo CV o clicca per selezionarlo";
        cvDrop.classList.remove("has-file");
    }
});

certForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    certError.hidden = true;

    const certificazioneFile = certificazioneInput.files[0];
    const codice = document.getElementById("c-codice").value.trim();
    const ente = document.getElementById("c-ente").value.trim();
    const rilascio = document.getElementById("c-rilascio").value;
    const scadenza = document.getElementById("c-scadenza").value;
    const cvFile = cvInput.files[0];

    if(!certificazioneFile || !codice || !ente || !rilascio || !scadenza || !cvFile){
        certError.hidden = false;
        return;
    }


    const oggi = new Date();
    oggi.setHours(0, 0, 0, 0);

    const dataScadenza = new Date(scadenza + "T00:00:00");

    // Verifica che la data di scadenza sia successiva a quella di rilascio
    if(dataScadenza <= new Date(rilascio + "T00:00:00")){
        certError.textContent = "La data di scadenza deve essere successiva alla data di rilascio.";
        certError.hidden = false;
        return;
    }

    // Verifica se la certificazione è già scaduta (deve essere maggiore di oggi)
    if(dataScadenza < oggi){
        certError.textContent = "La data di scadenza deve essere maggiore o uguale alla data attuale.";
        certError.hidden = false;
        return;
    }

    const fd = new FormData();
    fd.append('codice', codice);
    fd.append('ente_rilascio', ente);
    fd.append('data_rilascio', rilascio);
    fd.append('data_scadenza', scadenza);
    fd.append('certificazione', certificazioneFile);
    fd.append('cv', cvFile);

    const r = await api(`/api/trainer/${utente.id}/certificazione`, { method: 'POST', body: fd });
    if(!r.ok){ certError.textContent = r.data.errore || "Errore salvataggio certificazione."; certError.hidden = false; return; }
    console.log(certError.textContent);
    // Aggiorna chip
    const chipText = document.getElementById("trainerChipText");
    chipText.textContent = `${trainerProfile.nome} ${trainerProfile.cognome} · Trainer · Certificato`;
    document.getElementById("trainerChip").hidden = false;

    document.getElementById("gate").hidden = true;
    document.getElementById("dashboard").hidden = false;
    await caricaClienti();
    renderClients();
});

/* =========================================================
   TABELLA CLIENTI
   ========================================================= */
function renderClients(){
    const body = document.getElementById("clientsBody");
    body.innerHTML = "";
    clients.forEach(c => {
        const tr = document.createElement("tr");
        const count = c.n_piani || 0;
        tr.innerHTML = `
            <td><div class="client-name"><span class="client-avatar">${initials(c.nome,c.cognome)}</span><strong>${c.nome} ${c.cognome}</strong></div></td>
            <td class="client-email">${c.email}</td>
            <td>${formatDate(c.data_registrazione)}</td>
            <td><span class="plan-count ${count===0?"zero":""}">${count}</span></td>
            <td>
                <div class="row-actions">
                    <button class="btn btn-ghost btn-small" data-view="${c.id}" ${count===0?"disabled style='opacity:.4;pointer-events:none;'":""}>Vedi piani</button>
                    <button class="btn btn-ghost btn-small" data-progress="${c.id}">Progressi</button>
                    <button class="btn btn-solid btn-small" data-plan="${c.id}">Crea piano</button>
                </div>
            </td>
        `;
        body.appendChild(tr);
    });
    body.querySelectorAll("[data-plan]").forEach(btn => btn.addEventListener("click", () => openPlanModal(Number(btn.dataset.plan))));
    body.querySelectorAll("[data-view]").forEach(btn => btn.addEventListener("click", () => openViewModal(Number(btn.dataset.view))));
    body.querySelectorAll("[data-progress]").forEach(btn => btn.addEventListener("click", () => openProgressModal(Number(btn.dataset.progress))));
}

/* =========================================================
   MODAL: NUOVO PIANO ALLENAMENTO
   ========================================================= */
const planOverlay = document.getElementById("planOverlay");
const planForm = document.getElementById("planForm");
const exerciseList = document.getElementById("exerciseList");
const noExerciseHint = document.getElementById("noExerciseHint");
const planError = document.getElementById("planError");
const exerciseTemplate = document.getElementById("exerciseTemplate");

function selectValue(select, value){
    if(value === null || value === undefined) return;
    const str = String(value);
    select.value = str;
    if(select.value === str) return;
    const opt = document.createElement("option");
    opt.value = str;
    opt.textContent = str;
    select.appendChild(opt);
    select.value = str;
}

function openPlanModal(clientId, plan){
    activeClientId = clientId;
    editingPlanId = plan ? plan.id : null;
    const client = getClient(clientId);
    document.getElementById("planClientName").textContent = client ? `${client.nome} ${client.cognome}` : "—";
    document.getElementById("planEyebrow").textContent = plan ? "Modifica piano per" : "Nuovo piano per";
    document.getElementById("planTitle").textContent = plan ? "Aggiorna il piano di allenamento." : "Costruisci l'allenamento.";
    document.getElementById("planSubmitBtn").textContent = plan ? "Salva modifiche" : "Salva piano";
    planForm.reset();
    exerciseList.innerHTML = "";
    exerciseCounter = 0;
    planError.hidden = true;
    planError.textContent = "";
    if(plan){
        document.getElementById("p-nome").value = plan.nome || "";
        selectValue(document.getElementById("p-obiettivo"), plan.obiettivo);
        selectValue(document.getElementById("p-livello"), plan.livello_difficolta);
        document.getElementById("p-durata").value = plan.durata_settimane || "";
        document.getElementById("p-descrizione").value = plan.descrizione || "";
        (plan.esercizi || []).forEach(ex => fillExerciseCard(addExerciseCard(), ex));
    }
    updateExerciseHint();
    planOverlay.classList.add("open");
    document.body.style.overflow = "hidden";
}
function closePlanModal(){ planOverlay.classList.remove("open"); document.body.style.overflow = ""; }
document.getElementById("planClose").addEventListener("click", closePlanModal);
planOverlay.addEventListener("click", (e) => { if(e.target === planOverlay) closePlanModal(); });

function fillExerciseCard(card, ex){
    const set = (f, v) => { card.querySelector(`[data-field="${f}"]`).value = (v === null || v === undefined) ? "" : v; };
    set("nome", ex.nome);
    set("descrizione", ex.descrizione);
    selectValue(card.querySelector('[data-field="gruppo"]'), ex.gruppo_muscolare);
    set("video", ex.url_video);
    set("serie", ex.serie);
    set("ripetizioni", ex.ripetizioni);
    set("recupero", ex.recupero_sec);
}

function updateExerciseHint(){ noExerciseHint.hidden = exerciseList.children.length > 0; }
function renumberExercises(){
    exerciseList.querySelectorAll("[data-ex]").forEach((card, i) => { card.querySelector("[data-order]").textContent = i + 1; });
}

function addExerciseCard(){
    exerciseCounter++;
    const node = exerciseTemplate.content.cloneNode(true);
    const card = node.querySelector("[data-ex]");
    card.dataset.id = exerciseCounter;
    card.querySelector("[data-remove]").addEventListener("click", () => { card.remove(); renumberExercises(); updateExerciseHint(); });
    card.querySelector('[data-move="up"]').addEventListener("click", () => { const prev = card.previousElementSibling; if(prev) exerciseList.insertBefore(card, prev); renumberExercises(); });
    card.querySelector('[data-move="down"]').addEventListener("click", () => { const next = card.nextElementSibling; if(next) exerciseList.insertBefore(next, card); renumberExercises(); });
    exerciseList.appendChild(card);
    renumberExercises();
    updateExerciseHint();
    return card;
}
document.getElementById("addExercise").addEventListener("click", addExerciseCard);

planForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    planError.hidden = true;
    const nome = document.getElementById("p-nome").value.trim();
    const obiettivo = document.getElementById("p-obiettivo").value;
    const livello = document.getElementById("p-livello").value;
    const durata = document.getElementById("p-durata").value;
    const descrizione = document.getElementById("p-descrizione").value.trim();
    const cards = [...exerciseList.querySelectorAll("[data-ex]")];
    if(!nome || !obiettivo || !livello || !durata || !descrizione || cards.length === 0){
        planError.textContent = "Compila tutti i campi del piano e aggiungi almeno un esercizio.";
        planError.hidden = false;
        return;
    }
    const esercizi = [];
    for(const card of cards){
        const get = f => card.querySelector(`[data-field="${f}"]`).value.trim();
        const eNome = get("nome"), eDesc = get("descrizione"), eGruppo = get("gruppo"), eVideo = get("video"), eSerie = get("serie"), eRip = get("ripetizioni"), eRecupero = get("recupero");
        if(!eNome || !eDesc || !eGruppo || !eSerie || !eRip || eRecupero===""){ planError.textContent = `Compila tutti i campi dell'esercizio n° ${card.querySelector("[data-order]").textContent}.`; planError.hidden = false; return; }
        esercizi.push({
            nome: eNome, descrizione: eDesc, gruppo_muscolare: eGruppo, url_video: eVideo,
            serie: Number(eSerie), ripetizioni: eRip, recupero_sec: Number(eRecupero)
        });
    }
    const url = editingPlanId
        ? `/api/trainer/${utente.id}/clienti/${activeClientId}/piani/${editingPlanId}`
        : `/api/trainer/${utente.id}/clienti/${activeClientId}/piani`;
    const r = await api(url, {
        method: editingPlanId ? 'PUT' : 'POST', headers: {'Content-Type':'application/json'},
        body: JSON.stringify({nome, obiettivo, livello_difficolta: livello, durata_settimane: Number(durata), descrizione, esercizi})
    });
    if(!r.ok){ planError.textContent = r.data.errore || "Errore salvataggio piano."; planError.hidden = false; return; }
    closePlanModal();
    await caricaClienti();
    renderClients();
});

/* =========================================================
   MODAL: VEDI PIANI
   ========================================================= */
const viewOverlay = document.getElementById("viewOverlay");

function renderStars(valutazione) {
    return "★".repeat(valutazione) + "☆".repeat(5 - valutazione);
}

async function caricaFeedbackPiano(clientId, pianoId, containerId) {
    const el = document.getElementById(containerId);
    if (!el) return;
    const r = await api(`/api/trainer/${utente.id}/clienti/${clientId}/piani/${pianoId}/feedback`);
    if (!r.ok) { el.innerHTML = `<p class="plan-empty" style="padding:10px 0;">Impossibile caricare i feedback.</p>`; return; }
    const feedbacks = r.data.feedback || [];
    if (feedbacks.length === 0) {
        el.innerHTML = `<p class="plan-empty" style="padding:10px 0;">Nessun feedback ancora.</p>`;
        return;
    }
    el.innerHTML = feedbacks.map(fb => `
        <div class="feedback-item">
            <div class="feedback-head">
                <span class="feedback-cliente">${fb.nome_cliente}</span>
                <span class="feedback-stars">${renderStars(fb.valutazione)}</span>
                <span class="feedback-date">${formatDate(fb.data)}</span>
            </div>
            ${fb.commento ? `<p class="feedback-commento">${fb.commento}</p>` : ""}
        </div>
    `).join("");
}

async function openViewModal(clientId){
    const client = getClient(clientId);
    document.getElementById("viewClientName").textContent = client ? `${client.nome} ${client.cognome}` : "—";
    const container = document.getElementById("plansList");
    container.innerHTML = `<p class="plan-empty" style="text-align:center; padding:20px;">Caricamento...</p>`;
    viewOverlay.classList.add("open");
    document.body.style.overflow = "hidden";

    const r = await api(`/api/trainer/${utente.id}/clienti/${clientId}/piani`);
    const piani = r.ok ? (r.data.piani || []) : [];
    container.innerHTML = "";
    if(piani.length === 0){
        container.innerHTML = `<p class="plan-empty">Nessun piano creato per questo cliente.</p>`;
        return;
    }
    piani.forEach(p => {
        const div = document.createElement("div");
        div.className = "plan-card";
        const esercizi = p.esercizi || [];
        const isMio = p.id_trainer === utente.id;
        div.innerHTML = `
            <div class="plan-card-head">
                <h5>${p.nome}</h5>
                <div class="plan-tags">
                    <span class="plan-tag">${p.obiettivo}</span>
                    <span class="plan-tag">${p.livello_difficolta}</span>
                    <span class="plan-tag">${p.durata_settimane} settimane</span>
                </div>
            </div>
            <p class="plan-desc">${p.descrizione || ""}</p>
            ${isMio ? `
            <div class="row-actions plan-actions">
                <button class="btn btn-ghost btn-small" data-edit="${p.id}">Modifica</button>
                <button class="btn btn-ghost btn-small" data-del="${p.id}">Elimina</button>
            </div>` : ''}
            ${esercizi.map(ex => `
                <div class="plan-ex-row">
                    <span class="ex-order">${ex.ordine}</span>
                    <div class="plan-ex-body">
                        <strong>${ex.nome || "Esercizio"}${ex.url_video ? ` — <a href="${ex.url_video}" target="_blank" rel="noopener" style="color:var(--mint);">video</a>` : ""}</strong>
                        <span>${ex.gruppo_muscolare || ""} · ${ex.descrizione || ""}</span>
                    </div>
                    <div class="plan-ex-meta">${ex.serie} serie × ${ex.ripetizioni}<br>${ex.recupero_sec}s recupero</div>
                </div>
            `).join("")}
            <div class="feedback-section">
                <h4 class="feedback-section-title">Feedback dei clienti</h4>
                <div id="feedback-trainer-${p.id}"><p class="plan-empty" style="padding:10px 0;">Caricamento feedback...</p></div>
            </div>
        `;
        if(isMio){
            div.querySelector(`[data-edit="${p.id}"]`).addEventListener("click", () => { closeViewModal(); openPlanModal(clientId, p); });
            div.querySelector(`[data-del="${p.id}"]`).addEventListener("click", async () => {
                if(!confirm(`Eliminare il piano "${p.nome}"?`)) return;
                const r = await api(`/api/trainer/${utente.id}/clienti/${clientId}/piani/${p.id}`, { method: 'DELETE' });
                await caricaClienti();
                renderClients();
                if(r.ok) openViewModal(clientId);
                else alert(r.data.errore || "Errore eliminazione piano.");
            });
        }
        container.appendChild(div);

        // Carica feedback per questo programma
        caricaFeedbackPiano(clientId, p.id, `feedback-trainer-${p.id}`);
    });
}
document.getElementById("viewClose").addEventListener("click", closeViewModal);
viewOverlay.addEventListener("click", (e) => { if(e.target === viewOverlay) closeViewModal(); });
function closeViewModal(){ viewOverlay.classList.remove("open"); document.body.style.overflow = ""; }

/* =========================================================
   MODAL: PROGRESSI CLIENTE
   ========================================================= */
const progressOverlay = document.getElementById("progressOverlay");

async function openProgressModal(clientId){
    const client = getClient(clientId);
    document.getElementById("progressClientName").textContent = client ? `${client.nome} ${client.cognome}` : "—";
    const body = document.getElementById("progressBody");
    body.innerHTML = `<p class="plan-empty" style="text-align:center; padding:20px;">Caricamento...</p>`;
    progressOverlay.classList.add("open");
    document.body.style.overflow = "hidden";

    const r = await api(`/api/trainer/${utente.id}/clienti/${clientId}/progressi`);
    if(!r.ok){
        body.innerHTML = `<p class="plan-empty">Impossibile caricare i progressi.</p>`;
        return;
    }
    const rip = r.data.riepilogo || {};
    const perProg = r.data.per_programma || [];
    const pct = v => (v === null || v === undefined) ? "—" : `${v}%`;

    body.innerHTML = `
        <div class="progress-stats">
            <div class="progress-stat"><span class="ps-num">${rip.sessioni_totali ?? 0}</span><span class="ps-label">Sessioni totali</span></div>
            <div class="progress-stat"><span class="ps-num">${rip.minuti_totali ?? 0}</span><span class="ps-label">Minuti allenati</span></div>
            <div class="progress-stat"><span class="ps-num">${rip.media_settimanale ?? 0}</span><span class="ps-label">Sessioni / settimana</span></div>
            <div class="progress-stat"><span class="ps-num">${pct(rip.partecipazione)}</span><span class="ps-label">Partecipazione<br>(sessioni completate)</span></div>
        </div>
        ${(rip.prima_sessione || rip.ultima_sessione) ? `
        <p class="dash-sub" style="margin-top:14px;">
            ${rip.prima_sessione ? `Prima sessione: ${formatDate(rip.prima_sessione)}` : ""}${rip.prima_sessione && rip.ultima_sessione ? " · " : ""}${rip.ultima_sessione ? `Ultima sessione: ${formatDate(rip.ultima_sessione)}` : ""}
        </p>` : ""}
        <h4 class="progress-caption">Dettaglio per programma</h4>
        ${perProg.length === 0
            ? `<p class="plan-empty">Nessuna sessione registrata per questo cliente.</p>`
            : `<div class="table-card"><table class="clients-table">
                <thead><tr><th>Programma</th><th>Sessioni</th><th>Completate</th><th>Minuti</th><th>Partecipazione</th></tr></thead>
                <tbody>
                ${perProg.map(pp => `
                    <tr>
                        <td><strong>${pp.nome}</strong></td>
                        <td>${pp.sessioni_totali}</td>
                        <td>${pp.sessioni_completate}</td>
                        <td>${pp.minuti_totali}</td>
                        <td>${pct(pp.partecipazione)}</td>
                    </tr>`).join("")}
                </tbody>
            </table></div>`}
    `;
}
function closeProgressModal(){ progressOverlay.classList.remove("open"); document.body.style.overflow = ""; }
document.getElementById("progressClose").addEventListener("click", closeProgressModal);
progressOverlay.addEventListener("click", (e) => { if(e.target === progressOverlay) closeProgressModal(); });

document.addEventListener("keydown", (e) => {
    if(e.key === "Escape"){ closePlanModal(); closeViewModal(); closeProgressModal(); }
});
