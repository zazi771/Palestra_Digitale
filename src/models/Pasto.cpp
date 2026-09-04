//
// Created by giorg on 23/08/2026.
//

#include "Pasto.h"

#include <utility>

Pasto::Pasto(int id_pasto, int id_piano, int id_cibo, int giorno, string tipo_pasto)
    :id_pasto(id_pasto), id_piano(id_piano), id_cibo(id_cibo), giorno(giorno), tipo_pasto(std::move(tipo_pasto)){}

int Pasto::getId() const { return id_pasto; }
int Pasto::getIdPiano() const { return id_piano; }
int Pasto::getIdCibo() const { return id_cibo; }
int Pasto::getGiorno() const { return giorno; }
std::string Pasto::getTipoPasto() const { return tipo_pasto; }

