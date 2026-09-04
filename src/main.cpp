#include "database/Database.h"
#include "routes/auth.h"
#include "routes/cliente.h"
#include "routes/trainer.h"
#include "routes/nutrizionista.h"
#include <crow.h>
#include <iostream>
#include <windows.h>

namespace {

// Determina la cartella del progetto a partire dal path dell'eseguibile.
// L'eseguibile vive in <progetto>/cmake-build-debug/, quindi risaliamo di un livello.
std::string cartellaProgetto() {
    char buf[MAX_PATH];
    DWORD n = GetModuleFileNameA(nullptr, buf, MAX_PATH);
    std::string percorso(buf, n);
    auto slash = percorso.find_last_of("\\/");
    if (slash != std::string::npos) percorso = percorso.substr(0, slash);   // rimuove PALESTRA_DIGITALE.exe
    slash = percorso.find_last_of("\\/");
    if (slash != std::string::npos) percorso = percorso.substr(0, slash);   // risale da cmake-build-debug
    return percorso + "\\";
}

std::string PROGETTO;

// Cartella base del frontend
std::string BASE;

// Cartella dei file caricati (CV, certificazioni)
std::string UPLOADS;

crow::response serviFile(const std::string& relativo) {
    crow::response res(200);
    // Usiamo la variante "unsafe" (senza sanitizzazione del percorso): i percorsi sono
    // assoluti e hardcoded, e la sanitizzazione di Crow sostituirebbe i ':' della lettera di drive.
    res.set_static_file_info_unsafe(BASE + relativo);
    return res;
}

// Serve un file caricato dalla cartella uploads (ad es. /uploads/<nomefile>).
crow::response serviUpload(const std::string& nome) {
    // Cerca il nome nella cartella uploads; se per sicurezza il nome contiene
    // separatori di percorso lo ignoriamo e serviamo solo il nome base.
    auto base = nome;
    auto slash = base.find_last_of("\\/");
    if (slash != std::string::npos) base = base.substr(slash + 1);
    if (base.empty() || base == "." || base == "..") return crow::response(404);
    crow::response res(200);
    res.set_static_file_info_unsafe(UPLOADS + base);
    return res;
}

}

int main() {
    try {
        PROGETTO = cartellaProgetto();
        BASE = PROGETTO + "Frontend\\";
        UPLOADS = PROGETTO + "uploads\\";
        CreateDirectoryA(UPLOADS.c_str(), nullptr);

        // Il percorso del DB è risolto rispetto alla cartella del progetto
        Database db(PROGETTO + "PalestraDigitale.db");

        crow::SimpleApp app;

        // ---- File statici (Frontend) ----
        CROW_ROUTE(app, "/")([] { return serviFile("home.html"); });
        CROW_ROUTE(app, "/home.html")([] { return serviFile("home.html"); });
        CROW_ROUTE(app, "/home.css")([] { return serviFile("home.css"); });
        CROW_ROUTE(app, "/styleHome.css")([] { return serviFile("home.css"); });
        CROW_ROUTE(app, "/home.js")([] { return serviFile("home.js"); });

        // Area cliente
        CROW_ROUTE(app, "/cliente/area_cliente.html")([] { return serviFile("cliente/area_cliente.html"); });
        CROW_ROUTE(app, "/cliente/area_cliente.js")([] { return serviFile("cliente/area_cliente.js"); });
        CROW_ROUTE(app, "/cliente/area_cliente.css")([] { return serviFile("cliente/area_cliente.css"); });
        CROW_ROUTE(app, "/styleAreaCliente.css")([] { return serviFile("cliente/area_cliente.css"); });

        // Area trainer
        CROW_ROUTE(app, "/trainer/area_trainer.html")([] { return serviFile("trainer/area_trainer.html"); });
        CROW_ROUTE(app, "/trainer/area_trainer.js")([] { return serviFile("trainer/area_trainer.js"); });
        CROW_ROUTE(app, "/trainer/area_trainer.css")([] { return serviFile("trainer/area_trainer.css"); });
        CROW_ROUTE(app, "/styleTrainer.css")([] { return serviFile("trainer/area_trainer.css"); });

        // Area nutrizionista
        CROW_ROUTE(app, "/nutrizionista/area_nutrizionista.html")([] { return serviFile("nutrizionista/area_nutrizionista.html"); });
        CROW_ROUTE(app, "/nutrizionista/area_nutrizionista.js")([] { return serviFile("nutrizionista/area_nutrizionista.js"); });
        CROW_ROUTE(app, "/nutrizionista/area_nutrizionista.css")([] { return serviFile("nutrizionista/area_nutrizionista.css"); });
        CROW_ROUTE(app, "/styleAreaNutrizionista.css")([] { return serviFile("nutrizionista/area_nutrizionista.css"); });

        // Assets
        CROW_ROUTE(app, "/assets/logo.jpg")([] { return serviFile("assets/logo.jpg"); });
        CROW_ROUTE(app, "/img/logo.jpg")([] { return serviFile("assets/logo.jpg"); });

        // File caricati (CV, certificazioni)
        CROW_ROUTE(app, "/uploads/<string>")([](const std::string& nome) { return serviUpload(nome); });

        // ---- API ----
        registraAuthRoutes(app, db);
        registraClienteRoutes(app, db);
        registraTrainerRoutes(app, db, UPLOADS);
        registraNutrizionistaRoutes(app, db, UPLOADS);

        std::cout << "Server avviato su http://localhost:18080" << std::endl;
        std::cout << "Cartella progetto: " << PROGETTO << std::endl;
        app.port(18080).multithreaded().run();

    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
