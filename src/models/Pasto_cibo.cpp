//
// Created by giorg on 23/08/2026.
//

#include "Pasto_cibo.h"

Pasto_cibo::Pasto_cibo(int id_pasto_cibo, int id_pasto, int id_cibo, int quantita_gr)
    :id_pasto_cibo(id_pasto_cibo), id_pasto(id_pasto), id_cibo(id_cibo), quantita_gr(quantita_gr){}

int Pasto_cibo::getId() const { return id_pasto_cibo; }
int Pasto_cibo::getIdPasto() const { return id_pasto; }
int Pasto_cibo::getIdCibo() const { return id_cibo; }
int Pasto_cibo::getQuantita() const { return quantita_gr; }