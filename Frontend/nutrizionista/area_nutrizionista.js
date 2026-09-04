/* =========================================================
   area_nutrizionista.js — Area Nutrizionista collegata al backend REST
   ========================================================= */

let utente = null;
let nutriProfile = null;
let clients = [];
let activeClientId = null;
let pastoCounter = 0;

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

const giorniOrdine = ["Lunedì","Martedì","Mercoledì","Giovedì","Venerdì","Sabato","Domenica"];
const giornoDaNumero = n => giorniOrdine[n-1] || giorniOrdine[0];

/* =========================================================
   INIT
   ========================================================= */
utente = getUtente();
if(!utente || !utente.id) redirectHome();
else init();

async function init(){
    const r = await api(`/api/nutrizionista/${utente.id}`);
    if(!r.ok){ redirectHome(); return; }
    nutriProfile = r.data;

    const chip = document.getElementById("nutriChip");
    const chipText = document.getElementById("nutriChipText");
    chipText.textContent = `${nutriProfile.nome} ${nutriProfile.cognome} · Nutrizionista`;
    chip.hidden = false;

    const cert = await api(`/api/nutrizionista/${utente.id}/certificazione`);
    if(cert.ok && cert.data.id){
        document.getElementById("gate").style.display = "none";
        document.getElementById("dashboard").hidden = false;
        await caricaClienti();
        renderClients();
    } else {
        document.getElementById("gate").style.display = "";
        document.getElementById("dashboard").hidden = true;
    }
}

async function caricaClienti(){
    const r = await api(`/api/nutrizionista/${utente.id}/clienti`);
    if(!r.ok) return;
    clients = (r.data.clienti || []).map(c => ({ ...c, piani: [] }));
}

/* =========================================================
   GATE — CERTIFICAZIONE NUTRIZIONISTA
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
    certError.textContent = "";

    const professione = document.getElementById("c-professione").value;
    const certificazioneFile = certificazioneInput.files[0];
    const codice = document.getElementById("c-codice").value.trim();
    const ente = document.getElementById("c-ente").value.trim();
    const rilascio = document.getElementById("c-rilascio").value;
    const scadenza = document.getElementById("c-scadenza").value;
    const cvFile = cvInput.files[0];

    const mancanti = [];
    if(!professione) mancanti.push("Professione");
    if(!certificazioneFile) mancanti.push("Certificazione allegata");
    if(!codice) mancanti.push("Codice certificazione");
    if(!ente) mancanti.push("Ente di rilascio");
    if(!rilascio) mancanti.push("Data rilascio");
    if(!scadenza) mancanti.push("Data scadenza");
    if(!cvFile) mancanti.push("CV allegato");
    if(rilascio && scadenza && new Date(scadenza) <= new Date(rilascio)) mancanti.push("Data scadenza deve essere DOPO la data rilascio");

    if(mancanti.length > 0){
        certError.textContent = "Controlla: " + mancanti.join(", ");
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

    const r = await api(`/api/nutrizionista/${utente.id}/certificazione`, { method: 'POST', body: fd });
    if(!r.ok){ certError.textContent = r.data.errore || "Errore salvataggio."; certError.hidden = false; return; }

    const chipText = document.getElementById("nutriChipText");
    const labels = {"nutrizionista":"Nutrizionista","biologo-nutrizionista":"Biologo Nutrizionista","dietista":"Dietista","medico-nutrizionista":"Medico Nutrizionista"};
    chipText.textContent = `${labels[professione] || professione} · Certificato`;
    document.getElementById("nutriChip").hidden = false;

    document.getElementById("gate").style.display = "none";
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
   MODAL: NUOVO PIANO ALIMENTARE
   ========================================================= */
const planOverlay = document.getElementById("planOverlay");
const planForm = document.getElementById("planForm");
const pastoList = document.getElementById("pastoList");
const noPastoHint = document.getElementById("noPastoHint");
const planError = document.getElementById("planError");
const pastoTemplate = document.getElementById("pastoTemplate");
const alimentoTemplate = document.getElementById("alimentoTemplate");

function openPlanModal(clientId){
    activeClientId = clientId;
    const client = getClient(clientId);
    document.getElementById("planClientName").textContent = client ? `${client.nome} ${client.cognome}` : "—";
    planForm.reset();
    pastoList.innerHTML = "";
    pastoCounter = 0;
    planError.hidden = true;
    planError.textContent = "";
    updatePastoHint();
    planOverlay.classList.add("open");
    document.body.style.overflow = "hidden";
}
function closePlanModal(){ planOverlay.classList.remove("open"); document.body.style.overflow = ""; }
document.getElementById("planClose").addEventListener("click", closePlanModal);
planOverlay.addEventListener("click", (e) => { if(e.target === planOverlay) closePlanModal(); });

