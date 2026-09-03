//
// Created by giorg on 23/08/2026.
//

#include "Programma_esercizio.h"

#include <utility>

Programma_esercizio::Programma_esercizio(int id_programma_esercizio, int id_programma, int id_esercizio, int ordine,
    int serie, string ripetizioni, int recupero_sec)
        :id_programma_esercizio(id_programma_esercizio), id_programma(id_programma), id_esercizio(id_esercizio),
        ordine(ordine), serie(serie), ripetizioni(std::move(ripetizioni)), recupero_sec(recupero_sec){}