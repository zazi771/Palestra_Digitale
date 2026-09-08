//
// Created by giorg on 04/09/2026.
//

#include "nutrizionista.h"
#include "uploads.h"
#include <sstream>
#include <cctype>
#include <unordered_map>

namespace  {

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

int giornoToInt(const std::string& g) {
    // Riceviamo "Lunedì", "Martedì", ... e restituiamo 1..7. Default: 1.
    std::string g1 = g;
    for (auto& c : g1) c = (char)std::tolower((unsigned char)c);
    if (g1.find("luned") != std::string::npos) return 1;
    if (g1.find("marted") != std::string::npos) return 2;
    if (g1.find("mercol") != std::string::npos) return 3;
    if (g1.find("gioved") != std::string::npos) return 4;
    if (g1.find("venerd") != std::string::npos) return 5;
    if (g1.find("sabato") != std::string::npos) return 6;
    if (g1.find("domen") != std::string::npos) return 7;
    return 1;
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

// Costruisce una mappa id Cibo -> Cibo per arricchire i pasti con il nome.
std::unordered_map<int, Cibo> mappaCibi(Database& db) {
    std::unordered_map<int, Cibo> m;
    for (const auto& c : db.getTuttiCibi()) m.emplace(c.getId(), c);
    return m;
}

// Trova l'id del cibo per nome; se non esiste lo inserisce (kcal a 0).
int trovaOInserisciCibo(Database& db, const std::string& nome) {
    auto candidati = db.cercaCibi(nome);
    for (const auto& c : candidati) {
        std::string cn = c.getNome();
        for (auto& ch : cn) ch = (char)std::tolower((unsigned char)ch);
        std::string n = nome;
        for (auto& ch : n) ch = (char)std::tolower((unsigned char)ch);
        if (cn == n) return c.getId();
    }
    Cibo nuovo(0, nome, 0.0f, 0.0f, 0.0f, 0.0f);
    return db.inserisciCibo(nuovo);
}

}

void registraNutrizionistaRoutes(crow::SimpleApp& app, Database& db, const std::string& uploadsDir) {

    // GET /api/nutrizionista/<id>  -> dati profilo nutrizionista
    CROW_ROUTE(app, "/api/nutrizionista/<int>")
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

    // POST /api/nutrizionista/<id>/certificazione  -> salva la certificazione (multipart/form-data)
    CROW_ROUTE(app, "/api/nutrizionista/<int>/certificazione").methods(crow::HTTPMethod::POST)
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

    // GET /api/nutrizionista/<id>/certificazione
    CROW_ROUTE(app, "/api/nutrizionista/<int>/certificazione")
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

    // GET /api/nutrizionista/<id>/clienti
    CROW_ROUTE(app, "/api/nutrizionista/<int>/clienti")
    ([&db](int /*idNutri*/) {
        auto clienti = db.getUtentiByRuolo("cliente");
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& cl : clienti) {
            int nPiani = (int)db.getPianiByCliente(cl.getId()).size();
            arr.push_back(utenteSintesi(cl, nPiani));
        }
        w["clienti"] = std::move(arr);
        return crow::response(200, w);
    });

