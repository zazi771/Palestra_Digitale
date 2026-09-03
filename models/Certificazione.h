//
// Created by giorg on 22/08/2026.
//

#ifndef PALESTRADIGITALE_CERTIFICAZIONE_H
#define PALESTRADIGITALE_CERTIFICAZIONE_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Certificazione {
protected:
    //Parametri
    int id_certificazione; //PK
    int id_esperto; //FK
    string cv;
    string certificazione;
    string professione;
    string ente_rilascio;
    int codice_certificazione;
    date data_rilascio;
    date data_scadenza;

public:
    //Costruttore
    Certificazione(int id_certificazione, int id_esperto, string cv, string certificazione, string professione,
        string ente_rilascio, int codice_certificazione, date data_rilascio, date data_scadenza);
};


#endif //PALESTRADIGITALE_CERTIFICAZIONE_H
