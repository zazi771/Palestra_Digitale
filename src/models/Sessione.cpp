//
// Created by giorg on 23/08/2026.
//

#include "Sessione.h"

Sessione::Sessione(int id_sessione, int id_utente, int id_programma, date data, int tempo_minuti, int completato)
    :id_sessione(id_sessione), id_utente(id_utente), id_programma(id_programma), data(data), tempo_minuti(tempo_minuti),
    completato(completato){}