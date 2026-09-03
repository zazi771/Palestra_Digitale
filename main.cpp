#include "Database.h"
#include <iostream>
#include <chrono>
using namespace  std::chrono;


int main() {
    try {
        Database db("PalestraDigitale.db");

        std::vector<Utente> tutti = db.getTuttiUtenti();
        for (const auto& u : tutti) {
            u.stampa();
        }

        Utente nuovo("luca@test.com", "hash123", "Luca", "Verdi",
            "cliente", {year{2026}, month{9}, day{2}}, "Maschio",
            {year{1998}, month{3}, day{22}});
        db.inserisciUtente(nuovo);

        Utente u = db.getUtenteById(1);
        u.stampa();

    } catch (const std::exception& e) {
        std::cerr << "Errore database: " << e.what() << std::endl;
    }

    return 0;
}