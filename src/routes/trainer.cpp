//
// Created by giorg on 04/09/2026.
//

#include "trainer.h"
#include "uploads.h"
#include <sstream>
#include <unordered_map>

namespace  {

// Costruisce una mappa id Esercizio -> Esercizio per arricchire i programmi con nome/gruppo/video.
std::unordered_map<int, Esercizio> mappaEsercizi(Database& db) {
    std::unordered_map<int, Esercizio> m;
    for (const auto& e : db.getTuttiEsercizi()) m.emplace(e.getId(), e);
    return m;
}


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

crow::json::wvalue utenteSintesi(const Utente& u, int nPiani) {
    crow::json::wvalue w;
    w["id"] = u.getId();
    w["email"] = u.getEmail();
    w["nome"] = u.getNome();
    w["cognome"] = u.getCognome();
    w["data_registrazione"] = u.getDataRegistrazioneStr();
    w["n_piani"] = nPiani;
    return w;
}

}

void registraTrainerRoutes(crow::SimpleApp& app, Database& db, const std::string& uploadsDir) {

    // GET /api/trainer/<id>  -> dati profilo trainer
    CROW_ROUTE(app, "/api/trainer/<int>")
    ([&db](int id) {
        try {
            Utente u = db.getUtenteById(id);
            crow::json::wvalue w;
            w["id"] = u.getId();
            w["email"] = u.getEmail();
            w["nome"] = u.getNome();
            w["cognome"] = u.getCognome();
            w["ruolo"] = u.getRuolo();
            return crow::response(200, w);
        } catch (const std::exception&) {
            return crow::response(404, R"({"errore":"Utente non trovato"})");
        }
    });

    // POST /api/trainer/<id>/certificazione  -> salva la certificazione (multipart/form-data,
    // con i campi di testo professione/codice/ente/data e i file "certificazione" e "cv")
    CROW_ROUTE(app, "/api/trainer/<int>/certificazione").methods(crow::HTTPMethod::POST)
    ([&db, &uploadsDir](const crow::request& req, int id) {
        std::string ente, rilascio, scadenza, codice;
        std::string nomeCertificazione, nomeCv;

        try {
            crow::multipart::message msg(req);
            auto str = [&](const std::string& k, const std::string& def) {
                auto p = msg.get_part_by_name(k);
                return p.body.empty() ? def : p.body;
            };
            ente = str("ente_rilascio", "");
            rilascio = str("data_rilascio", "");
            scadenza = str("data_scadenza", "");
            codice = str("codice", "");

            // File (se presenti) salvati nella cartella uploads
            auto partCert = msg.get_part_by_name("certificazione");
            if (!partCert.body.empty()) {
                if (!uploads::salvaFile(uploadsDir, partCert, nomeCertificazione))
                    return crow::response(500, R"({"errore":"Errore salvataggio certificazione"})");
            }
            auto partCv = msg.get_part_by_name("cv");
            if (!partCv.body.empty()) {
                if (!uploads::salvaFile(uploadsDir, partCv, nomeCv))
                    return crow::response(500, R"({"errore":"Errore salvataggio CV"})");
            }
        } catch (const std::exception&) {
            return crow::response(400, R"({"errore":"Body multipart non valido"})");
        }



        int codiceInt = 0;
        try { codiceInt = std::stoi(codice); } catch (...) { codiceInt = 0; }

        Certificazione c(0, id, nomeCv, nomeCertificazione, ente, codiceInt,
                         parseDateISO(rilascio), parseDateISO(scadenza));
        if (!db.inserisciCertificazione(c)) {
            return crow::response(500, R"({"errore":"Errore durante il salvataggio"})");
        }
        return crow::response(201, R"({"esito":"ok"})");
    });

    // GET /api/trainer/<id>/certificazione  -> recupera certificazione del trainer
    CROW_ROUTE(app, "/api/trainer/<int>/certificazione")
    ([&db](int id) {
        auto c = db.getCertificazioneByEsperto(id);
        if (!c) return crow::response(404, R"({"errore":"Nessuna certificazione trovata"})");
        crow::json::wvalue w;
        w["id"] = c->getId();
        w["certificazione"] = c->getCertificazione();
        w["ente_rilascio"] = c->getEnteRilascio();
        w["codice"] = c->getCodice();
        w["cv"] = c->getCv();
        w["data_rilascio"] = c->getDataRilascioStr();
        w["data_scadenza"] = c->getDataScadenzaStr();
        return crow::response(200, w);
    });

    // GET /api/trainer/<id>/clienti  -> elenco clienti (con conteggio piani)
    CROW_ROUTE(app, "/api/trainer/<int>/clienti")
    ([&db](int /*idTrainer*/) {
        auto clienti = db.getUtentiByRuolo("cliente");
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& cl : clienti) {
            int nProgrammi = (int)db.getProgrammiByCliente(cl.getId()).size();
            arr.push_back(utenteSintesi(cl, nProgrammi));
        }
        w["clienti"] = std::move(arr);
        return crow::response(200, w);
    });

    // GET /api/trainer/<id>/clienti/<clienteId>/piani  -> piani di un cliente (con esercizi)
    CROW_ROUTE(app, "/api/trainer/<int>/clienti/<int>/piani")
    ([&db](int /*idTrainer*/, int clienteId) {
        auto programmi = db.getProgrammiByCliente(clienteId);
        auto mappaEs = mappaEsercizi(db);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& pr : programmi) {
            crow::json::wvalue pj;
            pj["id"] = pr.getId();
            pj["id_trainer"] = pr.getIdTrainer();
            pj["nome"] = pr.getNome();
            pj["obiettivo"] = pr.getObiettivo();
            pj["livello_difficolta"] = pr.getLivelloDifficolta();
            pj["durata_settimane"] = pr.getDurataSettimane();
            pj["descrizione"] = pr.getDescrizione();

            auto esercizi = db.getEserciziByProgramma(pr.getId());
            std::vector<crow::json::wvalue> arrEs;
            for (const auto& e : esercizi) {
                crow::json::wvalue ej;
                ej["id"] = e.getId();
                ej["id_esercizio"] = e.getIdEsercizio();
                ej["ordine"] = e.getOrdine();
                ej["serie"] = e.getSerie();
                ej["ripetizioni"] = e.getRipetizioni();
                ej["recupero_sec"] = e.getRecupero();
                auto it = mappaEs.find(e.getIdEsercizio());
                if (it != mappaEs.end()) {
                    ej["nome"] = it->second.getNome();
                    ej["descrizione"] = it->second.getDescrizione();
                    ej["gruppo_muscolare"] = it->second.getGruppoMuscolare();
                    ej["url_video"] = it->second.getUrlVideo();
                } else {
                    ej["nome"] = "";
                    ej["descrizione"] = "";
                    ej["gruppo_muscolare"] = "";
                    ej["url_video"] = "";
                }
                arrEs.push_back(std::move(ej));
            }
            pj["esercizi"] = std::move(arrEs);
            arr.push_back(std::move(pj));
        }
        w["piani"] = std::move(arr);
        return crow::response(200, w);
    });

    // POST /api/trainer/<id>/clienti/<clienteId>/piani  -> crea programma + assegna
    CROW_ROUTE(app, "/api/trainer/<int>/clienti/<int>/piani").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int idTrainer, int clienteId) {
        try{
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, R"({"errore":"Body JSON non valido"})");

            std::string nome = body.has("nome") ? std::string(body["nome"]) : "";
            std::string obiettivo = body.has("obiettivo") ? std::string(body["obiettivo"]) : "";
            std::string livello = body.has("livello_difficolta") ? std::string(body["livello_difficolta"]) : "";
            int durata = body.has("durata_settimane") ? (int)body["durata_settimane"] : 0;
            std::string descrizione = body.has("descrizione") ? std::string(body["descrizione"]) : "";

            if (nome.empty() || obiettivo.empty() || livello.empty()) {
                return crow::response(400, R"({"errore":"Campi obbligatori mancanti"})");
            }

            // 1) inserisce il programma e ottiene l'id
            Programma_allenamento prog(0, idTrainer, nome, obiettivo, livello, durata, descrizione);
            int idProgramma = db.inserisciProgramma(prog);
            if (idProgramma <= 0) return crow::response(500, R"({"errore":"Errore salvataggio programma"})");

            // 2) assegna il programma al cliente
            auto oggi = std::chrono::system_clock::now();
            auto tt = std::chrono::system_clock::to_time_t(oggi);
            std::tm tm = *std::localtime(&tt);
            std::ostringstream oss;
            oss << (tm.tm_year + 1900) << "-"
                << (tm.tm_mon + 1 < 10 ? "0" : "") << (tm.tm_mon + 1) << "-"
                << (tm.tm_mday < 10 ? "0" : "") << tm.tm_mday;
            db.assegnaProgramma(clienteId, idProgramma, oss.str());

            // 3) inserisce gli esercizi
            if (body.has("esercizi") && body["esercizi"].t() == crow::json::type::List) {
                int ordine = 1;
                for (const auto& es : body["esercizi"]) {
                    std::string esNome = es.has("nome") ? std::string(es["nome"]) : "";
                    std::string esDesc = es.has("descrizione") ? std::string(es["descrizione"]) : "";
                    std::string esGruppo = es.has("gruppo_muscolare") ? std::string(es["gruppo_muscolare"]) : "";
                    std::string esVideo = es.has("url_video") ? std::string(es["url_video"]) : "";
                    int esSerie = es.has("serie") ? (int)es["serie"] : 0;
                    std::string esRip = es.has("ripetizioni") ? std::string(es["ripetizioni"]) : "";
                    int esRecupero = es.has("recupero_sec") ? (int)es["recupero_sec"] : 0;

                    if (esNome.empty()) continue;

                    // Crea (o riusa) l'esercizio e ottiene l'id
                    Esercizio ex(0, esNome, esDesc, esGruppo, esVideo);
                    int idEsercizio = db.inserisciEsercizio(ex);

                    Programma_esercizio pe(0, idProgramma, idEsercizio, ordine, esSerie, esRip, esRecupero);
                    db.inserisciProgrammaEsercizio(pe);
                    ordine++;
                }
            }

            crow::json::wvalue w;
            w["id"] = idProgramma;
            return crow::response(201, w);
        } catch (const std::exception& e) {
        return crow::response(500, std::string(R"({"errore":")") + e.what() + R"("})");
        } catch (...) {
            return crow::response(500, R"({"errore":"Errore sconosciuto"})");
        }
    });

    // GET /api/esercizi  -> elenco di tutti gli esercizi disponibili
    CROW_ROUTE(app, "/api/esercizi")
    ([&db]() {
        auto esercizi = db.getTuttiEsercizi();
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& e : esercizi) {
            crow::json::wvalue ej;
            ej["id"] = e.getId();
            ej["nome"] = e.getNome();
            ej["descrizione"] = e.getDescrizione();
            ej["gruppo_muscolare"] = e.getGruppoMuscolare();
            ej["url_video"] = e.getUrlVideo();
            arr.push_back(std::move(ej));
        }
        w["esercizi"] = std::move(arr);
        return crow::response(200, w);
    });

}
