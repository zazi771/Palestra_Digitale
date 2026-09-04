/* =========================================================
   area_trainer.js — Area Trainer collegata al backend REST
   ========================================================= */

let utente = null;
let trainerProfile = null;
let clients = [];
let activeClientId = null;
let exerciseCounter = 0;

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

    // Verifica se ha già certificazione
    const cert = await api(`/api/trainer/${utente.id}/certificazione`);
    if(cert.ok && cert.data.id){
        // Ha già certificazione → salta gate
        document.getElementById("gate").hidden = true;
        document.getElementById("dashboard").hidden = false;
        await caricaClienti();
        renderClients();
    } else {
        // Mostra gate
        document.getElementById("gate").hidden = false;
        document.getElementById("dashboard").hidden = true;
    }
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

    const professione = document.getElementById("c-professione").value;
    const certificazioneFile = certificazioneInput.files[0];
    const codice = document.getElementById("c-codice").value.trim();
    const ente = document.getElementById("c-ente").value.trim();
    const rilascio = document.getElementById("c-rilascio").value;
    const scadenza = document.getElementById("c-scadenza").value;
    const cvFile = cvInput.files[0];

    if(!professione || !certificazioneFile || !codice || !ente || !rilascio || !scadenza || !cvFile){
        certError.hidden = false;
        return;
    }
    if(new Date(scadenza) <= new Date(rilascio)){
        certError.textContent = "La data di scadenza deve essere successiva alla data di rilascio.";
        certError.hidden = false;
        return;
    }

    const fd = new FormData();
    fd.append('professione', professione);
    fd.append('codice', codice);
    fd.append('ente_rilascio', ente);
    fd.append('data_rilascio', rilascio);
    fd.append('data_scadenza', scadenza);
    fd.append('certificazione', certificazioneFile);
    fd.append('cv', cvFile);

    const r = await api(`/api/trainer/${utente.id}/certificazione`, { method: 'POST', body: fd });
    if(!r.ok){ certError.textContent = r.data.errore || "Errore salvataggio certificazione."; certError.hidden = false; return; }

    // Aggiorna chip
    const chipText = document.getElementById("trainerChipText");
    const labels = {"personal-trainer":"Personal Trainer","istruttore-fitness":"Istruttore Fitness","preparatore-atletico":"Preparatore Atletico","fisioterapista":"Fisioterapista"};
    chipText.textContent = `${labels[professione] || professione} · Certificato`;
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
                    <button class="btn btn-solid btn-small" data-plan="${c.id}">Crea piano</button>
                </div>
            </td>
        `;
        body.appendChild(tr);
    });
    body.querySelectorAll("[data-plan]").forEach(btn => btn.addEventListener("click", () => openPlanModal(Number(btn.dataset.plan))));
    body.querySelectorAll("[data-view]").forEach(btn => btn.addEventListener("click", () => openViewModal(Number(btn.dataset.view))));
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

function openPlanModal(clientId){
    activeClientId = clientId;
    const client = getClient(clientId);
    document.getElementById("planClientName").textContent = client ? `${client.nome} ${client.cognome}` : "—";
    planForm.reset();
    exerciseList.innerHTML = "";
    exerciseCounter = 0;
    planError.hidden = true;
    updateExerciseHint();
    planOverlay.classList.add("open");
    document.body.style.overflow = "hidden";
}
function closePlanModal(){ planOverlay.classList.remove("open"); document.body.style.overflow = ""; }
document.getElementById("planClose").addEventListener("click", closePlanModal);
planOverlay.addEventListener("click", (e) => { if(e.target === planOverlay) closePlanModal(); });

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
        planError.hidden = false;
        return;
    }
    const esercizi = [];
    for(const card of cards){
        const get = f => card.querySelector(`[data-field="${f}"]`).value.trim();
        const eNome = get("nome"), eDesc = get("descrizione"), eGruppo = get("gruppo"), eVideo = get("video"), eSerie = get("serie"), eRip = get("ripetizioni"), eRecupero = get("recupero");
        if(!eNome || !eDesc || !eGruppo || !eSerie || !eRip || eRecupero===""){ planError.hidden = false; return; }
        esercizi.push({
            nome: eNome, descrizione: eDesc, gruppo_muscolare: eGruppo, url_video: eVideo,
            serie: Number(eSerie), ripetizioni: eRip, recupero_sec: Number(eRecupero)
        });
    }
    const r = await api(`/api/trainer/${utente.id}/clienti/${activeClientId}/piani`, {
        method: 'POST', headers: {'Content-Type':'application/json'},
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
        `;
        container.appendChild(div);
    });
}
document.getElementById("viewClose").addEventListener("click", closeViewModal);
viewOverlay.addEventListener("click", (e) => { if(e.target === viewOverlay) closeViewModal(); });
function closeViewModal(){ viewOverlay.classList.remove("open"); document.body.style.overflow = ""; }

document.addEventListener("keydown", (e) => {
    if(e.key === "Escape"){ closePlanModal(); closeViewModal(); }
});
