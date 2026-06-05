// #####################################################################################################################

// FILE SEPARATO PER LA GESTIONE DELLE VARIE FUNZIONI USATE DAL SISTEMA


// TEST_FUNCTIONS  Funzione vuota che contiene tutta la modalita test in un unico blocco

void ESEGUI_TEST_MODE() {
    
    PULISCI_LCD();
    strcpy_P(W_BUFF, PSTR("*** ORTO V2026 ***"));   SCRIVI_LCD(0, 0, W_BUFF);
    strcpy_P(W_BUFF, PSTR("MODALITA' TEST"));       SCRIVI_LCD(0, 1, W_BUFF);
    strcpy_P(W_BUFF, PSTR("CARICAMENTO ........")); SCRIVI_LCD(0, 2, W_BUFF);
    BARRA_AVANZAMENTO(3000, 3);
  
    STATO_PULSANTE=GESTIONE_PULSANTE(PULSANTE); 
    prt("stato pulsante = "); prtln_v(STATO_PULSANTE);

    // TEST SENSORI
    // effettua 5 test - lettura consecutivi
      for (int q=0; q<10; q++) {
        PULISCI_LCD();
        strcpy_P(W_BUFF, PSTR("LETTURA SENSORI"));      SCRIVI_LCD(0, 0, W_BUFF);
        LETTURA_SENSORI ();
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("Z1: %d => %d%%"), TERRENO.Z1_RAW, TERRENO.ZONA1[0]);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("Z2: %d => %d%%"), TERRENO.Z2_RAW, TERRENO.ZONA2[0]);
        prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
        BARRA_AVANZAMENTO(3000, 3); // vedere se il tempo di visualizzazione va bene....
      }

    // TEST LIVELLO H2O
    PULISCI_LCD();
    digitalWrite(LED_B, HIGH);
    strcpy_P(W_BUFF, PSTR("TEST LIVELLO ACQUA"));      prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);
    for (int q=0; q<10; q++) {
      if (VERIFICA_LIVELLO(LIVELLO_H2O, true) == 1) {
        strcpy_P(W_BUFF, PSTR("LIVELLO ACQUA OK    "));     prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);
        LampeggiaLED(LED_V, 2, 150); digitalWrite(LED_V, LOW);
        BARRA_AVANZAMENTO(3000, 3);
      }
      else if (VERIFICA_LIVELLO(LIVELLO_H2O, true) == 0) { 
        strcpy_P(W_BUFF, PSTR("LIVELLO ACQUA BASSO"));    prtln_v(W_BUFF);  SCRIVI_LCD(0, 1, W_BUFF);
        LampeggiaLED(LED_R, 3, 150); digitalWrite(LED_R, LOW);
        BARRA_AVANZAMENTO(3000, 3);
      }
      snprintf_P(W_BUFF, sizeof(W_BUFF), PSTR("TEST N %d"), q+1);    prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);
    
    }
    digitalWrite(LED_B, LOW);


    // TEST VALVOLE ZONA E POMPA (TEST RELE)
    PULISCI_LCD();
    // forzo lo stato di HIGH sui pin rele per evitare sfarfalii sulle bobine
    strcpy_P(W_BUFF, PSTR("TEST RELE"));      SCRIVI_LCD(0, 1, W_BUFF);
    digitalWrite (VALVOLA_Z1,HIGH);  digitalWrite (VALVOLA_Z2,HIGH);
    digitalWrite (RELE_COMUNE,HIGH); digitalWrite (RELE_POMPA,HIGH);
    // risveglia i rele: ritrasforma i pin in OUTPUT con uscita a HIGH (impostata sopra)
    pinMode (VALVOLA_Z1,OUTPUT);  pinMode (VALVOLA_Z2,OUTPUT);
    pinMode (RELE_COMUNE,OUTPUT); pinMode (RELE_POMPA,OUTPUT);
    digitalWrite(ATTIVA_SCHEDA, HIGH);  delay(10); // attiva scheda rele e stabilizza
    digitalWrite(RELE_COMUNE, LOW);    delay(10);  // attiva uscita rele comune (quello per chiudere le valvole)
    strcpy_P(W_BUFF, PSTR("TEST ZONA 1"));      SCRIVI_LCD(0, 1, W_BUFF);
    // TEST RELE-VALVOLA Z1 PER 10 SEC
    digitalWrite(VALVOLA_Z1, LOW);  BARRA_AVANZAMENTO(10000, 3); delay (10);
    digitalWrite(VALVOLA_Z1, HIGH); BARRA_AVANZAMENTO(10000, 3); delay (10);
    // TEST RELE-VALVOLA Z2 PER 10 SEC
    strcpy_P(W_BUFF, PSTR("TEST ZONA 2"));      SCRIVI_LCD(0, 1, W_BUFF);
    digitalWrite(VALVOLA_Z2, LOW);  BARRA_AVANZAMENTO(10000, 3); delay (10);
    // CON VALOLA APERTA PROVO LA POMPA
    // TEST RELLE/POMPA PER 3 SEC
    strcpy_P(W_BUFF, PSTR("TEST POMPA     "));      SCRIVI_LCD(0, 1, W_BUFF);
    digitalWrite(RELE_POMPA, LOW);  BARRA_AVANZAMENTO(3000, 3); delay (10);
    digitalWrite(RELE_POMPA, HIGH); delay (50);
    strcpy_P(W_BUFF, PSTR("SET FINE TEST    "));      SCRIVI_LCD(0, 2, W_BUFF);
    // ALLA FINE CHIUDO VALVOLA 2 CON TEMPO MAGGIORATO ! (>10+3 !!!)
    digitalWrite(VALVOLA_Z2, HIGH); BARRA_AVANZAMENTO(15000, 3); delay (10);
    digitalWrite(RELE_COMUNE, LOW); delay (10);
    digitalWrite(ATTIVA_SCHEDA, LOW);  delay(10); // attiva scheda rele e stabilizza
    // isolo i pin rele contro attivazioni involontarie
    pinMode(VALVOLA_Z1, INPUT);  pinMode(VALVOLA_Z2, INPUT);
    pinMode(RELE_COMUNE, INPUT); pinMode(RELE_POMPA, INPUT);

}


