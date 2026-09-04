//
// Created by giorg on 01/09/2026.
//

#include "Database.h"
#include <sstream>

Database::Database(const std::string& percorsoFile)
    : db(percorsoFile, SQLite::OPEN_READWRITE) {
    db.exec("PRAGMA foreign_keys = ON;");
}

// =====================================================================
// UTENTE
// =====================================================================

namespace {
// Converte date (anno-mese-giorno) in stringa ISO "YYYY-MM-DD"
date parseDate(const std::string& s) {
    std::istringstream iss(s);
    int y, m, d;
    char sep;
    if (iss >> y >> sep >> m >> sep >> d) {
        return date{std::chrono::year{y}, std::chrono::month{static_cast<unsigned>(m)}, std::chrono::day{static_cast<unsigned>(d)}};
    }
    return date{};
}
}

std::vector<Utente> Database::getTuttiUtenti() {
    std::vector<Utente> risultato;
    SQLite::Statement query(db, "SELECT id_utente, email, password, nome, cognome, ruolo, data_registrazione, sesso, data_nascita FROM Utente;");
    while (query.executeStep()) {
        risultato.push_back(Utente(
            (int)query.getColumn(0), std::string(query.getColumn(1)), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4)), std::string(query.getColumn(5)),
            parseDate(std::string(query.getColumn(6))), std::string(query.getColumn(7)), parseDate(std::string(query.getColumn(8)))));
    }
    return risultato;
}

Utente Database::getUtenteById(int id) {
    SQLite::Statement query(db, "SELECT id_utente, email, password, nome, cognome, ruolo, data_registrazione, sesso, data_nascita FROM Utente WHERE id_utente = ?;");
    query.bind(1, id);
    if (query.executeStep()) {
        return Utente(
            (int)query.getColumn(0), std::string(query.getColumn(1)), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4)), std::string(query.getColumn(5)),
            parseDate(std::string(query.getColumn(6))), std::string(query.getColumn(7)), parseDate(std::string(query.getColumn(8))));
    }
    throw std::runtime_error("Utente non trovato");
}

std::optional<Utente> Database::getUtenteByEmail(const std::string& email) {
    SQLite::Statement query(db, "SELECT id_utente, email, password, nome, cognome, ruolo, data_registrazione, sesso, data_nascita FROM Utente WHERE email = ?;");
    query.bind(1, email);
    if (query.executeStep()) {
        return Utente(
            (int)query.getColumn(0), std::string(query.getColumn(1)), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4)), std::string(query.getColumn(5)),
            parseDate(std::string(query.getColumn(6))), std::string(query.getColumn(7)), parseDate(std::string(query.getColumn(8))));
    }
    return std::nullopt;
}

std::vector<Utente> Database::getUtentiByRuolo(const std::string& ruolo) {
    std::vector<Utente> risultato;
    SQLite::Statement query(db, "SELECT id_utente, email, password, nome, cognome, ruolo, data_registrazione, sesso, data_nascita FROM Utente WHERE ruolo = ?;");
    query.bind(1, ruolo);
    while (query.executeStep()) {
        risultato.push_back(Utente(
            (int)query.getColumn(0), std::string(query.getColumn(1)), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4)), std::string(query.getColumn(5)),
            parseDate(std::string(query.getColumn(6))), std::string(query.getColumn(7)), parseDate(std::string(query.getColumn(8)))));
    }
    return risultato;
}

