cmake_minimum_required(VERSION 3.17)
project(PALESTRA_DIGITALE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/SQLiteCpp)

add_executable(PALESTRA_DIGITALE
    src/main.cpp
    src/database/Database.cpp
    src/database/Database.h
    src/models/Utente.cpp
    src/models/Utente.h
    src/models/Cartella_clinica.cpp
    src/models/Cartella_clinica.h
    src/models/Certificazione.cpp
    src/models/Certificazione.h
    src/models/Cibo.cpp
    src/models/Cibo.h
    src/models/Esercizio.cpp
    src/models/Esercizio.h
    src/models/Feedback.cpp
    src/models/Feedback.h
    src/models/Pasto.cpp
    src/models/Pasto.h
    src/models/Pasto_cibo.cpp
    src/models/Pasto_cibo.h
    src/models/Piano_alimentare.cpp
    src/models/Piano_alimentare.h
    src/models/Programma_allenamento.cpp
    src/models/Programma_allenamento.h
    src/models/Programma_esercizio.cpp
    src/models/Programma_esercizio.h
    src/models/Sessione.cpp
    src/models/Sessione.h
    src/models/Utente_programma.cpp
    src/models/Utente_programma.h
)

target_include_directories(PALESTRA_DIGITALE PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(PALESTRA_DIGITALE SQLiteCpp)

if (APPLE)
    target_link_libraries(PALESTRA_DIGITALE PRIVATE "-framework CoreFoundation")
endif()
