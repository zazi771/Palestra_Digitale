/* =========================================================
   DEMO STATICO — nessun dato viene inviato a un server.
   Tutti i dati (nutrizionista, clienti, piani) vivono solo in memoria
   e verranno sostituiti in seguito da chiamate al backend/DB.
   ========================================================= */

/* ---------- STATO IN MEMORIA ---------- */
let nutrizionista = null; // { professione, certificazioneName, codice, ente, rilascio, scadenza, cvName }

// clienti "registrati sul sito" — dati mock, in attesa del backend
let clients = [
    { id: 1, nome: "Marco", cognome: "Ricci", email: "marco.ricci@example.it", iscritto: "2026-02-11", piani: [] },
    { id: 2, nome: "Giulia", cognome: "Ferraro", email: "giulia.ferraro@example.it", iscritto: "2026-03-04", piani: [] },
    { id: 3, nome: "Antonio", cognome: "De Luca", email: "antonio.deluca@example.it", iscritto: "2026-04-22", piani: [] },
    { id: 4, nome: "Sara", cognome: "Bianchi", email: "sara.bianchi@example.it", iscritto: "2026-06-09", piani: [] }
];

let activeClientId = null;
let pastoCounter = 0;

/* ---------- UTIL ---------- */
function formatDate(iso){
    if(!iso) return "—";
    const d = new Date(iso + "T00:00:00");
    return d.toLocaleDateString("it-IT", { day:"2-digit", month:"short", year:"numeric" });
}
function initials(nome, cognome){
    return (nome[0] || "").toUpperCase() + (cognome[0] || "").toUpperCase();
}
function getClient(id){
    return clients.find(c => c.id === id);
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

certForm.addEventListener("submit", (e) => {
    e.preventDefault();
    certError.hidden = true;

    const professione = document.getElementById("c-professione").value;
    const certificazioneFile = certificazioneInput.files[0];
    const codice = document.getElementById("c-codice").value.trim();
    const ente = document.getElementById("c-ente").value.trim();
    const rilascio = document.getElementById("c-rilascio").value;
    const scadenza = document.getElementById("c-scadenza").value;
    const cvFile = cvInput.files[0];

    // elenca esattamente cosa manca, cosi' non fallisce mai in silenzio
    const mancanti = [];
    if(!professione) mancanti.push("Professione");
    if(!certificazioneFile) mancanti.push("Certificazione allegata");
    if(!codice) mancanti.push("Codice certificazione");
    if(!ente) mancanti.push("Ente di rilascio");
    if(!rilascio) mancanti.push("Data rilascio");
    if(!scadenza) mancanti.push("Data scadenza");
    if(!cvFile) mancanti.push("CV allegato");
    if(rilascio && scadenza && new Date(scadenza) <= new Date(rilascio)){
        mancanti.push("Data scadenza deve essere DOPO la data rilascio");
    }

    if(mancanti.length > 0){
        certError.textContent = "Controlla questi campi: " + mancanti.join(", ");
        certError.hidden = false;
        return;
    }

    nutrizionista = {
        professione, certificazioneName: certificazioneFile.name,
        codice, ente, rilascio, scadenza, cvName: cvFile.name
    };

    // aggiorna chip in header
    const chip = document.getElementById("nutriChip");
    const chipText = document.getElementById("nutriChipText");
    const professionLabels = {
        "nutrizionista": "Nutrizionista",
        "biologo-nutrizionista": "Biologo Nutrizionista",
        "dietista": "Dietista",
        "medico-nutrizionista": "Medico Nutrizionista"
    };
    chipText.textContent = `${professionLabels[professione] || professione} · Certificato`;
    chip.hidden = false;

    document.getElementById("gate").style.display = "none";
    document.getElementById("dashboard").hidden = false;
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
        const count = c.piani.length;

        tr.innerHTML = `
            <td>
                <div class="client-name">
                    <span class="client-avatar">${initials(c.nome, c.cognome)}</span>
                    <strong>${c.nome} ${c.cognome}</strong>
                </div>
            </td>
            <td class="client-email">${c.email}</td>
            <td>${formatDate(c.iscritto)}</td>
            <td><span class="plan-count ${count === 0 ? "zero" : ""}">${count}</span></td>
            <td>
                <div class="row-actions">
                    <button class="btn btn-ghost btn-small" data-view="${c.id}" ${count === 0 ? "disabled style='opacity:.4;pointer-events:none;'" : ""}>Vedi piani</button>
                    <button class="btn btn-solid btn-small" data-plan="${c.id}">Crea piano</button>
                </div>
            </td>
        `;
        body.appendChild(tr);
    });

    body.querySelectorAll("[data-plan]").forEach(btn => {
        btn.addEventListener("click", () => openPlanModal(Number(btn.dataset.plan)));
    });
    body.querySelectorAll("[data-view]").forEach(btn => {
        btn.addEventListener("click", () => openViewModal(Number(btn.dataset.view)));
    });
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
    document.getElementById("planClientName").textContent = `${client.nome} ${client.cognome}`;

    planForm.reset();
    pastoList.innerHTML = "";
    pastoCounter = 0;
    planError.hidden = true;
    updatePastoHint();

    planOverlay.classList.add("open");
    document.body.style.overflow = "hidden";
}
function closePlanModal(){
    planOverlay.classList.remove("open");
    document.body.style.overflow = "";
}
document.getElementById("planClose").addEventListener("click", closePlanModal);
planOverlay.addEventListener("click", (e) => { if(e.target === planOverlay) closePlanModal(); });

