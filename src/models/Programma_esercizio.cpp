//
// Created by giorg on 23/08/2026.
//

#include "Programma_esercizio.h"

#include <utility>

Programma_esercizio::Programma_esercizio(int id_programma_esercizio, int id_programma, int id_esercizio, int ordine,
    int serie, string ripetizioni, int recupero_sec)
        :id_programma_esercizio(id_programma_esercizio), id_programma(id_programma), id_esercizio(id_esercizio),
        ordine(ordine), serie(serie), ripetizioni(std::move(ripetizioni)), recupero_sec(recupero_sec){}

int Programma_esercizio::getId() const { return id_programma_esercizio; }
int Programma_esercizio::getIdProgramma() const { return id_programma; }
int Programma_esercizio::getIdEsercizio() const { return id_esercizio; }
int Programma_esercizio::getOrdine() const { return ordine; }
int Programma_esercizio::getSerie() const { return serie; }
std::string Programma_esercizio::getRipetizioni() const { return ripetizioni; }
int Programma_esercizio::getRecupero() const { return recupero_sec; }