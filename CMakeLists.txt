cmake_minimum_required(VERSION 3.17)
project(PALESTRA_DIGITALE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eseguibile autocontenuto: collega staticamente il runtime MinGW (g++ e libstdc++).
# Nota: libwinpthread-1.dll resta dinamica; MinGW\bin deve essere nel PATH (CLion lo fa già).
if (MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
endif()

add_subdirectory(libs/SQLiteCpp)

# ---------------------------------------------------------------------------
# Crow (HTTP micro-framework) — header-only
# Dipende da asio standalone (header-only), incluso nella cartella libs/
# ---------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/Crow/cmake)

# Indirizza find_path del modulo Findasio.cmake verso la copia locale di asio
set(ASIO_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/asio/include CACHE PATH "Percorso include di asio" FORCE)
set(ASIO_FOUND TRUE CACHE BOOL "asio trovato" FORCE)
set(asio_VERSION "1.30.0" CACHE STRING "Versione di asio" FORCE)

set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CROW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/Crow)

add_executable(PALESTRA_DIGITALE
    src/main.cpp
    src/database/Database.cpp
    src/database/Database.h
    src/routes/auth.cpp
    src/routes/auth.h
    src/routes/cliente.cpp
    src/routes/cliente.h
    src/routes/trainer.cpp
    src/routes/trainer.h
    src/routes/nutrizionista.cpp
    src/routes/nutrizionista.h
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

target_link_libraries(PALESTRA_DIGITALE PRIVATE SQLiteCpp Crow::Crow)

if (WIN32)
    target_link_libraries(PALESTRA_DIGITALE PRIVATE ws2_32 mswsock)
endif()

if (APPLE)
    target_link_libraries(PALESTRA_DIGITALE PRIVATE "-framework CoreFoundation")
endif()