// PULISCI LCD: funzione veloce per evitare di usare sempre lcd.clear() che gerena sfarfalii...
void PULISCI_LCD() {
  for (int bb = 0; bb < 4; bb++) {       // per ogni riga
    lcd.setCursor(0, bb);                // vai all’inizio della riga
    for (int aa = 0; aa < 20; aa++) {    // per ogni colonna
      lcd.print(" "); }                  // stampa spazio
  }
  lcd.setCursor(0, 0);                    // riporta il cursore in alto a sinistra
}

// LAMPEGGIO LED 
// settaggi: led , numero lampeggi (max8) e durata lampeggio (millis)
void LampeggiaLED(uint8_t pin, uint8_t codice, uint8_t durata) {
  for (uint8_t i = 0; i < (codice * 2); i++) {
    digitalWrite(pin, HIGH); delay(durata); digitalWrite(pin, LOW);  delay(durata);
  }
}

// SCRIVI SU LCD
// funzione per scrivere le righe del display, parametri posizional1 1=numero riga - 2=messaggio)
// da usare cosi strcpy_P(W_BUFF, PSTR("messaggio...")); SCRIVI_LCD(0,1,W_BUFF);
// usa la logica x,y del piano cartesiano
void SCRIVI_LCD(uint8_t colonna, uint8_t riga, const char* TXT) {
  lcd.setCursor(colonna, riga);
  lcd.print(TXT);
}

