// HEADER DEFINIZIONE LINEARE PIN in file separato per limitare la lunghezza del file principale
// il file .h viene semplicemente incluso nel mainfile

// sintassi : define Nome_Pin Numero_Pin
 // #define PIN0              0        // non usare, collegamento usb
 // #define PIN1              1        // non usare, collegamento usb
    #define PULSANTE          2       // bottone manuale
    #define ALIM_SENS         3       // pin dal quale esce l'alimentaizone ai sensori terreno e acqua via 2N2222
    #define LED_B             4       // collegamento al led BLU   = ACQUA
    #define LED_V             5       // collegamento al led VERDE = TUTTO OK
    #define LED_R             6       // collegamento al led ROSSO = ERRORE - QUALCOSA NON VA
    #define ATTIVA_SCHEDA     7       // alimentazione scheda gestione rele tramite 2n2222
    #define LIVELLO_H2O       8       // interruttore riserva acqua, chiuso con acqua, aperto senza
    #define VALVOLA_Z1        9       // rele valvola zona 1
    #define VALVOLA_Z2        10      // rele valvola zona 2
    #define RELE_COMUNE       11      // rele comune valvole
    #define RELE_POMPA        12      // attivazione pompa acqua - da usare alimentazione come attivazione rele grande
//  #define pin13             13      
//  #define pin 14            14      // -- A0 --
// invertiti i numeri, fili collegati invertiti...
    #define S_ZONA2           15      // -- A1 -- sensore zona 1
    #define S_ZONA1           16      // -- A2 -- sensore zona 2
//  #define                   17      // -- A3 -- partitiore per il controllo illuminazione
//  #define PIN18             18      // -- A4 -- ----> possibili pin per Software Serial <----  (A4 => SDA schermo !!)
//  #define PIN19             19      // -- A5 -- ----> possibili pin per Software Serial <----  (A5 => SCL schermo !!)
