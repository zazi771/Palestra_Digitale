//
// Created by giorg on 04/09/2026.
//

#ifndef PALESTRADIGITALE_ROUTES_UPLOADS_H
#define PALESTRADIGITALE_ROUTES_UPLOADS_H

#include <crow.h>
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cctype>

namespace uploads {

// Estrae il valore del parametro "filename" dalla parte multipart, se presente.
inline std::string filenameDa(const crow::multipart::part& parte) {
    auto it = parte.headers.find("Content-Disposition");
    if (it == parte.headers.end()) return "";
    auto params = it->second.params;
    auto f = params.find("filename");
    if (f == params.end()) return "";
    auto nome = f->second;
    // tiene solo la parte finale dopo eventuali separatori di percorso
    auto slash = nome.find_last_of("\\/");
    if (slash != std::string::npos) nome = nome.substr(slash + 1);
    return nome;
}

// Produce un nome file sicuro, eventualmente prefissato con timestamp per evitare collisioni.
inline std::string nomeSicuro(const std::string& nome) {
    std::string out;
    for (char c : nome) {
        char l = (char)std::tolower((unsigned char)c);
        if (std::isalnum(l) || c == '.' || c == '-' || c == '_') out += c;
    }
    if (out.empty()) out = "file";
    // estensione
    std::string estensione;
    auto dot = out.find_last_of('.');
    if (dot != std::string::npos && dot != out.size() - 1) {
        estensione = out.substr(dot);
        out = out.substr(0, dot);
    }
    // prefisso temporale per evitare sovrascritture
    auto ora = std::time(nullptr);
    std::ostringstream oss;
    oss << out << "_" << ora << estensione;
    return oss.str();
}

// Salva il contenuto binario di una parte multipart nella cartella uploads.
// Ritorna true e imposta nomeSalvato (nome base) in caso di successo.
inline bool salvaFile(const std::string& uploadsDir, const crow::multipart::part& parte, std::string& nomeSalvato) {
    nomeSalvato = nomeSicuro(filenameDa(parte));
    std::string percorso = uploadsDir + nomeSalvato;
    std::ofstream file(percorso, std::ios::binary);
    if (!file) return false;
    file.write(parte.body.data(), (std::streamsize)parte.body.size());
    file.close();
    return true;
}

}

#endif //PALESTRADIGITALE_ROUTES_UPLOADS_H
