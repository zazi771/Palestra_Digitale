//
// Created by giorg on 04/09/2026.
//

#include "cliente.h"
#include <sstream>
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

crow::json::wvalue cartellaToJson(const Cartella_clinica& c) {
    crow::json::wvalue w;
    w["id"] = c.getId();
    w["id_cliente"] = c.getIdCliente();
    w["data_rilevazione"] = c.getDataRilevazioneStr();
    w["altezza_cm"] = c.getAltezza();
    w["peso_kg"] = c.getPeso();
    w["circonferenza_vita_cm"] = c.getCircVita();
    w["circonferenza_fianchi_cm"] = c.getCircFianchi();
    w["massa_grassa_percentuale"] = c.getMassaGrassa();
    w["massa_magra_kg"] = c.getMassaMagra();
    w["patologie"] = c.getPatologie();
    w["allergie"] = c.getAllergie();
    w["intolleranze_alimentari"] = c.getIntolleranze();
    w["infortuni_pregressi"] = c.getInfortuni();
    w["farmaci_assunti"] = c.getFarmaci();
    w["livello_attivita_fisica"] = c.getLivelloAttivita();
    w["obiettivo"] = c.getObiettivo();
    w["note_mediche"] = c.getNoteMediche();
    return w;
}

// Converte la quantità di una riga pasto_cibo in kcal approssimative usando i cibi
double quantitaKcal(double kcal100, int grammi) {
    return kcal100 * grammi / 100.0;
}

// Mappa id Esercizio -> Esercizio per arricchire i programmi con nome/gruppo/video.
std::unordered_map<int, Esercizio> mappaEsercizi(Database& db) {
    std::unordered_map<int, Esercizio> m;
    for (const auto& e : db.getTuttiEsercizi()) m.emplace(e.getId(), e);
    return m;
}

// Mappa id Cibo -> Cibo per arricchire i pasti con il nome dell'alimento.
std::unordered_map<int, Cibo> mappaCibi(Database& db) {
    std::unordered_map<int, Cibo> m;
    for (const auto& c : db.getTuttiCibi()) m.emplace(c.getId(), c);
    return m;
}

}

