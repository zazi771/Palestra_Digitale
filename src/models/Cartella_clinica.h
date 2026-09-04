//
// Created by giorg on 21/08/2026.
//

#ifndef PALESTRADIGITALE_CARTELLA_CLINICA_H
#define PALESTRADIGITALE_CARTELLA_CLINICA_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Cartella_clinica {
protected:
    //Parametri
    int id_cartella; //PK
    int id_cliente; //FK
    date data_rilevazione;
    float altezza_cm;
    float peso_kg;
    float circonferenza_vita_cm;
    float circonferenza_fianchi_cm;
    float massa_grassa_percentuale;
    float massa_magra_kg;
    string patologie;
    string allergie;
    string intolleranze_alimentari;
    string infortuni_pregressi;
    string farmaci_assunti;
    string livello_attivita_fisica; //ENUM
    string obiettivo;
    string note_mediche;

public:
    //Costruttore
    Cartella_clinica(int id_cartella, int id_cliente, date data_rilevazione, float altezza_cm, float peso_kg,
        float circonferenza_vita_cm, float circonferenza_fianchi_cm, float massa_grassa_percentuale,
        float massa_magra_kg, string patologie, string allergie, string intolleranze_alimentari,
        string infortuni_pregressi, string farmaci_assunti, string livello_attivita_fisica, string obiettivo,
        string note_mediche);

    //Metodi
    int getId() const;
    int getIdCliente() const;
    date getDataRilevazione() const;
    string getDataRilevazioneStr() const;
    float getAltezza() const;
    float getPeso() const;
    float getCircVita() const;
    float getCircFianchi() const;
    float getMassaGrassa() const;
    float getMassaMagra() const;
    string getPatologie() const;
    string getAllergie() const;
    string getIntolleranze() const;
    string getInfortuni() const;
    string getFarmaci() const;
    string getLivelloAttivita() const;
    string getObiettivo() const;
    string getNoteMediche() const;

};


#endif //PALESTRADIGITALE_CARTELLA_CLINICA_H
