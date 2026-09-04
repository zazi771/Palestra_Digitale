//
// Created by giorg on 21/08/2026.
//

#include "Cartella_clinica.h"
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

Cartella_clinica::Cartella_clinica( int id_cartella, int id_cliente, date data_rilevazione, float altezza_cm,
    float peso_kg, float circonferenza_vita_cm, float circonferenza_fianchi_cm, float massa_grassa_percentuale,
    float massa_magra_kg, string patologie, string allergie, string intolleranze_alimentari, string infortuni_pregressi,
    string farmaci_assunti, string livello_attivita_fisica, string obiettivo, string note_mediche)
    : id_cartella(id_cartella), id_cliente(id_cliente), data_rilevazione(data_rilevazione),  altezza_cm(altezza_cm),
        peso_kg(peso_kg), circonferenza_vita_cm(circonferenza_vita_cm),
        circonferenza_fianchi_cm(circonferenza_fianchi_cm), massa_grassa_percentuale(massa_grassa_percentuale),
        massa_magra_kg(massa_magra_kg), patologie(std::move(patologie)), allergie(std::move(allergie)),
        intolleranze_alimentari(std::move(intolleranze_alimentari)), infortuni_pregressi(std::move(infortuni_pregressi)),
        farmaci_assunti(std::move(farmaci_assunti)), livello_attivita_fisica(std::move(livello_attivita_fisica)),
        obiettivo(std::move(obiettivo)), note_mediche(std::move(note_mediche)) {}

int Cartella_clinica::getId() const { return id_cartella; }
int Cartella_clinica::getIdCliente() const { return id_cliente; }
date Cartella_clinica::getDataRilevazione() const { return data_rilevazione; }
std::string Cartella_clinica::getDataRilevazioneStr() const { return dateToStr(data_rilevazione); }
float Cartella_clinica::getAltezza() const { return altezza_cm; }
float Cartella_clinica::getPeso() const { return peso_kg; }
float Cartella_clinica::getCircVita() const { return circonferenza_vita_cm; }
float Cartella_clinica::getCircFianchi() const { return circonferenza_fianchi_cm; }
float Cartella_clinica::getMassaGrassa() const { return massa_grassa_percentuale; }
float Cartella_clinica::getMassaMagra() const { return massa_magra_kg; }
std::string Cartella_clinica::getPatologie() const { return patologie; }
std::string Cartella_clinica::getAllergie() const { return allergie; }
std::string Cartella_clinica::getIntolleranze() const { return intolleranze_alimentari; }
std::string Cartella_clinica::getInfortuni() const { return infortuni_pregressi; }
std::string Cartella_clinica::getFarmaci() const { return farmaci_assunti; }
std::string Cartella_clinica::getLivelloAttivita() const { return livello_attivita_fisica; }
std::string Cartella_clinica::getObiettivo() const { return obiettivo; }
std::string Cartella_clinica::getNoteMediche() const { return note_mediche; }