void registraClienteRoutes(crow::SimpleApp& app, Database& db) {

    // GET /api/cliente/<id>  -> dati profilo del cliente
    CROW_ROUTE(app, "/api/cliente/<int>")
    ([&db](int id) {
        try {
            Utente u = db.getUtenteById(id);
            crow::json::wvalue w;
            w["id"] = u.getId();
            w["email"] = u.getEmail();
            w["nome"] = u.getNome();
            w["cognome"] = u.getCognome();
            w["ruolo"] = u.getRuolo();
            w["data_registrazione"] = u.getDataRegistrazioneStr();
            return crow::response(200, w);
        } catch (const std::exception&) {
            return crow::response(404, R"({"errore":"Utente non trovato"})");
        }
    });

    // GET /api/cliente/<id>/cartella-clinica  -> storico rilevazioni
    CROW_ROUTE(app, "/api/cliente/<int>/cartella-clinica")
    ([&db](int id) {
        auto cartelle = db.getCartelleByCliente(id);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& c : cartelle) arr.push_back(cartellaToJson(c));
        w["cartelle"] = std::move(arr);
        return crow::response(200, w);
    });

    // POST /api/cliente/<id>/cartella-clinica  -> nuova rilevazione
    CROW_ROUTE(app, "/api/cliente/<int>/cartella-clinica").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int id) {
        try {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, R"({"errore":"Body JSON non valido"})");

            std::string data;
            if (body.has("data_rilevazione")) data = std::string(body["data_rilevazione"].s());
            float altezza = body.has("altezza_cm") ? (float)body["altezza_cm"].d() : 0;
            float peso = body.has("peso_kg") ? (float)body["peso_kg"].d() : 0;
            float vita = body.has("circonferenza_vita_cm") ? (float)body["circonferenza_vita_cm"].d() : 0;
            float fianchi = body.has("circonferenza_fianchi_cm") ? (float)body["circonferenza_fianchi_cm"].d() : 0;
            float grassa = body.has("massa_grassa_percentuale") ? (float)body["massa_grassa_percentuale"].d() : 0;
            float magra = body.has("massa_magra_kg") ? (float)body["massa_magra_kg"].d() : 0;
            auto str = [&](const char* k) -> std::string {
                if (body.has(k)) return std::string(body[k].s());
                return "";
            };

            Cartella_clinica c(0, id, parseDateISO(data), altezza, peso, vita, fianchi, grassa, magra,
                str("patologie"), str("allergie"), str("intolleranze_alimentari"), str("infortuni_pregressi"),
                str("farmaci_assunti"), str("livello_attivita_fisica"), str("obiettivo"), str("note_mediche"));

            if (!db.inserisciCartella(c)) {
                return crow::response(500, R"({"errore":"Errore durante il salvataggio"})");
            }
            return crow::response(201, R"({"esito":"ok"})");
        } catch (const std::exception& e) {
            return crow::response(500, std::string(R"({"errore":")") + e.what() + R"("})");
        } catch (...) {
            return crow::response(500, R"({"errore":"Errore sconosciuto"})");
        }
    });

    // GET /api/cliente/<id>/piano-alimentare  -> piani alimentari assegnati
    CROW_ROUTE(app, "/api/cliente/<int>/piano-alimentare")
    ([&db](int id) {
        auto piani = db.getPianiByCliente(id);
        auto mappaC = mappaCibi(db);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arrPiani;
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
            arrPiani.push_back(std::move(pj));
        }
        w["piani"] = std::move(arrPiani);
        return crow::response(200, w);
    });

    // GET /api/cliente/<id>/programma-allenamento  -> programmi allenamento assegnati
    CROW_ROUTE(app, "/api/cliente/<int>/programma-allenamento")
    ([&db](int id) {
        auto programmi = db.getProgrammiByCliente(id);
        auto mappaEs = mappaEsercizi(db);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arrProg;
        for (const auto& pr : programmi) {
            crow::json::wvalue prj;
            prj["id"] = pr.getId();
            prj["id_trainer"] = pr.getIdTrainer();
            prj["nome"] = pr.getNome();
            prj["obiettivo"] = pr.getObiettivo();
            prj["livello_difficolta"] = pr.getLivelloDifficolta();
            prj["durata_settimane"] = pr.getDurataSettimane();
            prj["descrizione"] = pr.getDescrizione();

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
            prj["esercizi"] = std::move(arrEs);
            arrProg.push_back(std::move(prj));
        }
        w["programmi"] = std::move(arrProg);
        return crow::response(200, w);
    });

    // GET /api/cliente/<id>/sessioni  -> sessioni di allenamento svolte
    CROW_ROUTE(app, "/api/cliente/<int>/sessioni")
    ([&db](int id) {
        auto sessioni = db.getSessioniByCliente(id);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;
        for (const auto& s : sessioni) {
            crow::json::wvalue sj;
            sj["id"] = s.getId();
            sj["id_programma"] = s.getIdProgramma();
            sj["data"] = s.getDataStr();
            sj["tempo_minuti"] = s.getTempoMinuti();
            sj["completato"] = s.getCompletato();
            arr.push_back(std::move(sj));
        }
        w["sessioni"] = std::move(arr);
        return crow::response(200, w);
    });

    // POST /api/cliente/<id>/sessioni  -> registra nuova sessione
    CROW_ROUTE(app, "/api/cliente/<int>/sessioni").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int id) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, R"({"errore":"Body JSON non valido"})");

        std::string data = body.has("data") ? std::string(body["data"]) : "";
        int idProgramma = body.has("id_programma") ? (int)body["id_programma"] : 0;
        int tempo = body.has("tempo_minuti") ? (int)body["tempo_minuti"] : 0;
        int completato = body.has("completato") ? (int)body["completato"] : 0;

        Sessione s(0, id, idProgramma, parseDateISO(data), tempo, completato);
        int nuovoId = db.inserisciSessione(s);
        if (nuovoId <= 0) return crow::response(500, R"({"errore":"Errore durante il salvataggio"})");
        crow::json::wvalue w;
        w["id"] = nuovoId;
        return crow::response(201, w);
    });

}
