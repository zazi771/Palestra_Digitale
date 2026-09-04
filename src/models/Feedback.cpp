//
// Created by giorg on 23/08/2026.
//

#include "Feedback.h"
#include <utility>

Feedback::Feedback(int id_feedback, int valutazione, string commento, date data, int id_utente, int id_programma,
    int id_piano)
        :id_feedback(id_feedback), valutazione(valutazione), commento(std::move(commento)), data(data), id_utente(id_utente),
        id_programma(id_programma), id_piano(id_piano){}