// BARRA AVANZAMENTO VOLUTAMENTE BLOCCANTE con valori numerici in % creati in base al tempo da gestire
// la riga per la scrittura sara' l'ultima dell'lcd ( riga 3 ), tempo da gestire in millis
// aggiornamento ogni 1% , output come segue "-- AVANZAMENTO  12  % --"
void BARRA_AVANZAMENTO(unsigned long TEMPO_DA_GESTIRE, uint8_t riga) {
  unsigned long tempo_inizio = millis();
  int percent_precedente = -1; 
  // Scrive la cornice fissa una sola volta per evitare sfarfallio
  lcd.setCursor(0, riga);
  lcd.print(F("AVANZAMENTO:    %")); 
  while (millis() - tempo_inizio <= TEMPO_DA_GESTIRE) {
    uint8_t percent_avanzamento = map((millis() - tempo_inizio), 0, TEMPO_DA_GESTIRE, 0, 100);
    // Aggiorna il display SOLO quando la percentuale cambia davvero
    if (percent_avanzamento != percent_precedente) { 
      lcd.setCursor(13, riga); 
      // Aggiunge spazi per pulire i numeri precedenti (es. quando passa da 10 a 9 o resta il 100)
      if (percent_avanzamento < 10) lcd.print(F("  ")); 
      else if (percent_avanzamento < 100) lcd.print(F(" "));
      lcd.print(percent_avanzamento);
      percent_precedente = percent_avanzamento;
    }
    delay(10); // Piccola pausa di respiro per il processore
  }
}

// IRRIGAZIONE ZONA VOLUTA con apertura e chiusura valvole zona
// ricordarsi che i pin di comando in modalita di riposo sono definiti come input ! 
// vanno riattivati prima di procedere !! La logica della funzione è spiegata sotto
// riattivo i pin di comando => led => attiva scheda => attiva comune => 
// attiva valvola zona specifica => attendi tempo apertura => disattiva comune e valvola zona => 
// attivo pompa => attendi tempo irrigazione => spegni pompa => attiva rele comune x chiusura valvola =>
// attendi tempo => spegni comune => spegni scheda e led
// lampeggi led gestiti dalla funzione flashled, tempo gestito dalla barra avanzamento bloccante
void BAGNA_ZONA(uint8_t RELE_ZONA, uint8_t POMPA_H20, long TEMPO) {
  digitalWrite(LED_B, HIGH);                     // attiva led blu...
  // forzo lo stato di HIGH sui pin rele per evitare sfarfalii sulle bobine
  digitalWrite (VALVOLA_Z1,HIGH);  digitalWrite (VALVOLA_Z2,HIGH);
  digitalWrite (RELE_COMUNE,HIGH); digitalWrite (RELE_POMPA,HIGH);
  // risveglia i rele: ritrasforma i pin in OUTPUT con uscita a HIGH (impostata sopra)
  pinMode (VALVOLA_Z1,OUTPUT);  pinMode (VALVOLA_Z2,OUTPUT);
  pinMode (RELE_COMUNE,OUTPUT); pinMode (RELE_POMPA,OUTPUT);
  // proseguo ....
  digitalWrite(ATTIVA_SCHEDA, HIGH);  delay(10); // attiva scheda rele e stabilizza
  digitalWrite(RELE_COMUNE, LOW);    delay(10);  // attiva uscita rele comune (quello per chiudere le valvole)
  digitalWrite(RELE_ZONA, LOW);                  // attiva il rele valvola zona (apre solo quella)
  BARRA_AVANZAMENTO(APERTURA_VALVOLA_ZONA, 3);   // barra che indica la % tempo apertura valvola su riga 3
  digitalWrite(RELE_COMUNE, HIGH);   delay(10);  // spengo comune
  digitalWrite(RELE_ZONA, HIGH);     delay(10);  // tolgo alimentazione alla valvola di zona lasciandola aperta
  digitalWrite(POMPA_H20, LOW);                  // attivazione pompa acqua....
  BARRA_AVANZAMENTO(TEMPO, 3);                   // per il tempo impostato in base alla zona su riga 3
  // N.B.: LA LENTEZZA DI CHIUSURA DELLA VALVOLA GESTISCE IL COLPO D'ARIETE !
  digitalWrite(POMPA_H20, HIGH); delay(10);      // disattivazione pompa acqua
  digitalWrite(RELE_COMUNE, LOW);                // chiude la valvola di zona alimentando solo il comune
  BARRA_AVANZAMENTO(APERTURA_VALVOLA_ZONA, 3);   // barra che indica la % tempo chiusura valvola su riga 3
  digitalWrite(RELE_COMUNE, HIGH); delay(10);    // chiude contatto rele comune
  digitalWrite(ATTIVA_SCHEDA, LOW);              // spegni scheda rele
  // isolo i pin rele contro attivazioni involontarie
  pinMode(VALVOLA_Z1, INPUT);  pinMode(VALVOLA_Z2, INPUT);
  pinMode(RELE_COMUNE, INPUT); pinMode(RELE_POMPA, INPUT);
  TERRENO.NUMERO_IRRIGAZIONI ++;                 // incrementa il numero delle irrigazioni effettuate
  LETTURA_SENSORI ();                            // lettura post irrigazione
  digitalWrite(LED_B, LOW);                      // spegne LED al termine
}

