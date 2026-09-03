//
// Created by giorg on 23/08/2026.
//

#include "Cibo.h"
#include <utility>

Cibo::Cibo(int id_cibo, string nome, float kcal, float carboidrati, float proteine, float grassi)
    :id_cibo(id_cibo), nome(std::move(nome)), kcal(kcal), carboidrati(carboidrati), proteine(proteine), grassi(grassi){}