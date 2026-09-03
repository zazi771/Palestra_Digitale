//
// Created by giorg on 20/08/2026.
//

#include "Utente.h"
#include <utility>

// Con id (utente esistente)
Utente::Utente(int id_utente, string email, string password, string nome, string cognome, string ruolo,
    date data_registrazione, string sesso, date data_nascita)
    :id_utente(id_utente), email(std::move(email)), password(std::move(password)), nome(std::move(nome)),
    cognome(std::move(cognome)), ruolo(std::move(ruolo)),
    data_registrazione(data_registrazione), sesso(std::move(sesso)), data_nascita(data_nascita){}

Utente::Utente(string email, string password, string nome, string cognome, string ruolo,
    date data_registrazione, string sesso, date data_nascita)
    :email(std::move(email)), password(std::move(password)), nome(std::move(nome)),
    cognome(std::move(cognome)), ruolo(std::move(ruolo)),
    data_registrazione(data_registrazione), sesso(std::move(sesso)), data_nascita(data_nascita){}

int Utente::getId() const { return id_utente; }
std::string Utente::getNome() const { return nome; }
std::string Utente::getRuolo() const { return ruolo; }