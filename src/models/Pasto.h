//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_PASTO_H
#define PALESTRADIGITALE_PASTO_H

#include <string>
using string = std::string;

class Pasto {
protected:
    //Parametri
    int id_pasto; //PK
    int id_piano; //FK
    int giorno;
    string tipo_pasto;

public:
    //Costruttore
    Pasto(int id_pasto, int id_piano, int giorno, string tipo_pasto);

    //Metodi
    int getId() const;
    int getIdPiano() const;
    int getGiorno() const;
    string getTipoPasto() const;
};


#endif //PALESTRADIGITALE_PASTO_H