function updatePastoHint(){
    noPastoHint.hidden = pastoList.children.length > 0;
}

function renumberPasti(){
    pastoList.querySelectorAll("[data-pasto]").forEach((card, i) => {
        card.querySelector("[data-order]").textContent = i + 1;
    });
}

/* ---- ALIMENTI dentro un pasto ---- */
function updateFoodHint(pastoCard){
    const foodList = pastoCard.querySelector("[data-food-list]");
    const hint = pastoCard.querySelector("[data-food-hint]");
    hint.hidden = foodList.children.length > 0;
}

function addAlimentoRow(pastoCard){
    const node = alimentoTemplate.content.cloneNode(true);
    const row = node.querySelector("[data-food]");

    row.querySelector("[data-remove-food]").addEventListener("click", () => {
        row.remove();
        updateFoodHint(pastoCard);
    });

    pastoCard.querySelector("[data-food-list]").appendChild(row);
    updateFoodHint(pastoCard);
}

/* ---- PASTI ---- */
function addPastoCard(){
    pastoCounter++;
    const node = pastoTemplate.content.cloneNode(true);
    const card = node.querySelector("[data-pasto]");
    card.dataset.id = pastoCounter;

    card.querySelector("[data-remove]").addEventListener("click", () => {
        card.remove();
        renumberPasti();
        updatePastoHint();
    });
    card.querySelector('[data-move="up"]').addEventListener("click", () => {
        const prev = card.previousElementSibling;
        if(prev) pastoList.insertBefore(card, prev);
        renumberPasti();
    });
    card.querySelector('[data-move="down"]').addEventListener("click", () => {
        const next = card.nextElementSibling;
        if(next) pastoList.insertBefore(next, card);
        renumberPasti();
    });
    card.querySelector("[data-add-food]").addEventListener("click", () => addAlimentoRow(card));

    pastoList.appendChild(card);
    renumberPasti();
    updatePastoHint();

    // ogni pasto parte con un alimento gia' pronto da compilare
    addAlimentoRow(card);
}
document.getElementById("addPasto").addEventListener("click", addPastoCard);

planForm.addEventListener("submit", (e) => {
    e.preventDefault();
    planError.hidden = true;

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
            planError.hidden = false;
            return;
        }

        const alimenti = [];
        for(const row of foodRows){
            const cibo = row.querySelector('[data-field="cibo"]').value.trim();
            const quantita = row.querySelector('[data-field="quantita"]').value;
            if(!cibo || quantita === ""){
                planError.hidden = false;
                return;
            }
            alimenti.push({ cibo, quantita: Number(quantita) });
        }

        pasti.push({ ordine: pasti.length + 1, giorno, tipo, alimenti });
    }

    const client = getClient(activeClientId);
    client.piani.push({
        id: Date.now(),
        nome, descrizione,
        pasti,
        creato: new Date().toISOString().slice(0,10)
    });

    closePlanModal();
    renderClients();
    alert(`Piano "${nome}" creato per ${client.nome} ${client.cognome} — questa è una demo statica, nessun dato viene inviato.`);
});

/* =========================================================
   MODAL: VEDI PIANI
   ========================================================= */
const viewOverlay = document.getElementById("viewOverlay");

function openViewModal(clientId){
    const client = getClient(clientId);
    document.getElementById("viewClientName").textContent = `${client.nome} ${client.cognome}`;

    const container = document.getElementById("plansList");
    container.innerHTML = "";

    if(client.piani.length === 0){
        container.innerHTML = `<p class="plan-empty">Nessun piano creato per questo cliente.</p>`;
    } else {
        client.piani.forEach(p => {
            const div = document.createElement("div");
            div.className = "plan-card";
            div.innerHTML = `
                <div class="plan-card-head">
                    <h5>${p.nome}</h5>
                    <div class="plan-tags">
                        <span class="plan-tag">${p.pasti.length} pasti</span>
                    </div>
                </div>
                <p class="plan-desc">${p.descrizione}</p>
                ${p.pasti.map(pasto => `
                    <div class="plan-ex-row">
                        <span class="ex-order">${pasto.ordine}</span>
                        <div class="plan-ex-body">
                            <strong>${pasto.giorno} — ${pasto.tipo}</strong>
                            <span>${pasto.alimenti.map(a => `${a.cibo} (${a.quantita} g)`).join(" · ")}</span>
                        </div>
                    </div>
                `).join("")}
            `;
            container.appendChild(div);
        });
    }

    viewOverlay.classList.add("open");
    document.body.style.overflow = "hidden";
}
document.getElementById("viewClose").addEventListener("click", closeViewModal);
viewOverlay.addEventListener("click", (e) => { if(e.target === viewOverlay) closeViewModal(); });
function closeViewModal(){
    viewOverlay.classList.remove("open");
    document.body.style.overflow = "";
}

document.addEventListener("keydown", (e) => {
    if(e.key === "Escape"){
        closePlanModal();
        closeViewModal();
    }
});