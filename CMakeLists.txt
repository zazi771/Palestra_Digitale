cmake_minimum_required(VERSION 3.17)
project(PALESTRA_DIGITALE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Aggiungi la directory corrente come percorso per gli header
include_directories(${CMAKE_SOURCE_DIR})

# Crea l'eseguibile con main.cpp e sqlite3.c
add_executable(PALESTRA_DIGITALE
    main.cpp
    sqlite3.c
)

# Linka le librerie necessarie per SQLite
find_package(Threads REQUIRED)
target_link_libraries(PALESTRA_DIGITALE PRIVATE Threads::Threads)

# Su macOS, aggiungi CoreFoundation
if (APPLE)
    target_link_libraries(PALESTRA_DIGITALE PRIVATE "-framework CoreFoundation")
endif()