// GESTIONE PULSANTE - funzione che ritorna uno stato associato alla pressione del pulsante
// in base alla durata della pressione pulsante si possono gestire piu cose, basta creare
// usare cosi: STATO_PULSANTE = GESTIONE_PULSANTE(PULSANTE); e ritorna STATO_PULSANTE = 0 o 1 o 3
uint8_t GESTIONE_PULSANTE(uint8_t BUTTON) {
  static bool scritto1 = false;                                           // definizione variabili interne 1
  static unsigned long pressTime = 0;                                     // definizione variabili interne 2
  uint8_t STATO = 0;                                                      // definizione variabili interne 3
  if (digitalRead(BUTTON) == HIGH) { delay(50);                           // debounce
    if (digitalRead(BUTTON) == HIGH) {                                    // se è stato premuto il pulsante  
      prtln("PREMUTO PULSANTE MULTIFUNZIONE");                            // debug seriale
      PULISCI_LCD();                                                      // pulisci lcd
      pressTime = millis();                                               // resetta il timer
      scritto1 = false;                                                   // il msg 1 è stato scritto ???
      while (digitalRead(BUTTON) == HIGH) {                                // finchè il pulsante è premuto
        if (!scritto1) {                                                  // se non è gia scritto il msg 1
          strcpy_P(W_BUFF, PSTR("PULSANTE PREMUTO"));                     // crea msg... 
          SCRIVI_LCD(0, 0, W_BUFF);                                       // scrivilo...
          scritto1 = true;                                                // ricordati che è stato scritto (bool=true)
          LampeggiaLED(LED_V,1,100); }                                    // lampeggia qualcosa
        // GESTIONE DEI TEMPI DI PREASSIONE PULSANTE
        // TEMPO = TIMEOUT (30 sec)
        if ((millis() - pressTime) >= TEMPO_MAX) {                  // TIMEOUT => ritorno valore 0
          PULISCI_LCD();                                                  // pulisci lcd
          strcpy_P(W_BUFF, PSTR("PREMUTO PER"));                          // crea msg 1
          SCRIVI_LCD(0, 0, W_BUFF);                                       // stampa msg 1
          strcpy_P(W_BUFF, PSTR("TROPPO TEMPO"));                         // crea msg 2
          SCRIVI_LCD(0, 2, W_BUFF);                                       // stampa msg 2
          LampeggiaLED(LED_R,3,150);                                      // warning visivo
          STATO=0;                                                        // stato di ritorno = 0 => ORDINARIO
        }                                                                 // scrivi errore + ritorna sicurezza tempo massimo 
        // TEMPO = REBOOT (19 sec)
        else if (millis()-pressTime>TEMPO_OP3) {                          // REBOOT => ritorno valore "3"
          PULISCI_LCD();                                                  // pulisci lcd
          strcpy_P(W_BUFF, PSTR("REBOOT FORZATO"));                       // crea msg 1
          SCRIVI_LCD(0, 2, W_BUFF);                                       // stampa msg1
          LampeggiaLED(LED_V,5,150);                                      // warning visivo per reboot forzato
          STATO=3;                                                        // assegnazione numero stato
        }
        // TEMPO = IRRIGAZIONE FORZATA (10 sec)
        else if (millis()-pressTime>TEMPO_OP2) {                          // IRRRIGAZIONE FORZATA => ritorno valore "2"
          strcpy_P(W_BUFF, PSTR("IRRIGAZIONE FORZATA"));                  // crea msg1
          SCRIVI_LCD(0, 2, W_BUFF);                                       // stampa msg1
          LampeggiaLED(LED_V,4,150);                                      // warning visivo per irrigazione forzata
          STATO=2;                                                        // assegnazione numero stato
        }
        // TEMPO = SHOW DATI (3 sec)
        else if (millis()-pressTime>TEMPO_OP1) {                          // SHOW DATI => ritorno valore "1"
          strcpy_P(W_BUFF, PSTR("VEDI DATI"));                            // crea msg1
          SCRIVI_LCD(0, 2, W_BUFF);                                       // stampa msg1
          LampeggiaLED(LED_V,3,150);                                      // warning visivo per show dati
          STATO=1;                                                        // assegnazione numero stato
        }
        // pressione per tempo strambo da come stato il valore 0 => ordinario
        delay(50);
      }
    }
  }
  prt ("STATO DA GESTIONE BOTTONE (dentro funzione) = "); prtln_v(STATO);                   // debug stato in seriale
  return STATO;                                                           // rotorna il numero dello stato modificato dalla funzione (0,1,2,3)
}

