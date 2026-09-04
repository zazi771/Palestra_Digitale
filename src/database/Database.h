//
// Created by giorg on 01/09/2026.
//

#ifndef PALESTRADIGITALE_DATABASE_H
#define PALESTRADIGITALE_DATABASE_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <vector>
#include <optional>
#include "models/Utente.h"
#include "models/Cartella_clinica.h"
#include "models/Certificazione.h"
#include "models/Cibo.h"
#include "models/Esercizio.h"
#include "models/Feedback.h"
#include "models/Pasto.h"
#include "models/Pasto_cibo.h"
#include "models/Piano_alimentare.h"
#include "models/Programma_allenamento.h"
#include "models/Programma_esercizio.h"
#include "models/Sessione.h"
#include "models/Utente_programma.h"

class Database {
private:
    SQLite::Database db;

public:
    Database(const std::string& percorsoFile);

    //UTENTE
    std::vector<Utente> getTuttiUtenti();
    Utente getUtenteById(int id);
    std::optional<Utente> getUtenteByEmail(const std::string& email);
    std::vector<Utente> getUtentiByRuolo(const std::string& ruolo);
    bool inserisciUtente(const Utente& u);
    bool aggiornaUtente(const Utente& u);
    bool eliminaUtente(int id);
    //

    //CARTELLA CLINICA
    std::vector<Cartella_clinica> getCartelleByCliente(int id_cliente);
    bool inserisciCartella(const Cartella_clinica& c);
    //

    //CERTIFICAZIONE
    std::optional<Certificazione> getCertificazioneByEsperto(int id_esperto);
    bool inserisciCertificazione(const Certificazione& c);
    //

    //CIBO
    std::vector<Cibo> getTuttiCibi();
    std::vector<Cibo> cercaCibi(const std::string& query);
    int inserisciCibo(const Cibo& c);  // ritorna id_cibo
    //

    //ESERCIZIO
    std::vector<Esercizio> getTuttiEsercizi();
    int inserisciEsercizio(const Esercizio& e);  // ritorna id_esercizio
    //

    //PIANO ALIMENTARE
    std::vector<Piano_alimentare> getPianiByCliente(int id_cliente);
    int inserisciPianoAlimentare(const Piano_alimentare& p);        // ritorna id_piano
    std::vector<Pasto> getPastiByPiano(int id_piano);
    int inserisciPasto(const Pasto& p);                             // ritorna id_pasto
    std::vector<Pasto_cibo> getAlimentiByPasto(int id_pasto);
    bool inserisciPastoCibo(const Pasto_cibo& pc);
    //

    //PROGRAMMA ALLENAMENTO
    std::vector<Programma_allenamento> getProgrammiByCliente(int id_cliente);
    int inserisciProgramma(const Programma_allenamento& p);         // ritorna id_programma
    std::vector<Programma_esercizio> getEserciziByProgramma(int id_programma);
    bool inserisciProgrammaEsercizio(const Programma_esercizio& pe);
    //

    //ASSEGNAZIONE PROGRAMMA <-> UTENTE
    bool assegnaProgramma(int id_utente, int id_programma, const std::string& data_inizio);
    //
    //Sessione
    std::vector<Sessione> getSessioniByCliente(int id_cliente);
    int inserisciSessione(const Sessione& s);
    //

};

#endif //PALESTRADIGITALE_DATABASE_H
