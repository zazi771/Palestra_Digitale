//
// Created by giorg on 20/08/2026.
//

#include "Utente.h"
#include <sstream>
#include <iomanip>
#include <utility>

// Converte una data year_month_day in stringa "YYYY-MM-DD"
static std::string dateToStr(date d) {
    if (!d.ok()) return "";
    std::ostringstream oss;
    oss << (int)d.year() << "-"
        << std::setw(2) << std::setfill('0') << (unsigned)d.month() << "-"
        << std::setw(2) << std::setfill('0') << (unsigned)d.day();
    return oss.str();
}

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
std::string Utente::getEmail() const { return email; }
std::string Utente::getPasswordHash() const { return password; }
std::string Utente::getNome() const { return nome; }
std::string Utente::getCognome() const { return cognome; }
std::string Utente::getRuolo() const { return ruolo; }
std::string Utente::getSesso() const { return sesso; }
date Utente::getDataRegistrazione() const { return data_registrazione; }
std::string Utente::getDataRegistrazioneStr() const { return dateToStr(data_registrazione); }
date Utente::getDataNascita() const { return data_nascita; }
std::string Utente::getDataNascitaStr() const { return dateToStr(data_nascita); }