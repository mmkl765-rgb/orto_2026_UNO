// HEADER PER LA GESTIONE DI TUTTE LE VARIABILI TEMPORALI
// sia per l'uso del pulsante multifinzione che per i timer di irrigazione ed altro


// GESTIONE PULSANTE MULTIFUNZIONE
  #define TEMPO_OP1               3000UL                          // definizione tempo pressione per show dati           = dopo 3 sec
  #define TEMPO_OP2               8000UL                          // definizione tempo pressione per irrigazione forzata = dopo 8 sec
  #define TEMPO_OP3               16000UL                         // definizione tempo pressione per reboot forzato      = dopo 16 sec
  #define TEMPO_MAX               30000UL                         // definizione tempo pressione per timeout output      = dopo 30 sec

// DEFINIZIONE TEMPI OPERATIVI
#define TEMPO_IRRIGAZIONE_Z1        (50UL*60*1000/10)             // tempo di effettiva irrorazione in millisecondi "area cudumars" 5,0 min
#define TEMPO_IRRIGAZIONE_Z2        (20UL*60*1000/10)             // tempo di effettiva irrorazione in millisecondi "area pomodori" 2,0 min

#define APERTURA_VALVOLA_ZONA       (25UL*1000)                   // tempo di apertura e chiusura valvola di zona nuovo tipo (25 sec)
#define INTERVALLO_IRRIGAZIONI      (2UL*60*60*1000)              // 2 ore tra le possibili irrigazioni
#define INTERVALLO_LETTURE          (30UL*60*1000)                // 30 minuti tra le letture
#define REFRESH_LCD_MODO_ORDINARIO  (60UL*1000)                   // Aggiorna lo schermo solo 1 volta al minuto
#define AUTO_RESET                  (10UL * 24 * 60 * 60 * 1000)  // autoriavvio sistema ogni 10 giorni
