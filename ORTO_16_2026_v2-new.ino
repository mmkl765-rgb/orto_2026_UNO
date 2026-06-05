// FILE PRONCIPALE DEL SISTEMA DI IRRIGAZIONE ORTO
// LE FUNZIONI SONO CONTENUTE NEL FILE FINZIONI.ino GESTITE TUTTE CON VARIABILI GLOBALI
// IL COMPILATORE LE INCLUDE AUTOMATICAMENTE ALLA COMPILAZIONIE

// INCLUSIONI VARIE...
    #include "DEBUG.h"                  // file per la gestione debug (pre-processore)
    #include <LiquidCrystal_I2C.h>      // includo libreria display i2c
    #include <avr/wdt.h>                // includo il watchdog per il reboot del sistema
    #include "PIN.h"                    // header per la definizione dei pin arduino
    #include "UMIDITA.h"                // header per i valori _RAW di umidita e soglia
    #include "TIMERS.h"                 // header per valori di timer e contatempo vari
    #include "VERSIONI.h"               // header versioni software

// BUFFER_SCRITTURA: arrray di caratteri per la creazione del buffer di scrittura per lcd
char W_BUFF[21];  // 20 caratteri su una linea + terminatore! gestione overflow ok

// CREAZIONE LCD con i parametri presi ca i2c monitor
LiquidCrystal_I2C lcd(0x27, 20, 4);  // settaggio LCD indirizzo 0x27 display 20 caratteri 4 linee

// STRUTTURA DATI per immagazzinamento dati letture aree, con l'array nella strutura posso fare
// uno storico delle letture (giocando con i posizionali) da visualizzare in uno SHOW DATI...
// i valori di partenza non sono messi a "0" in quanto potrebbe partire accidentalmente
// 15 posizioni nell'array mi permettono di controllare:
// 1) con intervallo letture di 30 min => 7 ore
// 2) con intervallo letture di 20 min => 4 ore
struct DATI { 
  int     NUMERO_MISURAZIONI = 0;
  uint8_t NUMERO_IRRIGAZIONI = 0;
  long Z1_RAW = 0;
  long Z2_RAW = 0;
  uint8_t ZONA1[15]; 
  uint8_t ZONA2[15];
};
DATI TERRENO;

// definizioni dei timers (spostato da file esterno per non rischiare conflitti in compilazione)
unsigned long timer_display;                                  // cronometro per aggiornamento display
unsigned long timer_letture;                                  // cronometro da usare con millis() per il controllo sensori + irrigazione
unsigned long timer_irrigazione;                              // cronometro da usare per controllo irrigazione ogni 6 h

unsigned long PROSSIMA_LETTURA;                               // countdown per la prossima lettura sensori
unsigned long PROSSIMA_IRRIGAZIONE;                           // countdown per la prossima irrigazione se serve

// definizione dello stato pulsante
uint8_t STATO_PULSANTE = 0;                          // valore di partenza corrispondente alla modalità ordinaria  

// DEFINIZIONE MODALITA OPAERATIVA : definendo la modalità test si attiva il test dei sensori
// dai dati di lettura (visualizzati sia grezzi che in % ) si possono tarare le discrepanze
// #define TEST_MODE