bool Database::inserisciUtente(const Utente& u) {
    SQLite::Statement query(db,
        "INSERT INTO Utente (email, password, nome, cognome, ruolo, data_registrazione, sesso, data_nascita) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
    query.bind(1, u.getEmail());
    query.bind(2, u.getPasswordHash());
    query.bind(3, u.getNome());
    query.bind(4, u.getCognome());
    query.bind(5, u.getRuolo());
    query.bind(6, u.getDataRegistrazioneStr());
    query.bind(7, u.getSesso());
    query.bind(8, u.getDataNascitaStr());
    return query.exec() > 0;
}

bool Database::aggiornaUtente(const Utente& u) {
    SQLite::Statement query(db,
        "UPDATE Utente SET email = ?, nome = ?, cognome = ?, ruolo = ? WHERE id_utente = ?;");
    query.bind(1, u.getEmail());
    query.bind(2, u.getNome());
    query.bind(3, u.getCognome());
    query.bind(4, u.getRuolo());
    query.bind(5, u.getId());
    return query.exec() > 0;
}

bool Database::eliminaUtente(int id) {
    SQLite::Statement query(db, "DELETE FROM Utente WHERE id_utente = ?;");
    query.bind(1, id);
    return query.exec() > 0;
}

// =====================================================================
// CARTELLA CLINICA
// =====================================================================
std::vector<Cartella_clinica> Database::getCartelleByCliente(int id_cliente) {
    std::vector<Cartella_clinica> risultato;
    SQLite::Statement query(db,
        "SELECT id_cartella, id_cliente, data_rilevazione, altezza_cm, peso_kg, circonferenza_vita_cm, "
        "circonferenza_fianchi_cm, massa_grassa_percentuale, massa_magra_kg, patologie, allergie, "
        "intolleranze_alimentari, infortuni_pregressi, farmaci_assunti, livello_attivita_fisica, obiettivo, "
        "note_mediche FROM Cartella_clinica WHERE id_cliente = ? ORDER BY data_rilevazione DESC;");
    query.bind(1, id_cliente);
    while (query.executeStep()) {
        risultato.push_back(Cartella_clinica(
            (int)query.getColumn(0), (int)query.getColumn(1), parseDate(std::string(query.getColumn(2))),
            (float)query.getColumn(3).getDouble(), (float)query.getColumn(4).getDouble(), (float)query.getColumn(5).getDouble(), (float)query.getColumn(6).getDouble(),
            (float)query.getColumn(7).getDouble(), (float)query.getColumn(8).getDouble(), std::string(query.getColumn(9)),
            std::string(query.getColumn(10)), std::string(query.getColumn(11)), std::string(query.getColumn(12)),
            std::string(query.getColumn(13)), std::string(query.getColumn(14)), std::string(query.getColumn(15)),
            std::string(query.getColumn(16))));
    }
    return risultato;
}

bool Database::inserisciCartella(const Cartella_clinica& c) {
    SQLite::Statement query(db,
        "INSERT INTO Cartella_clinica (id_cliente, data_rilevazione, altezza_cm, peso_kg, circonferenza_vita_cm, "
        "circonferenza_fianchi_cm, massa_grassa_percentuale, massa_magra_kg, patologie, allergie, "
        "intolleranze_alimentari, infortuni_pregressi, farmaci_assunti, livello_attivita_fisica, obiettivo, "
        "note_mediche) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    query.bind(1, c.getIdCliente());
    query.bind(2, c.getDataRilevazioneStr());
    query.bind(3, c.getAltezza());
    query.bind(4, c.getPeso());
    query.bind(5, c.getCircVita());
    query.bind(6, c.getCircFianchi());
    query.bind(7, c.getMassaGrassa());
    query.bind(8, c.getMassaMagra());
    query.bind(9, c.getPatologie());
    query.bind(10, c.getAllergie());
    query.bind(11, c.getIntolleranze());
    query.bind(12, c.getInfortuni());
    query.bind(13, c.getFarmaci());
    query.bind(14, c.getLivelloAttivita());
    query.bind(15, c.getObiettivo());
    query.bind(16, c.getNoteMediche());
    return query.exec() > 0;
}

// =====================================================================
// CERTIFICAZIONE
// =====================================================================
std::optional<Certificazione> Database::getCertificazioneByEsperto(int id_esperto) {
    SQLite::Statement query(db,
        "SELECT id_certificazione, id_esperto, cv, certificazione, professione, ente_rilascio, "
        "codice_certificazione, data_rilascio, data_scadenza FROM Certificazione WHERE id_esperto = ?;");
    query.bind(1, id_esperto);
    if (query.executeStep()) {
        return Certificazione((int)query.getColumn(0), (int)query.getColumn(1), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4)), std::string(query.getColumn(5)),
            (int)query.getColumn(6), parseDate(std::string(query.getColumn(7))),
            parseDate(std::string(query.getColumn(8))));
    }
    return std::nullopt;
}