// CONTROLLO LIVELLO ACQUA - funzionevolutamente bloccante => necessitasi operatore per riavviare
// la funzione modifica il bool del livello h20 in base alla situazione
// il sensore usa l'acqua per chiudere il circuito, se c'è acqua LOW , se non c'e' HIGH
// se manca blocca tutto altrimenti ritorna true
// uso if (VERIFICA_LIVELLO(LIVELLO_H2O,false/true)==true) {... (se c'e' acqua)
bool VERIFICA_LIVELLO(uint8_t SensoreLivelloH2O, bool TEST) {
  bool mostra_msg = false;                                                        // definisci bool interni 2
  digitalWrite(ALIM_SENS, HIGH); delay(100);                                      // attiva sensori e stabilizza
  prt("VALORE ANALOGICO SENS. ACQUA (dentro funzione!) = "); prtln_v(analogRead(SensoreLivelloH2O));
  if (digitalRead(SensoreLivelloH2O)==1)                { digitalWrite(ALIM_SENS, LOW); prtln ("acqua presente"); return true; }
  else if (digitalRead(SensoreLivelloH2O)==0 && TEST)   { digitalWrite(ALIM_SENS, LOW); prtln ("acqua bassa");    return false;}
  else if (digitalRead(SensoreLivelloH2O)==0 && !TEST)  {
       PULISCI_LCD(); digitalWrite(LED_R, HIGH);                                  // led errore acceso fisso !!!
      // accendo un led e spengo l'altro per creare poi nel while l'intermittenza delle luci blu-verde con rosso sempre acceso
      digitalWrite(LED_B, HIGH);
      digitalWrite(LED_V, LOW);
    while (1) {                                                                   // ciclo infinito
      if (!mostra_msg) {                                                          // solo se mostra_msg è falsa scrivi (evita sfarfallio lcd)
        strcpy_P(W_BUFF, PSTR("LIVELLO ACQUA BASSO"));  prtln_v(W_BUFF); SCRIVI_LCD(0, 0, W_BUFF);  // creo il msg lcd e lo scrivo
        strcpy_P(W_BUFF, PSTR("**  FERMO TUTTO  **"));  prtln_v(W_BUFF); SCRIVI_LCD(0, 1, W_BUFF);      // creo il msg lcd e lo scrivo
        strcpy_P(W_BUFF, PSTR("1)RIFORNIRE CISTERNA")); prtln_v(W_BUFF); SCRIVI_LCD(0, 2, W_BUFF);    // creo il msg lcd e lo scrivo
        strcpy_P(W_BUFF, PSTR("2)PREMI PER RIAVVIO ")); prtln_v(W_BUFF); SCRIVI_LCD(0, 3, W_BUFF);    // creo il msg lcd e lo scrivo
        mostra_msg = true; delay(100);                                            // bool diventa true (sfarfallio lcd) + ritarda
      }
      // attesa pressione per reboot dopo rifornimento
      if (digitalRead(PULSANTE) == HIGH) { delay(100);                             // leggo se c'e' la pressione del pulsante + debounce
        if (digitalRead(PULSANTE) == HIGH) {                                       // seconda lettura pressione pulsante
          digitalWrite(LED_R, LOW); digitalWrite(LED_B, LOW);                     // spengo tutti i led
          digitalWrite(LED_V, LOW);                                               // spengo tutti i led
          BARRA_AVANZAMENTO(2000, 3);                                             // attendo 2 secondi
          wdt_enable(WDTO_1S);                                                    // attiva watchdog con timeout di 1 Sec
          while (1) {}                                                            // attende il reset /reboot totale sistema
        }
      }
      digitalWrite(LED_B, !digitalRead(LED_B));
      digitalWrite(LED_V, !digitalRead(LED_V));
      delay(250);                                    // rallento il ciclo col led lampeggiante, il sistema rimane in attesa
    }
  }
}

