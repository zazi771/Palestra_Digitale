
#ifndef PALESTRADIGITALE_ROUTES_AUTH_H
#define PALESTRADIGITALE_ROUTES_AUTH_H

#include <crow.h>
#include "database/Database.h"

void registraAuthRoutes(crow::SimpleApp& app, Database& db);

#endif //PALESTRADIGITALE_ROUTES_AUTH_H