bool Database::inserisciCertificazione(const Certificazione& c) {
    SQLite::Statement query(db,
        "INSERT INTO Certificazione (id_esperto, cv, certificazione, professione, ente_rilascio, "
        "codice_certificazione, data_rilascio, data_scadenza) VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
    query.bind(1, c.getIdEsperto());
    query.bind(2, c.getCv());
    query.bind(3, c.getCertificazione());
    query.bind(4, c.getProfessione());
    query.bind(5, c.getEnteRilascio());
    query.bind(6, c.getCodice());
    query.bind(7, c.getDataRilascioStr());
    query.bind(8, c.getDataScadenzaStr());
    return query.exec() > 0;
}

// =====================================================================
// CIBO
// =====================================================================
std::vector<Cibo> Database::getTuttiCibi() {
    std::vector<Cibo> risultato;
    SQLite::Statement query(db, "SELECT id_cibo, nome, kcal, carboidrati, proteine, grassi FROM Cibo;");
    while (query.executeStep()) {
        risultato.push_back(Cibo(
            (int)query.getColumn(0), std::string(query.getColumn(1)), (float)query.getColumn(2).getDouble(),
            (float)query.getColumn(3).getDouble(), (float)query.getColumn(4).getDouble(), (float)query.getColumn(5).getDouble()));
    }
    return risultato;
}

std::vector<Cibo> Database::cercaCibi(const std::string& queryString) {
    std::vector<Cibo> risultato;
    SQLite::Statement query(db,
        "SELECT id_cibo, nome, kcal, carboidrati, proteine, grassi FROM Cibo WHERE nome LIKE ?;");
    query.bind(1, "%" + queryString + "%");
    while (query.executeStep()) {
        risultato.push_back(Cibo(
            (int)query.getColumn(0), std::string(query.getColumn(1)), (float)query.getColumn(2).getDouble(),
            (float)query.getColumn(3).getDouble(), (float)query.getColumn(4).getDouble(), (float)query.getColumn(5).getDouble()));
    }
    return risultato;
}

int Database::inserisciCibo(const Cibo& c) {
    SQLite::Statement query(db,
        "INSERT INTO Cibo (nome, kcal, carboidrati, proteine, grassi) VALUES (?, ?, ?, ?, ?);");
    query.bind(1, c.getNome());
    query.bind(2, c.getKcal());
    query.bind(3, c.getCarboidrati());
    query.bind(4, c.getProteine());
    query.bind(5, c.getGrassi());
    query.exec();
    return (int)db.getLastInsertRowid();
}

// =====================================================================
// ESERCIZIO
// =====================================================================
std::vector<Esercizio> Database::getTuttiEsercizi() {
    std::vector<Esercizio> risultato;
    SQLite::Statement query(db,
        "SELECT id_esercizio, nome, descrizione, gruppo_muscolare, url_video FROM Esercizio;");
    while (query.executeStep()) {
        risultato.push_back(Esercizio(
            (int)query.getColumn(0), std::string(query.getColumn(1)), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4))));
    }
    return risultato;
}

int Database::inserisciEsercizio(const Esercizio& e) {
    SQLite::Statement query(db,
        "INSERT INTO Esercizio (nome, descrizione, gruppo_muscolare, url_video) VALUES (?, ?, ?, ?);");
    query.bind(1, e.getNome());
    query.bind(2, e.getDescrizione());
    query.bind(3, e.getGruppoMuscolare());
    query.bind(4, e.getUrlVideo());
    query.exec();
    return (int)db.getLastInsertRowid();
}

