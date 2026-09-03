//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_UTENTE_PROGRAMMA_H
#define PALESTRADIGITALE_UTENTE_PROGRAMMA_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Utente_programma {
protected:
    //Parametri
    int id_assegnazione; //PK
    int id_utente; //FK
    int id_programma; //FK
    date data_inizio{};
    string stato{}; //ENUM

public:
    //Costruttore
    Utente_programma(int id_assegnazione, int id_utente, int id_programma, date data_inizio, string stato);
};


#endif //PALESTRADIGITALE_UTENTE_PROGRAMMA_H
