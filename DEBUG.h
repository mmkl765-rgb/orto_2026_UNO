// PRE_PROCESSORE per le fasi di debug e scrittura su monitor seriale. gestito tramite variabile DEBUG
// che se impostata a 1 (true) fa la sostituzione con Serial.print altrimenti fa la sostituzione con
// il nulla pertanto non scrive niente! 
#ifndef LOAD_DEBUG_SERIALE
    #define LOAD_DEBUG_SERIALE
 // #define DEBUG 0
    #define DEBUG 1                              // DEBUG 0 = disabilitato - DEBUG 1 = abilitato
    #if DEBUG                                    // se impostato a 1
      #define prt(x)  Serial.print(F(x))         // sostituisci prt(x) con Serial.print(x)
      #define prtln(x)  Serial.println(F(x))     // sostituisci prtln(x) con Serial.println(x)
      #define prt_v(x)  Serial.print(x)          // come sopra ma per le variabili - senza (F(...))
      #define prtln_v(x)  Serial.println(x)      // come sopra ma per le variabili - senza (F(...))
    #else                                        // se impostato a 0
      #define prt(x)                             // sostituisci prt(x) con nulla, comando vuoto
      #define prtln(x)                           // sostituisci prtln(x) con nulla, comando vuoto
      #define prt_v(x)                           // sostituisci prtln(x) con nulla
      #define prtln_v(x)                         // sostituisci prtln(x) con nulla
    #endif
#endif