//
// Created by giorg on 01/09/2026.
//

#ifndef PALESTRADIGITALE_DATABASE_H
#define PALESTRADIGITALE_DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include "models/Utente.h"

class Database {
private:
    SQLite::Database db;

public:
    Database(const std::string& percorsoFile);

    //UTENTE
    std::vector<Utente> getTuttiUtenti();
    Utente getUtenteById(int id);
    bool inserisciUtente(const Utente& u);
    bool aggiornaUtente(const Utente& u);
    bool eliminaUtente(int id);
    //

    //
};

#endif //PALESTRADIGITALE_DATABASE_H
