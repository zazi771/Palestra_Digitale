
#include "cliente.h"
#include <sstream>
#include <unordered_map>

namespace  {

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

// Costruisce l'oggetto JSON con i dati della cartella clinica dell'utente
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

// Mappa id Esercizio -> Esercizio per popolare i programmi con nome/gruppo/video.
std::unordered_map<int, Esercizio> mappaEsercizi(Database& db) {
    std::unordered_map<int, Esercizio> m;
    for (const auto& e : db.getTuttiEsercizi()) m.emplace(e.getId(), e);
    return m;
}

// Mappa id Cibo -> Cibo per popolare i pasti con il nome dell'alimento.
std::unordered_map<int, Cibo> mappaCibi(Database& db) {
    std::unordered_map<int, Cibo> m;
    for (const auto& c : db.getTuttiCibi()) m.emplace(c.getId(), c);
    return m;
}

// Mappa id Programma -> AssegnazioneProgramma per popolare i programmi con stato/data_inizio.
std::unordered_map<int, AssegnazioneProgramma> mappaAssegnazioni(Database& db, int id_cliente) {
    std::unordered_map<int, AssegnazioneProgramma> m;
    for (const auto& a : db.getAssegnazioniByCliente(id_cliente)) m.emplace(a.id_programma, a);
    return m;
}

//Possibili stati di assegnazione di un programma d'allenamento
const std::vector<std::string> STATI_VALIDI = {"Non iniziato", "In corso", "Terminato"};
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
        auto mappaAssegnazioniProg = mappaAssegnazioni(db, id);
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

            auto itAss = mappaAssegnazioniProg.find(pr.getId());
            if (itAss != mappaAssegnazioniProg.end()) {
                prj["stato"] = itAss->second.stato;
                prj["data_inizio"] = itAss->second.data_inizio;
            } else {
                prj["stato"] = "Non iniziato";
                prj["data_inizio"] = "";
            }

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
    
    // PATCH /api/cliente/<id>/programmi/<id_programma>/stato  -> aggiorna stato assegnazione
    CROW_ROUTE(app, "/api/cliente/<int>/programmi/<int>/stato").methods(crow::HTTPMethod::PATCH)
    ([&db](const crow::request& req, int id, int idProgramma) {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("stato")) {
            return crow::response(400, R"({"errore":"Campo 'stato' mancante"})");
        }

        std::string nuovoStato = std::string(body["stato"].s());
        if (std::find(STATI_VALIDI.begin(), STATI_VALIDI.end(), nuovoStato) == STATI_VALIDI.end()) {
            return crow::response(400, R"({"errore":"Stato non valido"})");
        }

        if (!db.aggiornaStatoAssegnazione(id, idProgramma, nuovoStato)) {
            return crow::response(404, R"({"errore":"Assegnazione non trovata"})");
        }
        return crow::response(200, R"({"esito":"ok"})");
    });

    // POST /api/cliente/<id>/feedback  -> invia un feedback
    CROW_ROUTE(app, "/api/cliente/<int>/feedback").methods(crow::HTTPMethod::POST)
    ([&db](const crow::request& req, int id) {
        try {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, R"({"errore":"Body JSON non valido"})");

            int valutazione = body.has("valutazione") ? (int)body["valutazione"] : 0;
            std::string commento = body.has("commento") ? std::string(body["commento"]) : "";
            int idProgramma = body.has("id_programma") ? (int)body["id_programma"] : 0;
            int idPiano = body.has("id_piano") ? (int)body["id_piano"] : 0;

            if (valutazione < 1 || valutazione > 5) {
                return crow::response(400, R"({"errore":"Valutazione deve essere tra 1 e 5"})");
            }
            if (idProgramma == 0 && idPiano == 0) {
                return crow::response(400, R"({"errore":"Specificare id_programma o id_piano"})");
            }

            date today = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());

            Feedback f(0, valutazione, commento, today, id, idProgramma, idPiano);
            int nuovoId = db.inserisciFeedback(f);
            if (nuovoId <= 0) return crow::response(500, R"({"errore":"Errore durante il salvataggio"})");

            crow::json::wvalue w;
            w["id"] = nuovoId;
            return crow::response(201, w);
        } catch (const std::exception& e) {
            return crow::response(500, std::string(R"({"errore":")") + e.what() + R"("})");
        } catch (...) {
            return crow::response(500, R"({"errore":"Errore sconosciuto"})");
        }
    });

    // GET /api/cliente/<id>/feedback  -> tutti i feedback del cliente
    CROW_ROUTE(app, "/api/cliente/<int>/feedback")
    ([&db](int id) {
        auto programmi = db.getProgrammiByCliente(id);
        auto piani = db.getPianiByCliente(id);
        crow::json::wvalue w;
        std::vector<crow::json::wvalue> arr;

        for (const auto& pr : programmi) {
            auto feedbacks = db.getFeedbackByProgramma(pr.getId());
            for (const auto& fb : feedbacks) {
                if (fb.getIdUtente() != id) continue;
                crow::json::wvalue fj;
                fj["id"] = fb.getIdFeedback();
                fj["valutazione"] = fb.getValutazione();
                fj["commento"] = fb.getCommento();
                fj["data"] = fb.getDataStr();
                fj["id_programma"] = fb.getIdProgramma();
                fj["id_piano"] = fb.getIdPiano();
                arr.push_back(std::move(fj));
            }
        }
        for (const auto& p : piani) {
            auto feedbacks = db.getFeedbackByPiano(p.getId());
            for (const auto& fb : feedbacks) {
                if (fb.getIdUtente() != id) continue;
                crow::json::wvalue fj;
                fj["id"] = fb.getIdFeedback();
                fj["valutazione"] = fb.getValutazione();
                fj["commento"] = fb.getCommento();
                fj["data"] = fb.getDataStr();
                fj["id_programma"] = fb.getIdProgramma();
                fj["id_piano"] = fb.getIdPiano();
                arr.push_back(std::move(fj));
            }
        }
        w["feedback"] = std::move(arr);
        return crow::response(200, w);
    });

    // DELETE /api/cliente/<id>/feedback/<fid>  -> elimina un feedback proprio
    CROW_ROUTE(app, "/api/cliente/<int>/feedback/<int>").methods(crow::HTTPMethod::Delete)
    ([&db](int id, int fid) {
        if (db.eliminaFeedback(fid, id)) {
            return crow::response(200, R"({"esito":"ok"})");
        }
        return crow::response(404, R"({"errore":"Feedback non trovato o non autorizzato"})");
    });
}
