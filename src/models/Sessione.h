//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_SESSIONE_H
#define PALESTRADIGITALE_SESSIONE_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Sessione {
protected:
    //Parametri
    int id_sessione; //PK
    int id_utente; //FK
    int id_programma; //FK
    date data;
    int tempo_minuti;
    int completato; //INT o BOOL?

public:
    //Costruttore
    Sessione(int id_sessione, int id_utente, int id_programma, date data, int tempo_minuti, int completato);

    int getId() const;
    int getIdUtente() const;
    int getIdProgramma() const;
    date getData() const;
    string getDataStr() const;
    int getTempoMinuti() const;
    int getCompletato() const;
};


#endif //PALESTRADIGITALE_SESSIONE_H
