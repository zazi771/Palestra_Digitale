//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_ESERCIZIO_H
#define PALESTRADIGITALE_ESERCIZIO_H

#include <string>
using string = std::string;

class Esercizio {
protected:
    //Parametri
    int id_esercizio; //PK
    string nome;
    string descrizione;
    string gruppo_muscolare;
    string url_video;

public:
    //Costruttore
    Esercizio(int id_esercizio, string nome, string descrizione, string gruppo_muscolare, string url_video);

    int getId() const;
    string getNome() const;
    string getDescrizione() const;
    string getGruppoMuscolare() const;
    string getUrlVideo() const;
};


#endif //PALESTRADIGITALE_ESERCIZIO_H