// =====================================================================
// PIANO ALIMENTARE (+ pasti + alimenti)
// =====================================================================
std::vector<Piano_alimentare> Database::getPianiByCliente(int id_cliente) {
    std::vector<Piano_alimentare> risultato;
    SQLite::Statement query(db,
        "SELECT id_piano, id_nutrizionista, nome, descrizione, id_cliente FROM Piano_alimentare WHERE id_cliente = ?;");
    query.bind(1, id_cliente);
    while (query.executeStep()) {
        risultato.push_back(Piano_alimentare(
            (int)query.getColumn(0), (int)query.getColumn(1), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), (int)query.getColumn(4)));
    }
    return risultato;
}

int Database::inserisciPianoAlimentare(const Piano_alimentare& p) {
    SQLite::Statement query(db,
        "INSERT INTO Piano_alimentare (id_nutrizionista, nome, descrizione, id_cliente) VALUES (?, ?, ?, ?);");
    query.bind(1, p.getIdNutrizionista());
    query.bind(2, p.getNome());
    query.bind(3, p.getDescrizione());
    query.bind(4, p.getIdCliente());
    query.exec();
    return (int)db.getLastInsertRowid();
}

std::vector<Pasto> Database::getPastiByPiano(int id_piano) {
    std::vector<Pasto> risultato;
    SQLite::Statement query(db,
        "SELECT id_pasto, id_piano, id_cibo, giorno, tipo_pasto FROM Pasto WHERE id_piano = ?;");
    query.bind(1, id_piano);
    while (query.executeStep()) {
        risultato.push_back(Pasto(
            (int)query.getColumn(0), (int)query.getColumn(1), (int)query.getColumn(2), (int)query.getColumn(3),
            std::string(query.getColumn(4))));
    }
    return risultato;
}

int Database::inserisciPasto(const Pasto& p) {
    SQLite::Statement query(db,
        "INSERT INTO Pasto (id_piano, id_cibo, giorno, tipo_pasto) VALUES (?, ?, ?, ?);");
    query.bind(1, p.getIdPiano());
    query.bind(2, p.getIdCibo());
    query.bind(3, p.getGiorno());
    query.bind(4, p.getTipoPasto());
    query.exec();
    return (int)db.getLastInsertRowid();
}

std::vector<Pasto_cibo> Database::getAlimentiByPasto(int id_pasto) {
    std::vector<Pasto_cibo> risultato;
    SQLite::Statement query(db,
        "SELECT id_pasto_cibo, id_pasto, id_cibo, quantita_gr FROM Pasto_cibo WHERE id_pasto = ?;");
    query.bind(1, id_pasto);
    while (query.executeStep()) {
        risultato.push_back(Pasto_cibo(
            (int)query.getColumn(0), (int)query.getColumn(1), (int)query.getColumn(2), (int)query.getColumn(3)));
    }
    return risultato;
}

bool Database::inserisciPastoCibo(const Pasto_cibo& pc) {
    SQLite::Statement query(db,
        "INSERT INTO Pasto_cibo (id_pasto, id_cibo, quantita_gr) VALUES (?, ?, ?);");
    query.bind(1, pc.getIdPasto());
    query.bind(2, pc.getIdCibo());
    query.bind(3, pc.getQuantita());
    return query.exec() > 0;
}

// =====================================================================
// PROGRAMMA ALLENAMENTO (+ esercizi)
// =====================================================================
std::vector<Programma_allenamento> Database::getProgrammiByCliente(int id_cliente) {
    std::vector<Programma_allenamento> risultato;
    SQLite::Statement query(db,
        "SELECT p.id_programma, p.id_trainer, p.nome, p.obiettivo, p.livello_difficolta, p.durata_settimane, "
        "p.descrizione FROM Programma_allenamento p "
        "JOIN Utente_Programma up ON p.id_programma = up.id_programma WHERE up.id_utente = ?;");
    query.bind(1, id_cliente);
    while (query.executeStep()) {
        risultato.push_back(Programma_allenamento(
            (int)query.getColumn(0), (int)query.getColumn(1), std::string(query.getColumn(2)),
            std::string(query.getColumn(3)), std::string(query.getColumn(4)), (int)query.getColumn(5),
            std::string(query.getColumn(6))));
    }
    return risultato;
}

