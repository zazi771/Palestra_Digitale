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
    function handleSubmit(e, msg){
    e.preventDefault();
    alert(msg + ' — questa è una demo statica, nessun dato viene inviato.');
    closeModal();
    return false;
}
    document.addEventListener('keydown', (e)=>{
    if(e.key === 'Escape') closeModal();
});