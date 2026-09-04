//
// Created by giorg on 22/08/2026.
//

#include "Certificazione.h"
#include <utility>

Certificazione::Certificazione(int id_certificazione, int id_esperto, string cv, string certificazione,
    string professione, string ente_rilascio, int codice_certificazione, date data_rilascio, date data_scadenza)
    :id_certificazione(id_certificazione), id_esperto(id_esperto), cv(std::move(cv)),
        certificazione(std::move(certificazione)), professione(std::move(professione)),
        ente_rilascio(std::move(ente_rilascio)), codice_certificazione(codice_certificazione),
        data_rilascio(data_rilascio), data_scadenza(data_scadenza){}