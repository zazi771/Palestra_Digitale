#include "database/Database.h"
#include "routes/auth.h"
#include "routes/cliente.h"
#include "routes/trainer.h"
#include "routes/nutrizionista.h"
#include <crow.h>
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <algorithm>

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

// Serve qualunque file dentro Frontend/, validando che il path richiesto
// non esca dalla cartella (protezione da path traversal, es. "../../PalestraDigitale.db").
crow::response serviFileGenerico(std::string richiesto) {
    // Rifiuta subito pattern sospetti prima ancora di toccare il filesystem
    if (richiesto.empty() || richiesto.find("..") != std::string::npos) {
        return crow::response(400);
    }

    // Normalizza eventuali backslash (nel caso arrivino da client strani)
    std::replace(richiesto.begin(), richiesto.end(), '/', '\\');

    std::filesystem::path base(BASE);
    std::filesystem::path target = base / richiesto;

    std::error_code ec;
    auto baseCanon = std::filesystem::weakly_canonical(base, ec);
    auto targetCanon = std::filesystem::weakly_canonical(target, ec);
    if (ec) return crow::response(404);

    // Controllo finale: il path risolto deve stare DENTRO la cartella Frontend
    auto baseStr = baseCanon.string();
    auto targetStr = targetCanon.string();
    if (targetStr.size() < baseStr.size() ||
        targetStr.compare(0, baseStr.size(), baseStr) != 0) {
        return crow::response(403);
    }

    if (!std::filesystem::exists(targetCanon) || std::filesystem::is_directory(targetCanon)) {
        return crow::response(404);
    }

    crow::response res(200);
    res.set_static_file_info_unsafe(targetStr);
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
        // Home page di default
        CROW_ROUTE(app, "/")([] { return serviFile("home.html"); });

        // File caricati (CV, certificazioni) — cartella separata da Frontend/
        CROW_ROUTE(app, "/uploads/<string>")([](const std::string& nome) { return serviUpload(nome); });

        // ---- API ----
        registraAuthRoutes(app, db);
        registraClienteRoutes(app, db);
        registraTrainerRoutes(app, db, UPLOADS);
        registraNutrizionistaRoutes(app, db, UPLOADS);

        // Route generica per QUALSIASI altro file dentro Frontend/
        // (home.html, home.css, home.js, cliente/area_cliente.css, assets/logo.jpg, ecc.)
        // Registrata per ultima: Crow dà priorità ai segmenti letterali (es. /uploads/...)
        // rispetto al parametro <path>, ma è buona norma tenerla in fondo al file.
        CROW_ROUTE(app, "/<path>")
        ([](const std::string& path) { return serviFileGenerico(path); });

        std::cout << "Server avviato su http://localhost:18080" << std::endl;
        std::cout << "Cartella progetto: " << PROGETTO << std::endl;
        app.port(18080).multithreaded().run();

    } catch (const std::exception& e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
