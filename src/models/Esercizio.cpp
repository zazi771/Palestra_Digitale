//
// Created by giorg on 23/08/2026.
//

#include "Esercizio.h"
#include <utility>

Esercizio::Esercizio(int id_esercizio, string nome, string descrizione, string gruppo_muscolare, string url_video)
    :id_esercizio(id_esercizio), nome(std::move(nome)), descrizione(std::move(descrizione)),
        gruppo_muscolare(std::move(gruppo_muscolare)), url_video(std::move(url_video)){}

int Esercizio::getId() const { return id_esercizio; }
std::string Esercizio::getNome() const { return nome; }
std::string Esercizio::getDescrizione() const { return descrizione; }
std::string Esercizio::getGruppoMuscolare() const { return gruppo_muscolare; }
std::string Esercizio::getUrlVideo() const { return url_video; }