//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_PASTO_CIBO_H
#define PALESTRADIGITALE_PASTO_CIBO_H


class Pasto_cibo {
protected:
    //Parametri
    int id_pasto_cibo; //PK
    int id_pasto; //FK
    int id_cibo; //FK
    int quantita_gr;

public:
    //Costruttore
    Pasto_cibo(int id_pasto_cibo, int id_pasto, int id_cibo, int quantita_gr);

    int getId() const;
    int getIdPasto() const;
    int getIdCibo() const;
    int getQuantita() const;
};


#endif //PALESTRADIGITALE_PASTO_CIBO_H
