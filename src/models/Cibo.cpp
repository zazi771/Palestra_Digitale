//
// Created by giorg on 23/08/2026.
//

#include "Cibo.h"
#include <utility>

Cibo::Cibo(int id_cibo, string nome, float kcal, float carboidrati, float proteine, float grassi)
    :id_cibo(id_cibo), nome(std::move(nome)), kcal(kcal), carboidrati(carboidrati), proteine(proteine), grassi(grassi){}

int Cibo::getId() const { return id_cibo; }
std::string Cibo::getNome() const { return nome; }
float Cibo::getKcal() const { return kcal; }
float Cibo::getCarboidrati() const { return carboidrati; }
float Cibo::getProteine() const { return proteine; }
float Cibo::getGrassi() const { return grassi; }