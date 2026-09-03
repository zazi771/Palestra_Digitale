//
// Created by giorg on 23/08/2026.
//

#include "Esercizio.h"
#include <utility>

Esercizio::Esercizio(int id_esercizio, string nome, string descrizione, string gruppo_muscolare, string url_video)
    :id_esercizio(id_esercizio), nome(std::move(nome)), descrizione(std::move(descrizione)),
        gruppo_muscolare(std::move(gruppo_muscolare)), url_video(std::move(url_video)){}