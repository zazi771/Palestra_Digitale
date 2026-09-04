//
// Created by giorg on 23/08/2026.
//

#include "Utente_programma.h"
#include <utility>

Utente_programma::Utente_programma(int id_assegnazione, int id_utente, int id_programma, date data_inizio, string stato)
    :id_assegnazione(id_assegnazione), id_utente(id_utente), id_programma(id_programma), data_inizio(data_inizio),
    stato(std::move(stato)){}

int Utente_programma::getId() const { return id_assegnazione; }
int Utente_programma::getIdUtente() const { return id_utente; }
int Utente_programma::getIdProgramma() const { return id_programma; }
date Utente_programma::getDataInizio() const { return data_inizio; }
std::string Utente_programma::getStato() const { return stato; }