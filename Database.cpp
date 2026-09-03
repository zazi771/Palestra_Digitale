//
// Created by giorg on 01/09/2026.
//

#include "Database.h"

#include "Database.h"

Database::Database(const std::string& percorsoFile)
    : db(percorsoFile, SQLite::OPEN_READWRITE) {
    db.exec("PRAGMA foreign_keys = ON;");
}

std::vector<Utente> Database::getTuttiUtenti() {
    std::vector<Utente> risultato;

    SQLite::Statement query(db,
        "SELECT id_utente, email, password_hash, nome, cognome, ruolo FROM UTENTI;");

    while (query.executeStep()) {
        int id = query.getColumn(0);
        std::string email = query.getColumn(1);
        std::string pwHash = query.getColumn(2);
        std::string nome = query.getColumn(3);
        std::string cognome = query.getColumn(4);
        std::string ruolo = query.getColumn(5);

        risultato.push_back(Utente(id, email, pwHash, nome, cognome, ruolo));
    }

    return risultato;
}

Utente Database::getUtenteById(int id) {
    SQLite::Statement query(db,
        "SELECT id_utente, email, password_hash, nome, cognome, ruolo FROM UTENTI WHERE id_utente = ?;");
    query.bind(1, id);

    if (query.executeStep()) {
        std::string email = query.getColumn(1);
        std::string pwHash = query.getColumn(2);
        std::string nome = query.getColumn(3);
        std::string cognome = query.getColumn(4);
        std::string ruolo = query.getColumn(5);

        return Utente(id, email, pwHash, nome, cognome, ruolo);
    }

    throw std::runtime_error("Utente non trovato");
}

bool Database::inserisciUtente(const Utente& u) {
    SQLite::Statement query(db,
        "INSERT INTO UTENTI (email, password_hash, nome, cognome, ruolo, data_registrazione) "
        "VALUES (?, ?, ?, ?, ?, ?);");

    query.bind(1, u.getEmail());
    query.bind(2, u.getPasswordHash());
    query.bind(3, u.getNome());
    query.bind(4, u.getCognome());
    query.bind(5, u.getRuolo());
    query.bind(6, dataToString(u.getDataRegistrazione()));

    return query.exec() > 0;
}

bool Database::aggiornaUtente(const Utente& u) {
    SQLite::Statement query(db,
        "UPDATE UTENTI SET email = ?, nome = ?, cognome = ?, ruolo = ? WHERE id_utente = ?;");

    query.bind(1, u.getEmail());
    query.bind(2, u.getNome());
    query.bind(3, u.getCognome());
    query.bind(4, u.getRuolo());
    query.bind(5, u.getId());

    return query.exec() > 0;
}

bool Database::eliminaUtente(int id) {
    SQLite::Statement query(db, "DELETE FROM UTENTI WHERE id_utente = ?;");
    query.bind(1, id);

    return query.exec() > 0;
}