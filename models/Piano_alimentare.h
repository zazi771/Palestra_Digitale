//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_PIANO_ALIMENTARE_H
#define PALESTRADIGITALE_PIANO_ALIMENTARE_H

#include <string>
using string = std::string;

class Piano_alimentare {
protected:
    //Parametri
    int id_piano; //PK
    int id_nutrizionista; //FK
    string nome;
    string descrizione;
    int id_cliente; //FK

public:
    //Costruttore
    Piano_alimentare(int id_piano, int id_nutrizionista, string nome, string descrizione, int id_cliente);
};


#endif //PALESTRADIGITALE_PIANO_ALIMENTARE_H
