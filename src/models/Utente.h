//
// Created by giorg on 20/08/2026.
//

#ifndef PALESTRADIGITALE_UTENTE_H
#define PALESTRADIGITALE_UTENTE_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Utente {
protected:
    //Parametri
    int id_utente; //PK
    string email;
    string password;
    string nome;
    string cognome;
    string ruolo; //ENUM
    date data_registrazione;
    string sesso; //ENUM
    date data_nascita;

public:
    //Costruttore per utenti GIÀ esistenti nel DB (letti da una SELECT)
    Utente(int id_utente, string email, string password, string nome, string cognome, string ruolo,
        date data_registrazione, string sesso, date data_nascita);

    // Costruttore per NUOVI utenti (id non ancora assegnato)
    Utente(string email, string password, string nome, string cognome, string ruolo, date data_registrazione,
        string sesso, date data_nascita);

    //Metodi
    int getId() const;
    string getEmail() const;
    string getPasswordHash() const;
    string getNome() const;
    string getCognome() const;
    string getRuolo() const;
    string getSesso() const;
    date getDataRegistrazione() const;
    string getDataRegistrazioneStr() const;
    date getDataNascita() const;
    string getDataNascitaStr() const;

};


#endif //PALESTRADIGITALE_UTENTE_H