// LETTURA SENSORI TERRENO
// legge i sensori terreno attivando l'alimentazione e leggendo il segnale di ritorno per XX volte e facendo la media
void LETTURA_SENSORI () {
  // warning visivo....
  digitalWrite(LED_V, HIGH); LampeggiaLED(LED_B, 2, 100); digitalWrite(LED_B, HIGH);
  // reset contatori dato grezzo
  TERRENO.Z1_RAW = 0; TERRENO.Z2_RAW = 0;
  // sposta le letture liberando lo spazio per la prossima lettura cancellando il valore della piu vecchia
  prtln ("Arretro letture precedenti");
  for (int iz = 14; iz > 0; iz--) {
    // la nuova posizione 15 = alla vcchia posizione 14...
    TERRENO.ZONA1[iz] = TERRENO.ZONA1[iz-1]; TERRENO.ZONA2[iz] = TERRENO.ZONA2[iz-1];
  }
  // attivazione alimentazione e somma letture
  prtln ("Effettuo letture zone");
  digitalWrite( ALIM_SENS , HIGH ); delay (10);
  for (int co=0; co<100; co++) {
    TERRENO.Z1_RAW += analogRead(S_ZONA1); delay(10);
    TERRENO.Z2_RAW += analogRead(S_ZONA2); delay(10);
  }
  digitalWrite(ALIM_SENS, LOW);
  prtln("Calcolo media e percentuali con correzione zona...");
  // zona 1
  TERRENO.Z1_RAW = ((TERRENO.Z1_RAW+50)/100);
  prt("Media arrotondata di Zona1 = "); prt_v(TERRENO.Z1_RAW);
  TERRENO.ZONA1[0] = constrain(map(TERRENO.Z1_RAW, SECCO_RAW_VALUE, BAGNATO_RAW_VALUE, 0, 99) + CORREZIONE_Z1 , 0 , 99 );
  prt(" => "); prtln_v(TERRENO.ZONA1[0]);
  // zona 2
  TERRENO.Z2_RAW = ((TERRENO.Z2_RAW+50)/100);
  prt("Media arrotondata di Zona2 = "); prt_v(TERRENO.Z2_RAW);
  TERRENO.ZONA2[0] = constrain(map(TERRENO.Z2_RAW, SECCO_RAW_VALUE, BAGNATO_RAW_VALUE, 0, 99) + CORREZIONE_Z2 , 0, 99);
  prt(" => "); prtln_v(TERRENO.ZONA2[0]);
  // incremento contatore numero misurazioni effettuate
  TERRENO.NUMERO_MISURAZIONI++ ;
  // warning visivo fine lettura
  LampeggiaLED(LED_V, 2, 100); digitalWrite(LED_V, LOW); digitalWrite(LED_B, LOW);
}

// FUNZIONE CONTROLLO MEMORIA (COPIATA DA GEMINI !!)
int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

// ----------------------------- FINE FUNZIONI -----------------------------------------------------------------------------
// #####################################################################################################################