int Database::inserisciProgramma(const Programma_allenamento& p) {
    SQLite::Statement query(db,
        "INSERT INTO Programma_allenamento (id_trainer, nome, obiettivo, livello_difficolta, durata_settimane, "
        "descrizione) VALUES (?, ?, ?, ?, ?, ?);");
    query.bind(1, p.getIdTrainer());
    query.bind(2, p.getNome());
    query.bind(3, p.getObiettivo());
    query.bind(4, p.getLivelloDifficolta());
    query.bind(5, p.getDurataSettimane());
    query.bind(6, p.getDescrizione());
    query.exec();
    return (int)db.getLastInsertRowid();
}

std::vector<Programma_esercizio> Database::getEserciziByProgramma(int id_programma) {
    std::vector<Programma_esercizio> risultato;
    SQLite::Statement query(db,
        "SELECT id_programma_esercizio, id_programma, id_esercizio, ordine, serie, ripetizioni, recupero_seci "
        "FROM Programma_esercizio WHERE id_programma = ? ORDER BY ordine;");
    query.bind(1, id_programma);
    while (query.executeStep()) {
        risultato.push_back(Programma_esercizio(
            (int)query.getColumn(0), (int)query.getColumn(1), (int)query.getColumn(2), (int)query.getColumn(3),
            (int)query.getColumn(4), std::string(query.getColumn(5)), (int)query.getColumn(6)));
    }
    return risultato;
}

bool Database::inserisciProgrammaEsercizio(const Programma_esercizio& pe) {
    SQLite::Statement query(db,
        "INSERT INTO Programma_esercizio (id_programma, id_esercizio, ordine, serie, ripetizioni, recupero_seci) "
        "VALUES (?, ?, ?, ?, ?, ?);");
    query.bind(1, pe.getIdProgramma());
    query.bind(2, pe.getIdEsercizio());
    query.bind(3, pe.getOrdine());
    query.bind(4, pe.getSerie());
    query.bind(5, pe.getRipetizioni());
    query.bind(6, pe.getRecupero());
    return query.exec() > 0;
}

// =====================================================================
// ASSEGNAZIONE PROGRAMMA <-> UTENTE
// =====================================================================
bool Database::assegnaProgramma(int id_utente, int id_programma, const std::string& data_inizio) {
    SQLite::Statement query(db,
        "INSERT INTO Utente_Programma (id_utente, id_programma, data_inizio, stato) VALUES (?, ?, ?, ?);");
    query.bind(1, id_utente);
    query.bind(2, id_programma);
    query.bind(3, data_inizio);
    query.bind(4, "attivo");
    return query.exec() > 0;
}

// =====================================================================
// SESSIONE
// =====================================================================
std::vector<Sessione> Database::getSessioniByCliente(int id_cliente) {
    std::vector<Sessione> risultato;
    SQLite::Statement query(db,
        "SELECT id_sessione, id_utente, id_programma, data, tempo_minuti, completato FROM Sessione "
        "WHERE id_utente = ? ORDER BY data DESC;");
    query.bind(1, id_cliente);
    while (query.executeStep()) {
        risultato.push_back(Sessione(
            (int)query.getColumn(0), (int)query.getColumn(1), (int)query.getColumn(2),
            parseDate(std::string(query.getColumn(3))), (int)query.getColumn(4), (int)query.getColumn(5)));
    }
    return risultato;
}

int Database::inserisciSessione(const Sessione& s) {
    SQLite::Statement query(db,
        "INSERT INTO Sessione (id_utente, id_programma, data, tempo_minuti, completato) VALUES (?, ?, ?, ?, ?);");
    query.bind(1, s.getIdUtente());
    query.bind(2, s.getIdProgramma());
    query.bind(3, s.getDataStr());
    query.bind(4, s.getTempoMinuti());
    query.bind(5, s.getCompletato());
    query.exec();
    return (int)db.getLastInsertRowid();
}
