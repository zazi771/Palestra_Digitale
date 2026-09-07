//
// Created by giorg on 04/09/2026.
//

#ifndef PALESTRADIGITALE_ROUTES_CLIENTE_H
#define PALESTRADIGITALE_ROUTES_CLIENTE_H

#include <crow.h>
#include "database/Database.h"

void registraClienteRoutes(crow::SimpleApp& app, Database& db);

#endif //PALESTRADIGITALE_ROUTES_CLIENTE_H