void setup() {
  Serial.begin(9600);
  lcd.init();                     // avvia lcd
  lcd.backlight();                // attivo retroilluminazione poi gestita da pulsante
  PULISCI_LCD();                  // barba e capelli

  // messaggio di benvenuto ...
  strcpy_P(W_BUFF, PSTR("*** ORTO V2026 ***")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
  snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("VERSIONE SW : %s"), VERSIONE); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
  strcpy_P(W_BUFF, PSTR("SETUP SISTEMA..."));   prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  delay(350);

  // setuo pin led e test
  // set pin vari
  pinMode(PULSANTE,INPUT);
  pinMode(ALIM_SENS, OUTPUT);     digitalWrite(ALIM_SENS, LOW);     // setto alimentazione sensori e forzo la chiusura
  pinMode(ATTIVA_SCHEDA, OUTPUT); digitalWrite(ATTIVA_SCHEDA, LOW); // setto alimentazione scheda e forzo la chiusura
  pinMode(LIVELLO_H2O, INPUT);
  // N.B.: PIN VALVOLE vengono inizialmente impostati come input ed effettuo il cambio del PINMODE nella funzione
  // solo quando mi serve in modo da evitare accensioni involontarie e vibrazioni delle bobine 
  pinMode(VALVOLA_Z1, INPUT);
  pinMode(VALVOLA_Z2, INPUT);        
  pinMode(RELE_COMUNE, INPUT);       
  pinMode(RELE_POMPA, INPUT);
  // se sei in test mode non serve chiudere le valvole altrimenti si...
  #ifndef TEST_MODE
    // chiusura valvole zona per sicurezza !!! UN PO BRUTALE !!!!
    strcpy_P(W_BUFF, PSTR("- CHIUDO VALVOLE    ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
    digitalWrite (RELE_COMUNE,HIGH); pinMode (RELE_COMUNE,OUTPUT);  // abilita il pin per la chiusura
    digitalWrite(ATTIVA_SCHEDA, HIGH);  delay(10); // attiva scheda rele e stabilizza
    digitalWrite(RELE_COMUNE, LOW);     delay(10);  // attiva uscita rele comune (quello per chiudere le valvole)
    BARRA_AVANZAMENTO(APERTURA_VALVOLA_ZONA, 3);
    digitalWrite(RELE_COMUNE, HIGH);    delay(10);  // attiva uscita rele comune (quello per chiudere le valvole)
    digitalWrite(ATTIVA_SCHEDA, LOW); pinMode(RELE_COMUNE, INPUT);
  #endif
  // set led + test
  strcpy_P(W_BUFF, PSTR("- SET LED + PIN  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  pinMode(LED_R, OUTPUT); LampeggiaLED(LED_R, 2, 100); digitalWrite(LED_R, LOW);
  pinMode(LED_V, OUTPUT); LampeggiaLED(LED_V, 2, 100); digitalWrite(LED_V, LOW);
  pinMode(LED_B, OUTPUT); LampeggiaLED(LED_B, 2, 100); digitalWrite(LED_B, LOW);
  BARRA_AVANZAMENTO(1000, 3);

  // set timer          
  strcpy_P(W_BUFF, PSTR("- SET TIMERS       ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  timer_letture     = millis();                                // faccio la prima lettura nel setup poi una ogni XX tempo
  timer_irrigazione = millis();                                // settaggio cronometri
  timer_display     = millis() - REFRESH_LCD_MODO_ORDINARIO;   // settaggio crono per aggiornamento display
  BARRA_AVANZAMENTO(1000, 3);                                  // barra avanzamento 1 sec
  
  // effettua la prima lettura delle zone in modo da sapere lo stato di partenza
  strcpy_P(W_BUFF, PSTR("-PRIMA LETTURA ZONE ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  LETTURA_SENSORI ();

  // -- IMPOSTAZIONE STORICO : NUOVO METODO BIS--
  // dopo aver fatto la prima lettura salvo negli storici lo stesso valore della prima lettura cosi all'avvio non ho tutti zero...
  strcpy_P(W_BUFF, PSTR("- SET STORICO")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
  for (uint8_t rr=0; rr<15; rr++) {
    TERRENO.ZONA1[rr] = TERRENO.ZONA1[0]; TERRENO.ZONA2[rr] = TERRENO.ZONA2[0];
  }
  delay (500); // giusto per far leggere la scritta, ma si puo anche togliere.....

  // impostazione modo automatico (nel caso solito che all'avvio non venga premuto il pulsante ! )
  STATO_PULSANTE = 0;        // stato = 0 => automatico !
  
  PULISCI_LCD();
  prtln("FINE SETUP.....");

}

// --------------------------- FINE SETUP  ------------------------------

void loop() {

  // *********************************************  MODO TEST **********************************************************
  // se definito la moddalita test carica solo la funzione che esegue i test ricorsivamente
  #ifdef TEST_MODE
    ESEGUI_TEST_MODE();
  #endif
  // *********************************************  FINE MODO TEST **********************************************************



  // *********************************************  MODO ORDINARIO **********************************************************
    #ifndef TEST_MODE // modalita ordinaria solo se non abiliti il modo test

    // RIAVVIO AUTOMATICO: dopo il tempo impostato esegue un reboot preventivo contro "glitch" 
    // abilito lo STATO a 3 cosi sfrutto la macchina a stati
    if (millis() > AUTO_RESET) { STATO_PULSANTE = 3; }

    // STATO OPERATIVO: in base alla tempo di ressione del pulsante effettua
    //                  operazioni diverse. Leggi a dislpay premendo il pulsante.
    uint8_t lettura = GESTIONE_PULSANTE(PULSANTE);
    if (lettura != 0) STATO_PULSANTE = lettura;
    prt("STATO PULSANTE NEL LOOP = "); prtln_v (STATO_PULSANTE);

    // MACCHINA A STATI
      // 0) MODALITA' ORDINARIA - con controllo livello acqua ed in caso blocco totale
      if (STATO_PULSANTE==0) {
        // aggiornamento del display solo dopo il tempo impostato di 1 minuto e controllo livello h20 (metti si buchi la cisterna!)
        if (millis() - timer_display >= REFRESH_LCD_MODO_ORDINARIO && VERIFICA_LIVELLO(LIVELLO_H2O,false) == 1) {
          PULISCI_LCD();
          // messaggi display...
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("  MODO ORDINARIO")); prtln_v(W_BUFF);  SCRIVI_LCD(0, 0, W_BUFF);
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR(" Z1: %2d%% | Z2: %2d%%"), TERRENO.ZONA1[0],TERRENO.ZONA2[0]); // 17 caratteri
          prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
          //strcpy_P(W_BUFF, PSTR("  TIMER CONTROLLI  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
          PROSSIMA_LETTURA      = (INTERVALLO_LETTURE - (millis() - timer_letture));            // valore in millis
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("LETT.  %2lu MIN (%3d)"), PROSSIMA_LETTURA/60000UL, TERRENO.NUMERO_MISURAZIONI );
          prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
          PROSSIMA_IRRIGAZIONE  = (INTERVALLO_IRRIGAZIONI - (millis() - timer_irrigazione));    // valore in millis
          snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("IRR.  %2lu MIN (%3d)"), PROSSIMA_IRRIGAZIONE/60000UL, TERRENO.NUMERO_IRRIGAZIONI);
          prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);
          // reset timer aggiornamento LCD
          timer_display = millis();
        }
        
        // se sono passati 30 minuti dalla ultima lettura rieffettua la lettura
        if (millis() - timer_letture >= INTERVALLO_LETTURE) {
          PULISCI_LCD();
          strcpy_P(W_BUFF, PSTR("LETTURA SENSORI ...")); SCRIVI_LCD(0, 1, W_BUFF);
          LETTURA_SENSORI ();
          timer_letture = millis();   // reset timer letture ad "adesso"
        } 
        
        // se è passato il tempo per il controllo valori irrigazione e se c'e' acqua in cisterna
        // N.B.: se non ci fosse acqua in cisterna la funzione stessa gestisce il blocco totale del ciclo !!!
        if (millis() - timer_irrigazione >= INTERVALLO_IRRIGAZIONI && VERIFICA_LIVELLO(LIVELLO_H2O,false) == 1) { 
          PULISCI_LCD(); LampeggiaLED(LED_B, 4, 100);    // warning visivo
          // se serve bagna le zone
          strcpy_P(W_BUFF, PSTR("VERIFICA IRRIGAZIONE")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
          // ZONA 1
          if (TERRENO.ZONA1[0] <= SOGLIARH_Z1) {
            strcpy_P(W_BUFF, PSTR("ZONA 1 => IRRIGO...")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
            // bagna con tempo differenziato tra Z1 e Z2
            BAGNA_ZONA(VALVOLA_Z1, RELE_POMPA,TEMPO_IRRIGAZIONE_Z1); 
            strcpy_P(W_BUFF, PSTR("ZONA 1 => FINE !   ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
            }
          else { 
            strcpy_P(W_BUFF, PSTR("ZONA 1 => TERRA OK ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
            }
          // ZONA 2
          if (TERRENO.ZONA2[0] <= SOGLIARH_Z2) {
            strcpy_P(W_BUFF, PSTR("ZONA 2 => IRRIGO...")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
            // bagna con tempo differenziato tra Z1 e Z2
            BAGNA_ZONA(VALVOLA_Z2, RELE_POMPA,TEMPO_IRRIGAZIONE_Z2); 
            strcpy_P(W_BUFF, PSTR("ZONA 2 => FINE !   ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
            }
          else { 
            strcpy_P(W_BUFF, PSTR("ZONA 2 => TERRA OK ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
            }
          //poi resetta il timer "ad adesso"
          timer_irrigazione = millis();
          delay(500);
        } 
         STATO_PULSANTE=0;   // forza lo stato nella modalita automatica prima di uscire
      }

      // 1) VEDI DATI : visualizza tutti i dati del sistema, tempo attivita ecc ecc
      if (STATO_PULSANTE==1) {
        PULISCI_LCD();
        // prima schermata...
        strcpy_P(W_BUFF, PSTR("MODO  VEDI  DATI "));   prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
        strcpy_P(W_BUFF, PSTR("- ORTO AUTOMATICO -")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        strcpy_P(W_BUFF, PSTR("VERSIONE: ORTO V2026")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("VERSIONE SW: %s"), VERSIONE); prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);
        delay (4000);
        // seconda schermata
        PULISCI_LCD();
        unsigned long t = millis() / 1000UL; // t in secondi
        int giorni = t / 86400UL; int ore = (t % 86400UL) / 3600; int minuti = (t % 3600) / 60; // da secondi a minuti-ore-giorni
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("UPTIME: %02dGG %02d:%02d"), giorni, ore, minuti);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("RAM LIBERA: %d%%"), map(freeMemory(), 0, 2048, 0, 100)); 
        prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF),PSTR("MISURAZIONI: %d"),TERRENO.NUMERO_MISURAZIONI);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF),PSTR("IRRIGAZIONI: %d"),TERRENO.NUMERO_IRRIGAZIONI);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);
        delay (5000);
        // terza schermata - storico comparativo
        PULISCI_LCD();
        // Riga 0: Intestazione colonne (20 caratteri esatti)
        strcpy_P(W_BUFF, PSTR("[STORICO]   Z1 |  Z2")); SCRIVI_LCD(0, 0, W_BUFF); prtln_v(W_BUFF);
        // Riga 1 : i valori di adesso
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("-ADESSO:   %2u%% | %2u%%"), TERRENO.ZONA1[0], TERRENO.ZONA2[0]); SCRIVI_LCD(0, 1, W_BUFF); prtln_v(W_BUFF);
        // Riga 2: Lettura di 2 ore fa (indice [4])
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR(" -2 ORE:   %2u%% | %2u%%"), TERRENO.ZONA1[4], TERRENO.ZONA2[4]); SCRIVI_LCD(0, 2, W_BUFF); prtln_v(W_BUFF);
        // Riga 3: Lettura di 4 ore fa (indice [8])
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR(" -4 ORE:   %2u%% | %2u%%"), TERRENO.ZONA1[8], TERRENO.ZONA2[8]); SCRIVI_LCD(0, 3, W_BUFF); prtln_v(W_BUFF);
        // Un unico delay finale per dare il tempo di leggere, poi torna in automatico
        delay(8000); PULISCI_LCD(); STATO_PULSANTE = 0; // Torna in modalità ordinaria
      }

      // 2) IRRIGAZIONE FORZATA DI TUTTE LE AREE ( CIRCA 10 MINUTI !)
      // la funzione è di tipo bloccante pertanto finche non finisce l'irrigazione forzatat non fa altro
      if (STATO_PULSANTE==2 && VERIFICA_LIVELLO(LIVELLO_H2O,false) == 1) {
        LampeggiaLED(LED_B, 4, 150); digitalWrite(LED_B, HIGH);
        PULISCI_LCD();
        strcpy_P(W_BUFF, PSTR("IRRIGAZIONE FORZATA"));  prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
        strcpy_P(W_BUFF, PSTR("ZONA1: IRRIG. INIZIO")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        BAGNA_ZONA(VALVOLA_Z1, RELE_POMPA,TEMPO_IRRIGAZIONE_Z1);
        strcpy_P(W_BUFF, PSTR("ZONA1: IRRIG. FINE  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        strcpy_P(W_BUFF, PSTR("ZONA2: IRRIG. INIZIO")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        BAGNA_ZONA(VALVOLA_Z2, RELE_POMPA,TEMPO_IRRIGAZIONE_Z2);
        strcpy_P(W_BUFF, PSTR("ZONA2: IRRIG. FINE  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        digitalWrite(LED_B, LOW); STATO_PULSANTE=0; PULISCI_LCD();
      }

      // 3) REBOOT FORZATO VOLUTO DA UTENTE
      if (STATO_PULSANTE==3) {
        PULISCI_LCD();
        if (millis() > AUTO_RESET) {
          strcpy_P(W_BUFF, PSTR("  - AUTO-RESET -  ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
          strcpy_P(W_BUFF, PSTR("MANUTENZIONE 10 GG")); prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        }
        else {
          strcpy_P(W_BUFF, PSTR("- REBOOT FORZATO -"));     prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
          strcpy_P(W_BUFF, PSTR("RICHIESTO DA UTENTE"));    prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
          strcpy_P(W_BUFF, PSTR("TRA 2 SECONDI"));          prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        }
        BARRA_AVANZAMENTO(2000, 3);
        wdt_enable(WDTO_1S);  // attiva watchdog con timeout di 1 Sec
        while (1) {}          // attende il reset
      }
  #endif
  // *********************************************  FINE MODO ORDINARIO **********************************************************

  // lampeggiamento del "sono vivo !"
  prtln("FINE LOOP"); 
  LampeggiaLED(LED_V,2,200);

  // fine loop

}

/*

PER TUTTE LE INFO RIGUARDO ALLA VERSIONE LEGGI IL FILE VERSIONI.h

*/