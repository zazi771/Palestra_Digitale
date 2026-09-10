//
// Created by giorg on 04/09/2026.
//

#include "auth.h"
#include "bcrypt_wrapper.h"
#include <sstream>

namespace {

// Converte "YYYY-MM-DD" in date (year_month_day)
date parseDateISO(const std::string& s) {
    std::istringstream iss(s);
    int y, m, d;
    char sep;
    if (iss >> y >> sep >> m >> sep >> d) {
        return date{std::chrono::year{y}, std::chrono::month{static_cast<unsigned>(m)},
                    std::chrono::day{static_cast<unsigned>(d)}};
    }
    return date{};
}

// Costruisce l'oggetto JSON con i dati pubblici dell'utente
crow::json::wvalue toJson(const Utente& u) {
    crow::json::wvalue w;
    w["id"] = u.getId();
    w["email"] = u.getEmail();
    w["nome"] = u.getNome();
    w["cognome"] = u.getCognome();
    w["ruolo"] = u.getRuolo();
    w["sesso"] = u.getSesso();
    w["data_registrazione"] = u.getDataRegistrazioneStr();
    w["data_nascita"] = u.getDataNascitaStr();
    return w;
}

}

void registraAuthRoutes(crow::SimpleApp& app, Database& db) {

    // POST /api/auth/registra  -> crea un nuovo utente
    CROW_ROUTE(app, "/api/auth/registra").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"errore":"Body JSON non valido"})");
        }

        std::string email = body.has("email") ? std::string(body["email"]) : "";
        std::string password = body.has("password") ? std::string(body["password"]) : "";
        std::string nome = body.has("nome") ? std::string(body["nome"]) : "";
        std::string cognome = body.has("cognome") ? std::string(body["cognome"]) : "";
        std::string ruolo = body.has("ruolo") ? std::string(body["ruolo"]) : "";
        std::string sesso = body.has("sesso") ? std::string(body["sesso"]) : "";
        std::string dataNascita = body.has("data_nascita") ? std::string(body["data_nascita"]) : "";

        if (email.empty() || password.empty() || nome.empty() || cognome.empty() || ruolo.empty()) {
            return crow::response(400, R"({"errore":"Campi obbligatori mancanti"})");
        }
        if (db.getUtenteByEmail(email).has_value()) {
            return crow::response(409, R"({"errore":"Email già registrata"})");
        }

        // data_registrazione = oggi (calcolata al volo in formato ISO)
        auto oggi = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(oggi);
        std::tm tm = *std::localtime(&tt);
        std::ostringstream oss;
        oss << (tm.tm_year + 1900) << "-"
            << (tm.tm_mon + 1 < 10 ? "0" : "") << (tm.tm_mon + 1) << "-"
            << (tm.tm_mday < 10 ? "0" : "") << tm.tm_mday;

        std::string passwordHash = bcrypt::generateHash(password);

        Utente nuovo(email, passwordHash, nome, cognome, ruolo, parseDateISO(oss.str()), sesso,
                     parseDateISO(dataNascita));

        if (!db.inserisciUtente(nuovo)) {
            return crow::response(500, R"({"errore":"Errore durante la registrazione"})");
        }

        auto creato = db.getUtenteByEmail(email);
        if (!creato) {
            return crow::response(500, R"({"errore":"Errore durante la registrazione"})");
        }
        return crow::response(201, toJson(*creato));
    });

    // POST /api/auth/login  -> verifica credenziali, ritorna i dati utente
    CROW_ROUTE(app, "/api/auth/login").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"errore":"Body JSON non valido"})");
        }

        std::string email = body.has("email") ? std::string(body["email"]) : "";
        std::string password = body.has("password") ? std::string(body["password"]) : "";

        auto utente = db.getUtenteByEmail(email);
        if (!utente || !bcrypt::validatePassword(password, utente->getPasswordHash())) {
            return crow::response(401, R"({"errore":"Credenziali non valide"})");
        }

        auto w = toJson(*utente);
        w["accesso"] = "ok";
        return crow::response(200, w);
    });

}
