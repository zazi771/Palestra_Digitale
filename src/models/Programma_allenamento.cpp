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

int Programma_allenamento::getId() const { return id_programma; }
int Programma_allenamento::getIdTrainer() const { return id_trainer; }
std::string Programma_allenamento::getNome() const { return nome; }
std::string Programma_allenamento::getObiettivo() const { return obiettivo; }
std::string Programma_allenamento::getLivelloDifficolta() const { return livello_difficolta; }
int Programma_allenamento::getDurataSettimane() const { return durata_settimane; }
std::string Programma_allenamento::getDescrizione() const { return descrizione; }