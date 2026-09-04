//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_PROGRAMMA_ALLENAMENTO_H
#define PALESTRADIGITALE_PROGRAMMA_ALLENAMENTO_H

#include <string>
using string = std::string;

class Programma_allenamento {
protected:
    //Parametri
    int id_programma; //PK
    int id_trainer; //FK
    string nome;
    string obiettivo;
    string livello_difficolta; //ENUM
    int durata_settimane;
    string descrizione;

public:
    //Costruttore
    Programma_allenamento(int id_programma, int id_trainer, string nome, string obiettivo, string livello_difficolta,
    int durata_settimane, string descrizione);

    int getId() const;
    int getIdTrainer() const;
    string getNome() const;
    string getObiettivo() const;
    string getLivelloDifficolta() const;
    int getDurataSettimane() const;
    string getDescrizione() const;
};


#endif //PALESTRADIGITALE_PROGRAMMA_ALLENAMENTO_H
