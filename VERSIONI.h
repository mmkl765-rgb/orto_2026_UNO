/*

FILE PURAMENTE TESTUALE che tengo come storico delle versioni precedenti
il software di partenza è quello della versione 2025 con tutti i suoi pro e contro.
ho tentato di inserire nel sistema versione 2026 l'interazione con le radio l'ora ma può dei problemi di comunicazione fra have software serial
interazione sicuramente da rivedere perché non voglio buttare il progetto in quanto ho già scritto tutto il protocollo di comunicazione


060526 => sistema funzionante completo, rimodulazione tempi di irrigazione (z2 = 2min // z1 = 1,5 min)
          Lo sketch usa 17740 byte (42%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 657 byte (33%) di memoria dinamica, lasciando altri 1537 byte liberi per le variabili locali. Il massimo è 2048 byte.

070526 => migliorato gestione avvio con   ottimizzazione buffer scrittura
          Lo sketch usa 13548 byte (42%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 522 byte (27%) di memoria dinamica, lasciando altri 1537 byte liberi per le variabili locali. Il massimo è 2048 byte.

080526 => migliorata la gestione memoria con l'uso completo di messaggi in flash
          Lo sketch usa 12754 byte (39%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 511 byte (24%) di memoria dinamica, lasciando altri 1537 byte liberi per le variabili locali. Il massimo è 2048 byte.

090526 => riconfigurata la parte dello show dati, da migliorare ancora se mi viene qualche idea tipo testo scorrevole..
          effettuate sistemazioni codice in loco (livello acqua che non andava e tempi irrigazione)  --  VERSIONE STABILE 1  --
          Lo sketch usa 13168 byte (40%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 528 byte (25%) di memoria dinamica, lasciando altri 1520 byte liberi per le variabili locali. Il massimo è 2048 byte.
          La stessa versione è stata rinominata "ORTO_16_2026_v1-backup" in modo da non toccarla

130526 => risistemazione codice vedendo di rosicchiare ancora qualcosana ma dalla compilazione... rosicchiato nulla !!!
          Lo sketch usa 13010 byte (40%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 528 byte (25%) di memoria dinamica, lasciando altri 1520 byte liberi per le variabili locali. Il massimo è 2048 byte.
          INSTALLATA IL 13 / 05 / 26

220526 => modificato i dati di settaggio umidita come richiesto da papa' in quanto sembra bagni poco.... impostato un fattore di correzione a -10 punti
          sul valore medio di umidita
          modificati i tempi di irrigazione, aumentati di 30 secondi entrambi
          Lo sketch usa 13010 byte (40%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 528 byte (25%) di memoria dinamica, lasciando altri 1520 byte liberi per le variabili locali. Il massimo è 2048 byte.\

250526 => modifica scritta display nella modalita ordinaria aggiunto il numero delle irrigazioni. comodo perche l'acqua va giu un po velocemente in cisterna !!!
          modifica della visualizzazione dello storico delle letture con tabella (sperando sia leggibile e comprensibile)
          modifica di parecchi messaggi lcd con "parole" piu appropriate
          modifica grafica della pagina di visualizzazione del sistema in automatico (pagina piu leggibile)
          Lo sketch usa 12712 byte (39%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 528 byte (25%) di memoria dinamica, lasciando altri 1520 byte liberi per le variabili locali. Il massimo è 2048 byte.

260526 => migliorato il warning visivo nel caso la cisterna sia vuota con introduzione del lampeggio dei led verde - blu intermittente - salta maggiormente
          all'occhio la lampadina che lampeggia rispetto ad una accesa fissa !!!!!
          Lo sketch usa 12726 byte (39%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 528 byte (25%) di memoria dinamica, lasciando altri 1520 byte liberi per le variabili locali. Il massimo è 2048 byte.

280526 => modificato la tempistica irrigazione (aumentato il tempo) e messo valori dsoglia differenti per zona e fatta inclusione per variabile "versione"
          Lo sketch usa 12718 byte (39%) dello spazio disponibile per i programmi. Il massimo è 32256 byte.
          Le variabili globali usano 528 byte (25%) di memoria dinamica, lasciando altri 1520 byte liberi per le variabili locali. Il massimo è 2048 byte.


*/

// VERSIONE OPERATIVA SOFTWARE : per maggiori info leggere le informazioni riportate sopra 
char VERSIONE[7] = "280526"; // FORMATTAZIONE : ggmmaa 