    // GET /api/nutrizionista/<id>/clienti/<clienteId>/piani  -> piani di un cliente (con pasti e alimenti)
    CROW_ROUTE(app, "/api/nutrizionista/<int>/clienti/<int>/piani")
    ([&db](int /*idNutri*/, int clienteId) {
        auto piani = db.getPianiByCliente(clienteId);
        auto mappaC = mappaCibi(db);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& p : piani) {
            crow::json::wvalue pj;
            pj["id"] = p.getId();
            pj["id_nutrizionista"] = p.getIdNutrizionista();
            pj["nome"] = p.getNome();
            pj["descrizione"] = p.getDescrizione();
            pj["id_cliente"] = p.getIdCliente();

            auto pasti = db.getPastiByPiano(p.getId());
            std::vector<crow::json::wvalue> arrPasti;
            for (const auto& pa : pasti) {
                crow::json::wvalue paj;
                paj["id"] = pa.getId();
                paj["giorno"] = pa.getGiorno();
                paj["tipo_pasto"] = pa.getTipoPasto();

                // alimenti: righe pasto_cibo collegate a questo pasto
                std::vector<crow::json::wvalue> arrAl;
                auto alimenti = db.getAlimentiByPasto(pa.getId());
                for (const auto& pc : alimenti) {
                    crow::json::wvalue aj;
                    auto it = mappaC.find(pc.getIdCibo());
                    aj["cibo"] = (it != mappaC.end()) ? it->second.getNome() : "Cibo";
                    aj["quantita_gr"] = pc.getQuantita();
                    arrAl.push_back(std::move(aj));
                }
                paj["alimenti"] = std::move(arrAl);
                arrPasti.push_back(std::move(paj));
            }
            pj["pasti"] = std::move(arrPasti);
            arr.push_back(std::move(pj));
        }
        w["piani"] = std::move(arr);
        return crow::response(200, w);
    });

    // POST /api/nutrizionista/<id>/clienti/<clienteId>/piani  -> crea piano alimentare
    CROW_ROUTE(app, "/api/nutrizionista/<int>/clienti/<int>/piani").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int idNutri, int clienteId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, R"({"errore":"Body JSON non valido"})");

        std::string nome = body.has("nome") ? std::string(body["nome"]) : "";
        std::string descrizione = body.has("descrizione") ? std::string(body["descrizione"]) : "";

        if (nome.empty()) return crow::response(400, R"({"errore":"Campi obbligatori mancanti"})");

        // 1) inserisce il piano alimentare
        Piano_alimentare piano(0, idNutri, nome, descrizione, clienteId);
        int idPiano = db.inserisciPianoAlimentare(piano);
        if (idPiano <= 0) return crow::response(500, R"({"errore":"Errore salvataggio piano"})");

        // 2) inserisce pasti + alimenti
        if (body.has("pasti") && body["pasti"].t() == crow::json::type::List) {
            for (const auto& pa : body["pasti"]) {
                std::string giorno = pa.has("giorno") ? std::string(pa["giorno"]) : "";
                std::string tipo = pa.has("tipo_pasto") ? std::string(pa["tipo_pasto"]) : "Pasto";
                int giornoNum = giornoToInt(giorno);

                if (pa.has("alimenti") && pa["alimenti"].t() == crow::json::type::List) {
                    for (const auto& al : pa["alimenti"]) {
                        std::string ciboNome = al.has("cibo") ? std::string(al["cibo"]) : "";
                        int quantita = al.has("quantita_gr") ? (int)al["quantita_gr"] : 0;
                        if (ciboNome.empty()) continue;

                        int idCibo = trovaOInserisciCibo(db, ciboNome);

                        // Un pasto per alimento (per visualizzare nome + quantità)
                        Pasto pasto(0, idPiano, idCibo, giornoNum, tipo);
                        int idPasto = db.inserisciPasto(pasto);
                        Pasto_cibo pc(0, idPasto, idCibo, quantita);
                        db.inserisciPastoCibo(pc);
                    }
                }
            }
        }

        crow::json::wvalue w;
        w["id"] = idPiano;
        return crow::response(201, w);
    });

    // GET /api/cibi  -> elenco alimenti disponibili (con opzione ?q= per ricerca)
    CROW_ROUTE(app, "/api/cibi")
    ([&db](const crow::request& req) {
        std::string q = req.url_params.get("q") ? req.url_params.get("q") : "";
        auto cibi = q.empty() ? db.getTuttiCibi() : db.cercaCibi(q);

        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& c : cibi) {
            crow::json::wvalue cj;
            cj["id"] = c.getId();
            cj["nome"] = c.getNome();
            cj["kcal"] = c.getKcal();
            cj["carboidrati"] = c.getCarboidrati();
            cj["proteine"] = c.getProteine();
            cj["grassi"] = c.getGrassi();
            arr.push_back(std::move(cj));
        }
        w["cibi"] = std::move(arr);
        return crow::response(200, w);
    });

}