function updatePastoHint(){ noPastoHint.hidden = pastoList.children.length > 0; }
function renumberPasti(){ pastoList.querySelectorAll("[data-pasto]").forEach((card, i) => { card.querySelector("[data-order]").textContent = i + 1; }); }

function updateFoodHint(pastoCard){
    const foodList = pastoCard.querySelector("[data-food-list]");
    const hint = pastoCard.querySelector("[data-food-hint]");
    hint.hidden = foodList.children.length > 0;
}

function addAlimentoRow(pastoCard){
    const node = alimentoTemplate.content.cloneNode(true);
    const row = node.querySelector("[data-food]");
    row.querySelector("[data-remove-food]").addEventListener("click", () => { row.remove(); updateFoodHint(pastoCard); });
    pastoCard.querySelector("[data-food-list]").appendChild(row);
    updateFoodHint(pastoCard);
}

function addPastoCard(){
    pastoCounter++;
    const node = pastoTemplate.content.cloneNode(true);
    const card = node.querySelector("[data-pasto]");
    card.dataset.id = pastoCounter;
    card.querySelector("[data-remove]").addEventListener("click", () => { card.remove(); renumberPasti(); updatePastoHint(); });
    card.querySelector('[data-move="up"]').addEventListener("click", () => { const prev = card.previousElementSibling; if(prev) pastoList.insertBefore(card, prev); renumberPasti(); });
    card.querySelector('[data-move="down"]').addEventListener("click", () => { const next = card.nextElementSibling; if(next) pastoList.insertBefore(next, card); renumberPasti(); });
    card.querySelector("[data-add-food]").addEventListener("click", () => addAlimentoRow(card));
    pastoList.appendChild(card);
    renumberPasti();
    updatePastoHint();
    addAlimentoRow(card);
}
document.getElementById("addPasto").addEventListener("click", addPastoCard);

planForm.addEventListener("submit", async (e) => {
    e.preventDefault();
    planError.hidden = true;
    planError.textContent = "";
    const nome = document.getElementById("p-nome").value.trim();
    const descrizione = document.getElementById("p-descrizione").value.trim();
    const pastoCards = [...pastoList.querySelectorAll("[data-pasto]")];
    if(!nome || !descrizione || pastoCards.length === 0){
        planError.hidden = false;
        return;
    }
    const pasti = [];
    for(const card of pastoCards){
        const giorno = card.querySelector('[data-field="giorno"]').value;
        const tipo = card.querySelector('[data-field="tipo"]').value;
        const foodRows = [...card.querySelectorAll("[data-food]")];
        if(!giorno || !tipo || foodRows.length === 0){
            planError.textContent = "Compila tutti i campi dei pasti e degli alimenti.";
            planError.hidden = false;
            return;
        }
        const alimenti = [];
        for(const row of foodRows){
            const cibo = row.querySelector('[data-field="cibo"]').value.trim();
            const quantita = row.querySelector('[data-field="quantita"]').value;
            if(!cibo || quantita === ""){
                planError.textContent = "Compila tutti gli alimenti.";
                planError.hidden = false;
                return;
            }
            alimenti.push({ cibo, quantita_gr: Number(quantita) });
        }
        pasti.push({ giorno, tipo_pasto: tipo, alimenti });
    }
    const r = await api(`/api/nutrizionista/${utente.id}/clienti/${activeClientId}/piani`, {
        method: 'POST', headers: {'Content-Type':'application/json'},
        body: JSON.stringify({nome, descrizione, pasti})
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

    const r = await api(`/api/nutrizionista/${utente.id}/clienti/${clientId}/piani`);
    const piani = r.ok ? (r.data.piani || []) : [];
    container.innerHTML = "";
    if(piani.length === 0){
        container.innerHTML = `<p class="plan-empty">Nessun piano creato per questo cliente.</p>`;
        return;
    }
    piani.forEach(p => {
        const div = document.createElement("div");
        div.className = "plan-card";
        const pasti = p.pasti || [];
        div.innerHTML = `
            <div class="plan-card-head">
                <h5>${p.nome}</h5>
                <div class="plan-tags">
                    <span class="plan-tag">${pasti.length} pasti</span>
                </div>
            </div>
            <p class="plan-desc">${p.descrizione || ""}</p>
            ${pasti.map(pasto => `
                <div class="plan-ex-row">
                    <span class="ex-order">${giornoDaNumero(pasto.giorno)} · ${pasto.tipo_pasto}</span>
                    <div class="plan-ex-body">
                        <span>${(pasto.alimenti || []).map(a => `${a.cibo} (${a.quantita_gr} g)`).join(" · ") || "—"}</span>
                    </div>
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
