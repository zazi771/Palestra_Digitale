
#ifndef PALESTRADIGITALE_ROUTES_CLIENTE_H
#define PALESTRADIGITALE_ROUTES_CLIENTE_H

#include <crow.h>
#include "database/Database.h"

void registraClienteRoutes(crow::SimpleApp& app, Database& db);

#endif //PALESTRADIGITALE_ROUTES_CLIENTE_H
