//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_CIBO_H
#define PALESTRADIGITALE_CIBO_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Cibo {
protected:
    //Parametri
    int id_cibo; //PK
    string nome;
    float kcal;
    float carboidrati;
    float proteine;
    float grassi;

public:
    //Costruttore
    Cibo(int id_cibo, string nome, float kcal, float carboidrati, float proteine, float grassi);

    int getId() const;
    string getNome() const;
    float getKcal() const;
    float getCarboidrati() const;
    float getProteine() const;
    float getGrassi() const;
};


#endif //PALESTRADIGITALE_CIBO_H
