//
// Created by giorg on 21/08/2026.
//

#include "Cartella_clinica.h"
#include <utility>

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