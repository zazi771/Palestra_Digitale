//
// Created by giorg on 22/08/2026.
//

#include "Certificazione.h"
#include <sstream>
#include <iomanip>
#include <utility>

static std::string dateToStr(date d) {
    if (!d.ok()) return "";
    std::ostringstream oss;
    oss << (int)d.year() << "-"
        << std::setw(2) << std::setfill('0') << (unsigned)d.month() << "-"
        << std::setw(2) << std::setfill('0') << (unsigned)d.day();
    return oss.str();
}

Certificazione::Certificazione(int id_certificazione, int id_esperto, string cv, string certificazione,
    string professione, string ente_rilascio, int codice_certificazione, date data_rilascio, date data_scadenza)
    :id_certificazione(id_certificazione), id_esperto(id_esperto), cv(std::move(cv)),
        certificazione(std::move(certificazione)), professione(std::move(professione)),
        ente_rilascio(std::move(ente_rilascio)), codice_certificazione(codice_certificazione),
        data_rilascio(data_rilascio), data_scadenza(data_scadenza){}

int Certificazione::getId() const { return id_certificazione; }
int Certificazione::getIdEsperto() const { return id_esperto; }
std::string Certificazione::getCv() const { return cv; }
std::string Certificazione::getCertificazione() const { return certificazione; }
std::string Certificazione::getProfessione() const { return professione; }
std::string Certificazione::getEnteRilascio() const { return ente_rilascio; }
int Certificazione::getCodice() const { return codice_certificazione; }
date Certificazione::getDataRilascio() const { return data_rilascio; }
std::string Certificazione::getDataRilascioStr() const { return dateToStr(data_rilascio); }
date Certificazione::getDataScadenza() const { return data_scadenza; }
std::string Certificazione::getDataScadenzaStr() const { return dateToStr(data_scadenza); }