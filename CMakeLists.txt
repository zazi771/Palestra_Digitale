cmake_minimum_required(VERSION 3.17)
project(PALESTRA_DIGITALE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/SQLiteCpp)

# Crea l'eseguibile con main.cpp e sqlite3.c
add_executable(PALESTRA_DIGITALE
    main.cpp
    Database.cpp
    Database.h
    models/Utente.cpp
    models/Utente.h
    models/Cartella_clinica.cpp
    models/Cartella_clinica.h
    models/Certificazione.cpp
    models/Certificazione.h
    models/Cibo.cpp
    models/Cibo.h
    models/Esercizio.cpp
    models/Esercizio.h
    models/Feedback.cpp
    models/Feedback.h
    models/Pasto.cpp
    models/Pasto.h
    models/Pasto_cibo.cpp
    models/Pasto_cibo.h
    models/Piano_alimentare.cpp
    models/Piano_alimentare.h
    models/Programma_allenamento.cpp
    models/Programma_allenamento.h
    models/Programma_esercizio.cpp
    models/Programma_esercizio.h
    models/Sessione.cpp
    models/Sessione.h
    models/Utente_programma.cpp
    models/Utente_programma.h
)

# Aggiungi la directory corrente come percorso per gli header
target_include_directories(PALESTRA_DIGITALE PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(PALESTRA_DIGITALE SQLiteCpp)

# Su macOS, aggiungi CoreFoundation
if (APPLE)
    target_link_libraries(PALESTRA_DIGITALE PRIVATE "-framework CoreFoundation")
endif()