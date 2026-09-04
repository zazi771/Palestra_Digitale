/* ---- Gestione modale (login/signup tabs) ---- */
function openModal(which){
    document.getElementById('overlay').classList.add('open');
    switchTab(which);
    document.body.style.overflow = 'hidden';
}
function closeModal(){
    document.getElementById('overlay').classList.remove('open');
    document.body.style.overflow = '';
}
function switchTab(which){
    document.getElementById('tab-login').classList.toggle('active', which==='login');
    document.getElementById('tab-signup').classList.toggle('active', which==='signup');
    document.getElementById('pane-login').classList.toggle('active', which==='login');
    document.getElementById('pane-signup').classList.toggle('active', which==='signup');
}
document.addEventListener('keydown', (e)=>{
    if(e.key === 'Escape') closeModal();
});

/* ---- Redirect per ruolo ---- */
function redirectByRole(ruolo){
    switch(ruolo){
        case 'cliente':      window.location.href = '/cliente/area_cliente.html'; break;
        case 'trainer':      window.location.href = '/trainer/area_trainer.html'; break;
        case 'nutrizionista': window.location.href = '/nutrizionista/area_nutrizionista.html'; break;
        default: window.location.href = '/';
    }
}

function showError(paneId, msg){
    let el = document.getElementById(paneId + '-error');
    if(!el){
        el = document.createElement('p');
        el.id = paneId + '-error';
        el.className = 'form-error';
        el.style.marginTop = '12px';
        document.getElementById(paneId).appendChild(el);
    }
    el.textContent = msg;
    el.hidden = false;
}
function hideError(paneId){
    const el = document.getElementById(paneId + '-error');
    if(el) el.hidden = true;
}

/* ---- LOGIN ---- */
(function(){
    const form = document.getElementById('pane-login').querySelector('form');
    form.addEventListener('submit', async (e)=>{
        e.preventDefault();
        hideError('pane-login');
        const email = document.getElementById('l-utente').value.trim();
        const password = document.getElementById('l-pass').value;
        if(!email || !password){ showError('pane-login', 'Inserisci email e password.'); return; }
        try{
            const res = await fetch('/api/auth/login', {
                method: 'POST',
                headers: {'Content-Type':'application/json'},
                body: JSON.stringify({email, password})
            });
            const data = await res.json();
            if(!res.ok){ showError('pane-login', data.errore || 'Credenziali non valide.'); return; }
            localStorage.setItem('utente', JSON.stringify(data));
            closeModal();
            redirectByRole(data.ruolo);
        }catch(err){
            showError('pane-login', 'Errore di connessione.');
        }
    });
})();

/* ---- REGISTRAZIONE ---- */
(function(){
    const form = document.getElementById('pane-signup').querySelector('form');
    form.addEventListener('submit', async (e)=>{
        e.preventDefault();
        hideError('pane-signup');
        const nome      = document.getElementById('s-name').value.trim();
        const cognome   = document.getElementById('s-cognome').value.trim();
        const email     = document.getElementById('s-email').value.trim();
        const sesso     = document.querySelector('input[name="sesso"]:checked')?.value || '';
        const data_nascita = document.getElementById('s-dataNascita').value;
        const ruolo     = document.getElementById('s-ruolo').value;
        const password  = document.getElementById('s-pass').value;

        if(!nome || !cognome || !email || !sesso || !data_nascita || !ruolo || !password){
            showError('pane-signup', 'Compila tutti i campi obbligatori.');
            return;
        }
        try{
            const res = await fetch('/api/auth/registra', {
                method: 'POST',
                headers: {'Content-Type':'application/json'},
                body: JSON.stringify({nome, cognome, email, sesso, data_nascita, ruolo, password})
            });
            const data = await res.json();
            if(!res.ok){ showError('pane-signup', data.errore || 'Registrazione fallita.'); return; }
            localStorage.setItem('utente', JSON.stringify(data));
            closeModal();
            redirectByRole(data.ruolo);
        }catch(err){
            showError('pane-signup', 'Errore di connessione.');
        }
    });
})();

/* ---- Se l'utente è già loggato, redirect diretto ---- */
(function(){
    const raw = localStorage.getItem('utente');
    if(raw){
        try{
            const u = JSON.parse(raw);
            if(u && u.id && u.ruolo) redirectByRole(u.ruolo);
        }catch(_){}
    }
})();
