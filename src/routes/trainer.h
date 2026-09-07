//
// Created by giorg on 04/09/2026.
//

#ifndef PALESTRADIGITALE_ROUTES_TRAINER_H
#define PALESTRADIGITALE_ROUTES_TRAINER_H

#include <crow.h>
#include "database/Database.h"

void registraTrainerRoutes(crow::SimpleApp& app, Database& db, const std::string& uploadsDir);

#endif //PALESTRADIGITALE_ROUTES_TRAINER_H
