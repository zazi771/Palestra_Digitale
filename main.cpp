#include <iostream>
#include <sqlite3.h>
#include <string>

// ============================================================
// 1. Funzione per inizializzare il database all'avvio
// ============================================================
bool initDatabase(sqlite3*& db, const std::string& dbPath = "palestra.db") {
    // Apri (o crea) il file del database
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Errore apertura database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Lo script SQL con TUTTE le tue tabelle
    const char* sqlScript = R"sql(
        PRAGMA foreign_keys = ON;

        CREATE TABLE IF NOT EXISTS utenti (
            id_utente       INTEGER PRIMARY KEY AUTOINCREMENT,
            email           TEXT NOT NULL UNIQUE,
            password_hash   TEXT NOT NULL,
            nome            TEXT NOT NULL,
            cognome         TEXT NOT NULL,
            ruolo           TEXT NOT NULL CHECK (ruolo IN ('cliente','trainer','nutrizionista')),
            data_registrazione TEXT NOT NULL DEFAULT (datetime('now'))
        );

        CREATE TABLE IF NOT EXISTS esercizi (
            id_esercizio    INTEGER PRIMARY KEY AUTOINCREMENT,
            nome            TEXT NOT NULL,
            descrizione     TEXT,
            gruppo_muscolare TEXT,
            url_video       TEXT
        );

        CREATE TABLE IF NOT EXISTS programmi_allenamento (
            id_programma    INTEGER PRIMARY KEY AUTOINCREMENT,
            id_trainer      INTEGER NOT NULL,
            nome            TEXT NOT NULL,
            obiettivo       TEXT,
            livello_difficolta TEXT CHECK (livello_difficolta IN ('principiante','intermedio','avanzato')),
            durata_settimane INTEGER,
            descrizione     TEXT,
            FOREIGN KEY (id_trainer) REFERENCES utenti(id_utente)
        );

        CREATE TABLE IF NOT EXISTS programma_esercizio (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            id_programma    INTEGER NOT NULL,
            id_esercizio    INTEGER NOT NULL,
            ordine_giorno   INTEGER NOT NULL,
            serie           INTEGER NOT NULL,
            ripetizioni     TEXT NOT NULL,
            recupero_secondi INTEGER,
            FOREIGN KEY (id_programma) REFERENCES programmi_allenamento(id_programma) ON DELETE CASCADE,
            FOREIGN KEY (id_esercizio) REFERENCES esercizi(id_esercizio) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS cibi (
            id_cibo         INTEGER PRIMARY KEY AUTOINCREMENT,
            nome            TEXT NOT NULL,
            kcal            REAL,
            proteine        REAL,
            carboidrati     REAL,
            grassi          REAL
        );

        CREATE TABLE IF NOT EXISTS piani_alimentari (
            id_piano        INTEGER PRIMARY KEY AUTOINCREMENT,
            id_nutrizionista INTEGER NOT NULL,
            nome            TEXT NOT NULL,
            descrizione     TEXT,
            FOREIGN KEY (id_nutrizionista) REFERENCES utenti(id_utente)
        );

        CREATE TABLE IF NOT EXISTS piano_cibo (
            id              INTEGER PRIMARY KEY AUTOINCREMENT,
            id_piano        INTEGER NOT NULL,
            id_cibo         INTEGER NOT NULL,
            giorno_settimana INTEGER NOT NULL CHECK (giorno_settimana BETWEEN 1 AND 7),
            tipo_pasto      TEXT NOT NULL CHECK (tipo_pasto IN ('colazione','spuntino_mattina','pranzo','spuntino_pomeriggio','cena')),
            quantita_grammi INTEGER NOT NULL,
            FOREIGN KEY (id_piano) REFERENCES piani_alimentari(id_piano) ON DELETE CASCADE,
            FOREIGN KEY (id_cibo) REFERENCES cibi(id_cibo) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS utente_programma (
            id_assegnazione INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utente       INTEGER NOT NULL,
            id_programma    INTEGER NOT NULL,
            data_inizio     TEXT NOT NULL DEFAULT (date('now')),
            stato           TEXT CHECK (stato IN ('attivo','completato','abbandonato')) DEFAULT 'attivo',
            FOREIGN KEY (id_utente) REFERENCES utenti(id_utente) ON DELETE CASCADE,
            FOREIGN KEY (id_programma) REFERENCES programmi_allenamento(id_programma) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS utente_piano (
            id_assegnazione INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utente       INTEGER NOT NULL,
            id_piano        INTEGER NOT NULL,
            data_inizio     TEXT NOT NULL DEFAULT (date('now')),
            stato           TEXT CHECK (stato IN ('attivo','completato','abbandonato')) DEFAULT 'attivo',
            FOREIGN KEY (id_utente) REFERENCES utenti(id_utente) ON DELETE CASCADE,
            FOREIGN KEY (id_piano) REFERENCES piani_alimentari(id_piano) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS sessioni_svolte (
            id_sessione     INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utente       INTEGER NOT NULL,
            id_programma    INTEGER NOT NULL,
            data            TEXT NOT NULL DEFAULT (date('now')),
            tempo_dedicato_minuti INTEGER,
            completato      INTEGER NOT NULL DEFAULT 0 CHECK (completato IN (0,1)),
            FOREIGN KEY (id_utente) REFERENCES utenti(id_utente) ON DELETE CASCADE,
            FOREIGN KEY (id_programma) REFERENCES programmi_allenamento(id_programma) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS feedback (
            id_feedback     INTEGER PRIMARY KEY AUTOINCREMENT,
            id_utente       INTEGER NOT NULL,
            id_programma    INTEGER,
            id_piano        INTEGER,
            valutazione     INTEGER NOT NULL CHECK (valutazione BETWEEN 1 AND 5),
            commento        TEXT,
            data            TEXT NOT NULL DEFAULT (date('now')),
            FOREIGN KEY (id_utente) REFERENCES utenti(id_utente) ON DELETE CASCADE,
            FOREIGN KEY (id_programma) REFERENCES programmi_allenamento(id_programma) ON DELETE CASCADE,
            FOREIGN KEY (id_piano) REFERENCES piani_alimentari(id_piano) ON DELETE CASCADE,
            CHECK ((id_programma IS NOT NULL AND id_piano IS NULL) OR
                   (id_programma IS NULL AND id_piano IS NOT NULL))
        );
    )sql";

    char* errMsg = nullptr;
    rc = sqlite3_exec(db, sqlScript, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Errore creazione schema: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    std::cout << "Database inizializzato correttamente" << std::endl;
    return true;
}

// ============================================================
// 2. Funzione per inserire dati di test (se le tabelle sono vuote)
// ============================================================
bool popolaDatiDiTest(sqlite3* db) {
    // Controlla se la tabella utenti è già popolata
    int count = 0;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM utenti;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            count = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    if (count > 0) {
        std::cout << "Dati già presenti. Salto l'inserimento." << std::endl;
        return true;
    }

    const char* insertData = R"sql(
        -- ============================================================
        -- 1. UTENTI (3 trainer, 2 nutrizionisti, 6 clienti)
        -- ============================================================
        INSERT INTO utenti (email, password_hash, nome, cognome, ruolo) VALUES
        ('marco.bianchi@palestra.it', 'hash_trainer1', 'Marco', 'Bianchi', 'trainer'),
        ('elena.russo@palestra.it', 'hash_trainer2', 'Elena', 'Russo', 'trainer'),
        ('paolo.ferrari@palestra.it', 'hash_trainer3', 'Paolo', 'Ferrari', 'trainer'),
        ('giulia.neri@palestra.it', 'hash_nutri1', 'Giulia', 'Neri', 'nutrizionista'),
        ('sara.moretti@palestra.it', 'hash_nutri2', 'Sara', 'Moretti', 'nutrizionista'),
        ('luca.gialli@email.it', 'hash_cliente1', 'Luca', 'Gialli', 'cliente'),
        ('francesca.verdi@email.it', 'hash_cliente2', 'Francesca', 'Verdi', 'cliente'),
        ('andrea.romano@email.it', 'hash_cliente3', 'Andrea', 'Romano', 'cliente'),
        ('valentina.deluca@email.it', 'hash_cliente4', 'Valentina', 'De Luca', 'cliente'),
        ('roberto.conti@email.it', 'hash_cliente5', 'Roberto', 'Conti', 'cliente'),
        ('martina.leone@email.it', 'hash_cliente6', 'Martina', 'Leone', 'cliente');

        -- ============================================================
        -- 2. ESERCIZI A CORPO LIBERO (per allenarsi a casa)
        -- ============================================================
        INSERT INTO esercizi (nome, descrizione, gruppo_muscolare, url_video) VALUES
        ('Squat', 'Piegamento delle gambe mantenendo la schiena dritta. Ottimo per gambe e glutei.', 'Gambe', 'https://youtu.be/example1'),
        ('Affondo', 'Passo in avanti e piegamento delle gambe a 90 gradi. Lavora quadricipiti e glutei.', 'Gambe', 'https://youtu.be/example2'),
        ('Ponte glutei', 'Disteso a terra, solleva il bacino contraendo i glutei.', 'Glutei', 'https://youtu.be/example3'),
        ('Push-up classico', 'Flessioni sulle braccia. Lavora petto, spalle e tricipiti.', 'Petto', 'https://youtu.be/example4'),
        ('Push-up diamante', 'Flessioni con le mani a forma di diamante. Maggiore focus su tricipiti.', 'Tricipiti', 'https://youtu.be/example5'),
        ('Plank', 'Posizione a tavola, mantieni il corpo in linea retta. Rafforza il core.', 'Addominali', 'https://youtu.be/example6'),
        ('Mountain climber', 'Da posizione plank, porta alternativamente le ginocchia al petto.', 'Cardio', 'https://youtu.be/example7'),
        ('Crunch', 'Sollevamento del busto da terra per contrarre gli addominali.', 'Addominali', 'https://youtu.be/example8'),
        ('Russian twist', 'Seduto, ruota il busto lateralmente con le gambe sollevate.', 'Addominali', 'https://youtu.be/example9'),
        ('Superman', 'Disteso a pancia in giù, solleva braccia e gambe simultaneamente.', 'Schiena', 'https://youtu.be/example10'),
        ('Bird dog', 'Da posizione a quattro zampe, estendi alternativamente braccio e gamba opposti.', 'Schiena', 'https://youtu.be/example11'),
        ('Jumping jack', 'Salto con apertura e chiusura di braccia e gambe. Ottimo per riscaldamento.', 'Cardio', 'https://youtu.be/example12'),
        ('Burpee', 'Da posizione eretta, accovacciati, salta in plank, esegui un push-up, e risali con un salto.', 'Corpo libero', 'https://youtu.be/example13'),
        ('Wall sit', 'Posizione seduta immaginaria contro il muro, mantieni le gambe a 90 gradi.', 'Gambe', 'https://youtu.be/example14'),
        ('Triceps dip', 'Su una sedia, abbassa il corpo flettendo i gomiti. Lavora i tricipiti.', 'Tricipiti', 'https://youtu.be/example15');

        -- ============================================================
        -- 3. PROGRAMMI DI ALLENAMENTO (creati dai trainer)
        -- ============================================================
        INSERT INTO programmi_allenamento (id_trainer, nome, obiettivo, livello_difficolta, durata_settimane, descrizione) VALUES
        (1, 'Forza Base', 'Aumento della forza e tonificazione', 'principiante', 8, 'Programma completo per chi inizia, con esercizi fondamentali a corpo libero'),
        (1, 'Resistenza Avanzata', 'Miglioramento della resistenza muscolare', 'avanzato', 12, 'Programma intenso con alte ripetizioni e poco recupero'),
        (2, 'Dimagrimento Veloce', 'Perdita di peso e definizione', 'intermedio', 6, 'Circuito ad alta intensità per bruciare grassi e tonificare'),
        (2, 'Core Power', 'Rafforzamento del centro del corpo', 'intermedio', 8, 'Focus su addominali, lombari e stabilità'),
        (3, 'Full Body a Casa', 'Allenamento completo senza attrezzi', 'principiante', 10, 'Tutti i gruppi muscolari allenati con il peso del corpo'),
        (3, 'Definizione Muscolare', 'Perdita di grasso e definizione', 'avanzato', 8, 'Allenamento ad alta intensità con esercizi composti');

        -- ============================================================
        -- 4. DETTAGLIO PROGRAMMA → ESERCIZI (serie, ripetizioni, giorni)
        -- ============================================================
        -- Programma 1: Forza Base (id=1) - trainer Marco
        INSERT INTO programma_esercizio (id_programma, id_esercizio, ordine_giorno, serie, ripetizioni, recupero_secondi) VALUES
        (1, 1, 1, 4, '12', 90),
        (1, 4, 1, 3, '10', 90),
        (1, 6, 1, 3, '45 secondi', 60),
        (1, 2, 2, 3, '10 per gamba', 90),
        (1, 5, 2, 3, '8', 90),
        (1, 8, 2, 3, '15', 60),
        (1, 10, 3, 3, '12', 60),
        (1, 12, 3, 3, '60 secondi', 45);

        -- Programma 2: Resistenza Avanzata (id=2) - trainer Marco
        INSERT INTO programma_esercizio (id_programma, id_esercizio, ordine_giorno, serie, ripetizioni, recupero_secondi) VALUES
        (2, 13, 1, 5, '8', 60),
        (2, 7, 1, 4, '20 per gamba', 45),
        (2, 1, 1, 4, '20', 60),
        (2, 4, 2, 5, '15', 60),
        (2, 11, 2, 4, '12 per lato', 45),
        (2, 9, 2, 4, '20 per lato', 45);

        -- Programma 3: Dimagrimento Veloce (id=3) - trainer Elena
        INSERT INTO programma_esercizio (id_programma, id_esercizio, ordine_giorno, serie, ripetizioni, recupero_secondi) VALUES
        (3, 12, 1, 4, '45 secondi', 30),
        (3, 7, 1, 4, '30 secondi', 30),
        (3, 4, 1, 3, '12', 30),
        (3, 1, 2, 4, '15', 45),
        (3, 13, 2, 3, '6', 45),
        (3, 6, 2, 4, '45 secondi', 30);

        -- Programma 4: Core Power (id=4) - trainer Elena
        INSERT INTO programma_esercizio (id_programma, id_esercizio, ordine_giorno, serie, ripetizioni, recupero_secondi) VALUES
        (4, 6, 1, 4, '60 secondi', 45),
        (4, 8, 1, 4, '20', 45),
        (4, 9, 1, 4, '15 per lato', 45),
        (4, 11, 2, 3, '15 per lato', 60),
        (4, 10, 2, 4, '15', 60),
        (4, 14, 2, 3, '45 secondi', 60);

        -- Programma 5: Full Body a Casa (id=5) - trainer Paolo
        INSERT INTO programma_esercizio (id_programma, id_esercizio, ordine_giorno, serie, ripetizioni, recupero_secondi) VALUES
        (5, 1, 1, 3, '12', 90),
        (5, 4, 1, 3, '8', 90),
        (5, 6, 1, 3, '30 secondi', 60),
        (5, 2, 2, 3, '8 per gamba', 90),
        (5, 5, 2, 3, '6', 90),
        (5, 8, 2, 3, '12', 60),
        (5, 10, 3, 3, '10', 60),
        (5, 12, 3, 3, '45 secondi', 45);

        -- Programma 6: Definizione Muscolare (id=6) - trainer Paolo
        INSERT INTO programma_esercizio (id_programma, id_esercizio, ordine_giorno, serie, ripetizioni, recupero_secondi) VALUES
        (6, 13, 1, 4, '10', 60),
        (6, 1, 1, 4, '20', 60),
        (6, 15, 1, 4, '12', 60),
        (6, 4, 2, 5, '15', 45),
        (6, 7, 2, 4, '20 per gamba', 45),
        (6, 14, 2, 3, '60 secondi', 45);

        -- ============================================================
        -- 5. CIBI (con valori nutrizionali reali)
        -- ============================================================
        INSERT INTO cibi (nome, kcal, proteine, carboidrati, grassi) VALUES
        ('Petto di pollo', 165, 31, 0, 3.6),
        ('Salmone', 208, 22, 0, 13),
        ('Uovo intero', 155, 13, 1.1, 11),
        ('Riso basmati', 130, 2.7, 28, 0.3),
        ('Pasta integrale', 124, 5, 25, 1),
        ('Quinoa', 120, 4.4, 21, 1.9),
        ('Avocado', 160, 2, 8.5, 14.7),
        ('Broccoli', 34, 2.8, 7, 0.4),
        ('Spinaci', 23, 2.9, 3.6, 0.4),
        ('Yogurt greco', 100, 10, 4, 5),
        ('Frutta mista', 80, 1, 20, 0.3),
        ('Mandorle', 579, 21, 22, 50),
        ('Pane integrale', 247, 8, 49, 2.5),
        ('Tacchino', 135, 29, 0, 2),
        ('Ricotta light', 138, 11, 3, 8),
        ('Ceci', 139, 8.9, 27, 2.6),
        ('Semi di chia', 486, 17, 42, 31),
        ('Fiocchi d''avena', 389, 16.9, 66, 6.9);

        -- ============================================================
        -- 6. PIANI ALIMENTARI (creati dai nutrizionisti)
        -- ============================================================
        INSERT INTO piani_alimentari (id_nutrizionista, nome, descrizione) VALUES
        (4, 'Dieta Iperproteica', 'Piano per aumento massa muscolare con alto apporto proteico. Ideale per chi si allena con i pesi.'),
        (4, 'Alimentazione Bilanciata', 'Piano per mantenimento del peso con nutrienti distribuiti uniformemente.'),
        (5, 'Dieta Chetogenica', 'Piano a basso contenuto di carboidrati per dimagrimento. Adatto per chi ha problemi di insulina.'),
        (5, 'Dieta Vegetariana', 'Piano senza carne ma ricco di proteine vegetali. Adatto a chi segue uno stile di vita vegetariano.');

        -- ============================================================
        -- 7. DETTAGLIO PIANO → CIBI (giorno, pasto, quantità)
        -- ============================================================
        -- Piano 1: Iperproteica (id=1)
        INSERT INTO piano_cibo (id_piano, id_cibo, giorno_settimana, tipo_pasto, quantita_grammi) VALUES
        (1, 1, 1, 'pranzo', 200),
        (1, 4, 1, 'pranzo', 150),
        (1, 8, 1, 'pranzo', 200),
        (1, 3, 2, 'colazione', 3),
        (1, 10, 2, 'colazione', 200),
        (1, 1, 2, 'pranzo', 200),
        (1, 6, 2, 'pranzo', 150),
        (1, 2, 3, 'cena', 150),
        (1, 7, 3, 'cena', 100),
        (1, 11, 3, 'spuntino_pomeriggio', 150);

        -- Piano 2: Bilanciata (id=2)
        INSERT INTO piano_cibo (id_piano, id_cibo, giorno_settimana, tipo_pasto, quantita_grammi) VALUES
        (2, 14, 1, 'pranzo', 180),
        (2, 5, 1, 'pranzo', 150),
        (2, 9, 1, 'pranzo', 200),
        (2, 10, 1, 'colazione', 200),
        (2, 13, 1, 'colazione', 60),
        (2, 14, 2, 'pranzo', 180),
        (2, 6, 2, 'pranzo', 150),
        (2, 8, 2, 'cena', 250),
        (2, 18, 3, 'colazione', 80),
        (2, 11, 3, 'spuntino_pomeriggio', 150);

        -- Piano 3: Chetogenica (id=3)
        INSERT INTO piano_cibo (id_piano, id_cibo, giorno_settimana, tipo_pasto, quantita_grammi) VALUES
        (3, 2, 1, 'pranzo', 180),
        (3, 7, 1, 'pranzo', 100),
        (3, 9, 1, 'pranzo', 200),
        (3, 3, 1, 'colazione', 3),
        (3, 15, 1, 'colazione', 200),
        (3, 2, 2, 'cena', 200),
        (3, 7, 2, 'cena', 120),
        (3, 1, 3, 'pranzo', 200),
        (3, 12, 3, 'spuntino_pomeriggio', 30);

        -- Piano 4: Vegetariana (id=4)
        INSERT INTO piano_cibo (id_piano, id_cibo, giorno_settimana, tipo_pasto, quantita_grammi) VALUES
        (4, 16, 1, 'pranzo', 200),
        (4, 6, 1, 'pranzo', 180),
        (4, 8, 1, 'pranzo', 250),
        (4, 10, 1, 'colazione', 200),
        (4, 18, 1, 'colazione', 60),
        (4, 16, 2, 'pranzo', 200),
        (4, 5, 2, 'pranzo', 150),
        (4, 9, 2, 'cena', 200),
        (4, 11, 3, 'spuntino_pomeriggio', 150),
        (4, 17, 3, 'colazione', 20);

        -- ============================================================
        -- 8. ASSEGNAZIONI CLIENTE → PROGRAMMA (con stati diversi)
        -- ============================================================
        INSERT INTO utente_programma (id_utente, id_programma, data_inizio, stato) VALUES
        -- Cliente Luca (6) segue 2 programmi
        (6, 1, '2025-06-01', 'completato'),
        (6, 3, '2025-07-15', 'attivo'),
        -- Cliente Francesca (7) segue 2 programmi
        (7, 4, '2025-07-01', 'attivo'),
        (7, 6, '2025-08-01', 'attivo'),
        -- Cliente Andrea (8) segue 3 programmi
        (8, 2, '2025-01-15', 'completato'),
        (8, 3, '2025-03-15', 'completato'),
        (8, 5, '2025-07-01', 'attivo'),
        -- Cliente Valentina (9) segue 1 programma
        (9, 5, '2025-06-01', 'attivo'),
        -- Cliente Roberto (10) segue 1 programma
        (10, 1, '2025-05-15', 'abbandonato'),
        (10, 4, '2025-06-20', 'attivo'),
        -- Cliente Martina (11) segue 2 programmi
        (11, 2, '2025-04-01', 'completato'),
        (11, 6, '2025-06-15', 'attivo');

        -- ============================================================
        -- 9. ASSEGNAZIONI CLIENTE → PIANO ALIMENTARE
        -- ============================================================
        INSERT INTO utente_piano (id_utente, id_piano, data_inizio, stato) VALUES
        -- Luca (6)
        (6, 1, '2025-06-01', 'completato'),
        (6, 2, '2025-07-15', 'attivo'),
        -- Francesca (7)
        (7, 2, '2025-07-01', 'attivo'),
        -- Andrea (8)
        (8, 1, '2025-01-15', 'completato'),
        (8, 3, '2025-03-15', 'completato'),
        (8, 4, '2025-07-01', 'attivo'),
        -- Valentina (9)
        (9, 4, '2025-06-01', 'attivo'),
        -- Roberto (10)
        (10, 1, '2025-05-15', 'abbandonato'),
        (10, 2, '2025-06-20', 'attivo'),
        -- Martina (11)
        (11, 3, '2025-04-01', 'completato'),
        (11, 1, '2025-06-15', 'attivo');

        -- ============================================================
        -- 10. SESSIONI SVOLTE (storico distribuito nel tempo)
        -- ============================================================
        -- Luca (6) - Programma 1, poi 3
        INSERT INTO sessioni_svolte (id_utente, id_programma, data, tempo_dedicato_minuti, completato) VALUES
        (6, 1, '2025-06-02', 45, 1),
        (6, 1, '2025-06-03', 50, 1),
        (6, 1, '2025-06-04', 40, 1),
        (6, 1, '2025-06-05', 55, 1),
        (6, 1, '2025-06-08', 48, 1),
        (6, 1, '2025-06-10', 42, 1),
        (6, 3, '2025-07-16', 35, 0),
        (6, 3, '2025-07-18', 38, 1),
        (6, 3, '2025-07-20', 40, 1);

        -- Francesca (7) - Programma 4 e 6
        INSERT INTO sessioni_svolte (id_utente, id_programma, data, tempo_dedicato_minuti, completato) VALUES
        (7, 4, '2025-07-02', 30, 1),
        (7, 4, '2025-07-04', 35, 1),
        (7, 4, '2025-07-06', 28, 1),
        (7, 4, '2025-07-08', 32, 1),
        (7, 6, '2025-08-02', 45, 1),
        (7, 6, '2025-08-04', 48, 1),
        (7, 6, '2025-08-06', 42, 0);

        -- Andrea (8) - Programma 2, 3 e 5
        INSERT INTO sessioni_svolte (id_utente, id_programma, data, tempo_dedicato_minuti, completato) VALUES
        (8, 2, '2025-01-16', 60, 1),
        (8, 2, '2025-01-18', 65, 1),
        (8, 2, '2025-01-20', 55, 1),
        (8, 2, '2025-01-22', 70, 1),
        (8, 2, '2025-01-25', 58, 1),
        (8, 2, '2025-01-28', 62, 1),
        (8, 3, '2025-03-16', 45, 1),
        (8, 3, '2025-03-18', 42, 1),
        (8, 3, '2025-03-20', 40, 1),
        (8, 3, '2025-03-22', 48, 1),
        (8, 5, '2025-07-02', 35, 1),
        (8, 5, '2025-07-04', 38, 1),
        (8, 5, '2025-07-06', 32, 0);

        -- Valentina (9) - Programma 5
        INSERT INTO sessioni_svolte (id_utente, id_programma, data, tempo_dedicato_minuti, completato) VALUES
        (9, 5, '2025-06-02', 40, 1),
        (9, 5, '2025-06-04', 45, 1),
        (9, 5, '2025-06-06', 38, 1),
        (9, 5, '2025-06-08', 42, 1),
        (9, 5, '2025-06-10', 35, 1);

        -- Roberto (10) - Programma 1 (abbandonato) e 4 (attivo)
        INSERT INTO sessioni_svolte (id_utente, id_programma, data, tempo_dedicato_minuti, completato) VALUES
        (10, 1, '2025-05-16', 40, 1),
        (10, 1, '2025-05-18', 35, 1),
        (10, 1, '2025-05-20', 30, 0),
        (10, 4, '2025-06-22', 35, 1),
        (10, 4, '2025-06-25', 40, 1),
        (10, 4, '2025-06-28', 32, 1);

        -- Martina (11) - Programma 2 e 6
        INSERT INTO sessioni_svolte (id_utente, id_programma, data, tempo_dedicato_minuti, completato) VALUES
        (11, 2, '2025-04-02', 55, 1),
        (11, 2, '2025-04-04', 60, 1),
        (11, 2, '2025-04-06', 50, 1),
        (11, 2, '2025-04-08', 55, 1),
        (11, 2, '2025-04-10', 58, 1),
        (11, 6, '2025-06-16', 45, 1),
        (11, 6, '2025-06-18', 48, 1),
        (11, 6, '2025-06-20', 42, 1),
        (11, 6, '2025-06-22', 50, 1);

        -- ============================================================
        -- 11. FEEDBACK (recensioni realistiche)
        -- ============================================================
        INSERT INTO feedback (id_utente, id_programma, id_piano, valutazione, commento, data) VALUES
        -- Luca (6) → Programma 1 (5 stelle)
        (6, 1, NULL, 5, 'Programma fantastico! Ho già visto progressi dopo solo un mese. Le gambe sono più toniche e ho molta più energia.', '2025-06-30'),
        -- Luca (6) → Piano 1 (4 stelle)
        (6, NULL, 1, 4, 'La dieta iperproteica funziona bene, ma all''inizio ho faticato ad abituarmi alle quantità. Ora mi sento più forte.', '2025-06-30'),
        -- Francesca (7) → Programma 4 (5 stelle)
        (7, 4, NULL, 5, 'Finalmente un programma che mette al centro il core! Dopo 3 settimane ho già meno mal di schiena. Consigliatissimo!', '2025-07-25'),
        -- Andrea (8) → Programma 2 (4 stelle)
        (8, 2, NULL, 4, 'Programma tosto ma efficace. Le ripetizioni alte mettono a dura prova la resistenza. Unico difetto: mancano esercizi per le braccia.', '2025-02-28'),
        -- Andrea (8) → Programma 3 (5 stelle)
        (8, 3, NULL, 5, 'Il circuito ad alta intensità è stato perfetto per perdere gli ultimi 5 kg. Lo consiglio a chi ha poco tempo ma vuole risultati.', '2025-04-01'),
        -- Andrea (8) → Piano 1 (3 stelle)
        (8, NULL, 1, 3, 'La dieta iperproteica mi ha aiutato a mettere massa, ma ho trovato difficile gestire le quantità di proteine. Forse troppo estremo per un principiante.', '2025-04-01'),
        -- Valentina (9) → Programma 5 (5 stelle)
        (9, 5, NULL, 5, 'Finalmente un programma completo che si può fare a casa senza attrezzi. Ho notato miglioramenti su tutto il corpo, soprattutto su braccia e spalle.', '2025-06-30'),
        -- Valentina (9) → Piano 4 (4 stelle)
        (9, NULL, 4, 4, 'La dieta vegetariana è ben bilanciata e mi ha dato più energia. Unica pecca: le ricette sono un po'' ripetitive.', '2025-06-30'),
        -- Roberto (10) → Programma 4 (5 stelle)
        (10, 4, NULL, 5, 'Questo programma mi ha salvato la schiena! Dopo anni di dolori lombari, ora mi sento molto più stabile e forte.', '2025-07-15'),
        -- Martina (11) → Programma 2 (4 stelle)
        (11, 2, NULL, 4, 'Ottimo per la resistenza, ma le prime due settimane sono state durissime. Ora che ci sono abituata, mi piace molto.', '2025-05-15'),
        -- Martina (11) → Programma 6 (5 stelle)
        (11, 6, NULL, 5, 'La definizione muscolare è incredibile! Dopo due mesi ho perso 3 kg di grasso e ho guadagnato tono. Super consigliato!', '2025-07-01'),
        -- Martina (11) → Piano 1 (4 stelle)
        (11, NULL, 1, 4, 'La dieta iperproteica mi ha aiutato a recuperare dopo gli allenamenti intensi. Buon equilibrio tra gusto e nutrizione.', '2025-07-01');

    )sql";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, insertData, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Errore inserimento dati di test: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    std::cout << "Dati di test inseriti con successo" << std::endl;
    return true;
}

// ============================================================
// 3. MAIN
// ============================================================
int main() {
    sqlite3* db = nullptr;

    // Inizializza database e crea tabelle
    if (!initDatabase(db)) {
        return 1;
    }

    // Popola con dati di test (se vuoto)
    popolaDatiDiTest(db);

    // ------------------------------------------------------------------
    // QUI inizia il tuo programma vero e proprio (client-server, GUI, ecc.)
    // ------------------------------------------------------------------

    std::cout << "Server avviato. Premi INVIO per terminare." << std::endl;
    std::cin.get();

    sqlite3_close(db);
    return 0;
}