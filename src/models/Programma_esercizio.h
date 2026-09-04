//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_PROGRAMMA_ESERCIZIO_H
#define PALESTRADIGITALE_PROGRAMMA_ESERCIZIO_H

#include <string>
using string = std::string;

class Programma_esercizio {
protected:
    //Parametri
    int id_programma_esercizio; //PK
    int id_programma; //FK
    int id_esercizio; //FK
    int ordine;
    int serie;
    string ripetizioni; //TEXT?
    int recupero_sec;

public:
    //Costruttore
    Programma_esercizio(int id_programma_esercizio, int id_programma, int id_esercizio, int ordine, int serie,
        string ripetizioni, int recupero_sec);

    int getId() const;
    int getIdProgramma() const;
    int getIdEsercizio() const;
    int getOrdine() const;
    int getSerie() const;
    string getRipetizioni() const;
    int getRecupero() const;
};


#endif //PALESTRADIGITALE_PROGRAMMA_ESERCIZIO_H
