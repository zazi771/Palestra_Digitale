//
// Created by giorg on 23/08/2026.
//

#include "Programma_allenamento.h"
#include <utility>

Programma_allenamento::Programma_allenamento(int id_programma, int id_trainer, string nome, string obiettivo,
    string livello_difficolta, int durata_settimane, string descrizione)
    :id_programma(id_programma), id_trainer(id_trainer), nome(std::move(nome)), obiettivo(std::move(obiettivo)),
        livello_difficolta(std::move(livello_difficolta)), durata_settimane(durata_settimane),
        descrizione(std::move(descrizione)){}