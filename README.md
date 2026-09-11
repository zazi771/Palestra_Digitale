# Palestra Digitale — Bud Spencer
Piattaforma web per la gestione di una palestra: cartella clinica,
piani alimentari, programmi di allenamento e monitoraggio dei progressi.

Progetto per l'esame di **Laboratorio di Informatica**.

## Descrizione
Web app con backend in **C++** (framework **Crow**), frontend in HTML/CSS/JS
vanilla servito staticamente dallo stesso server e database **SQLite**.
Tre aree distinte in base al ruolo:

- **Cliente** — compila la cartella clinica (antropometria, patologie,
  intolleranze, infortuni, farmaci, note mediche), consulta piano alimentare
  e programmi di allenamento, avvia una sessione con timer, inserisce
  progressi e lascia feedback.
- **Trainer** — carica certificazione e CV, crea e assegna programmi di
  allenamento con esercizi (serie, ripetizioni, recupero, video), segue i
  progressi dei clienti e ne legge i feedback.
- **Nutrizionista** — carica certificazione e CV, crea piani alimentari con
  pasti e cibi (quantità in grammi), gestisce e legge i feedback dei clienti.

Autenticazione con hah della password tramite **bcrypt**; nessuna sessione
lato server (login tramite oggetto utente in `localStorage`).

## Tecnologie e requisiti
- C++20, CMake ≥ 3.17, CLion (toolchain MinGW; `MinGW\bin` nel PATH) o MSVC
- Librerie vendored in `libs/`: Crow 1.1.1, Asio, SQLiteCpp (+ sqlite3), bcrypt
- Server su `http://localhost:18080`

## Struttura del progetto
- `src/main.cpp` — avvio server, file statici, registrazione rotte
- `src/routes/` — auth, cliente, trainer, nutrizionista (+ uploads.h)
- `src/models/` — modello per ogni tabella del DB
- `src/database/` — livello di accesso ai dati (SQL parametrizzato)
- `Frontend/` — home, area_cliente, area_trainer, area_nutrizionista
- `uploads/` — file caricati (CV, certificazioni)
- `libs/` — dipendenze vendored

## Compilazione ed esecuzione
1. Compilare con CLion (target `cmake-build-debug`).
2. L'eseguibile deve stare in `cmake-build-debug/`: il server risale i
   percorsi del progetto partendo da lì.
3. Eseguire `cmake-build-debug/PALESTRA_DIGITALE.exe` e aprire
   `http://localhost:18080`.
4. Il database `PalestraDigitale.db` (incluso in git) deve esistere: la
   applicazione non lo crea né lo migra.

## API principali
### Auth
| Metodo | Percorso | Descrizione |
|---|---|---|
| POST | /api/auth/registra | Registrazione utente (email, password bcrypt, ruoli) |
| POST | /api/auth/login | Login con verifica password |

### Cliente
| Metodo | Percorso | Descrizione |
|---|---|---|
| GET/POST | /api/cliente/{id}/cartella-clinica | Storico / nuova rilevazione |
| GET | /api/cliente/{id}/piano-alimentare | Piani alimentari assegnati |
| GET | /api/cliente/{id}/programma-allenamento | Programmi assegnati + esercizi |
| GET/POST | /api/cliente/{id}/sessioni | Elenco / registrazione sessione |
| PATCH | /api/cliente/{id}/programmi/{id}/stato | Aggiorna stato programma |
| GET/POST/DELETE | /api/cliente/{id}/feedback | Gestione feedback del cliente |

### Trainer
| Metodo | Percorso | Descrizione |
|---|---|---|
| GET/POST | /api/trainer/{id}/certificazione | Upload/lettura certificazione + CV |
| GET | /api/trainer/{id}/clienti | Elenco clienti |
| CRUD | /api/trainer/{id}/clienti/{cid}/piani | Piani di allenamento dei clienti |
| GET | /api/trainer/{id}/clienti/{cid}/progressi | Statistiche sedute e programmi |
| GET | /api/trainer/{id}/clienti/{cid}/piani/{pid}/feedback | Feedback su un piano |
| GET | /api/esercizi | Catalogo esercizi (condiviso) |

### Nutrizionista
| Metodo | Percorso | Descrizione |
|---|---|---|
| GET/POST | /api/nutrizionista/{id}/certificazione | Upload/lettura certificazione + CV |
| GET | /api/nutrizionista/{id}/clienti | Elenco clienti |
| CRUD | /api/nutrizionista/{id}/clienti/{cid}/piani | Piani alimentari (pasti + cibi) |
| GET | /api/cibi | Elenco/ricerca cibi (`?q=`) |
| GET | /api/nutrizionista/{id}/clienti/{cid}/piani/{pid}/feedback | Feedback su un piano |

### File statici
| Metodo | Percorso | Descrizione |
|---|---|---|
| GET | / | Frontend/home.html |
| GET | /uploads/{file} | File caricati (solo basename) |
| GET | /{path} | Qualsiasi file dentro Frontend/ (con controllo traversamento) |

## Schema del database
11 tabelle in `PalestraDigitale.db`: **Utente**, **Cartella_clinica**,
**Certificazione**, **Cibo**, **Esercizio**, **Piano_alimentare**, **Pasto**,
**Pasto_cibo**, **Programma_allenamento**, **Programma_esercizio**,
**Sessione**, **Feedback**, **Utente_Programma** (assegnazioni).

## Autori
Giorgio Natile - Gabriele Carlo Mongelli - Nazario Rinaldi