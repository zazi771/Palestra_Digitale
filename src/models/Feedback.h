//
// Created by giorg on 23/08/2026.
//

#ifndef PALESTRADIGITALE_FEEDBACK_H
#define PALESTRADIGITALE_FEEDBACK_H

#include <chrono>
using date = std::chrono::year_month_day;
#include <string>
using string = std::string;

class Feedback {
protected:
    //Parametri
    int id_feedback; //PK
    int valutazione;
    string commento;
    date data;
    int id_utente; //FK
    int id_programma; //FK
    int id_piano; //FK

public:
    //Costruttore
    Feedback(int id_feedback, int valutazione, string commento, date data, int id_utente, int id_programma, int id_piano);
};


#endif //PALESTRADIGITALE_FEEDBACK_H
