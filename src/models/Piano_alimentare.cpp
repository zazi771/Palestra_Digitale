//
// Created by giorg on 23/08/2026.
//

#include "Piano_alimentare.h"
#include <utility>

Piano_alimentare::Piano_alimentare(int id_piano, int id_nutrizionista, string nome, string descrizione, int id_cliente)
    :id_piano(id_piano), id_nutrizionista(id_nutrizionista), nome(std::move(nome)), descrizione(std::move(descrizione)),
        id_cliente(id_cliente){}

int Piano_alimentare::getId() const { return id_piano; }
int Piano_alimentare::getIdNutrizionista() const { return id_nutrizionista; }
std::string Piano_alimentare::getNome() const { return nome; }
std::string Piano_alimentare::getDescrizione() const { return descrizione; }
int Piano_alimentare::getIdCliente() const { return id_cliente; }