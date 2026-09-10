//
// Created by giorg on 23/08/2026.
//

#include "Feedback.h"
#include <utility>
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

Feedback::Feedback(int id_feedback, int valutazione, string commento, date data, int id_utente, int id_programma,
    int id_piano)
        :id_feedback(id_feedback), valutazione(valutazione), commento(std::move(commento)), data(data), id_utente(id_utente),
        id_programma(id_programma), id_piano(id_piano){}

int Feedback::getIdFeedback() const { return id_feedback; }
int Feedback::getValutazione() const { return valutazione; }
string Feedback::getCommento() const { return commento; }
string Feedback::getDataStr() const { return dateToStr(data); }
int Feedback::getIdUtente() const { return id_utente; }
int Feedback::getIdProgramma() const { return id_programma; }
int Feedback::getIdPiano() const { return id_piano; }