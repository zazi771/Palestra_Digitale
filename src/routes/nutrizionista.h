
#ifndef PALESTRADIGITALE_ROUTES_NUTRIZIONISTA_H
#define PALESTRADIGITALE_ROUTES_NUTRIZIONISTA_H

#include <crow.h>
#include "database/Database.h"

void registraNutrizionistaRoutes(crow::SimpleApp& app, Database& db, const std::string& uploadsDir);

#endif //PALESTRADIGITALE_ROUTES_NUTRIZIONISTA_H
