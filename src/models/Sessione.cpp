//
// Created by giorg on 23/08/2026.
//

#include "Sessione.h"
#include <sstream>
#include <iomanip>

static std::string dateToStr(date d) {
    if (!d.ok()) return "";
    std::ostringstream oss;
    oss << (int)d.year() << "-"
        << std::setw(2) << std::setfill('0') << (unsigned)d.month() << "-"
        << std::setw(2) << std::setfill('0') << (unsigned)d.day();
    return oss.str();
}

Sessione::Sessione(int id_sessione, int id_utente, int id_programma, date data, int tempo_minuti, int completato)
    :id_sessione(id_sessione), id_utente(id_utente), id_programma(id_programma), data(data), tempo_minuti(tempo_minuti),
    completato(completato){}

int Sessione::getId() const { return id_sessione; }
int Sessione::getIdUtente() const { return id_utente; }
int Sessione::getIdProgramma() const { return id_programma; }
date Sessione::getData() const { return data; }
std::string Sessione::getDataStr() const { return dateToStr(data); }
int Sessione::getTempoMinuti() const { return tempo_minuti; }
int Sessione::getCompletato() const { return completato; }