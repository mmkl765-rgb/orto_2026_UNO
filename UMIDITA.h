// HEADER DEFINIZIONE DEI VALORI LIMITE PER LA LETTURA DEI DATI UMIDITA' TERRENO
// vengono semplicemente inclusi

// SOGLIA LIMITE VALORE UMIDITA'
// la SOGLIA indica il valore al di sotto del quale parte l'irrigazione automatica
// ricordarsi che i valori sono molto influenzati dai sensori e dalla lunghezza dei cavi,
// fare dei test per il settaggio dei valori grezzi *_RAW il piu oreciso possibile
// eventualmente usare il fattore correzione (accetta solo valori positivi ! )
#define SOGLIARH_Z1            61          // % limire umiditra al di sotto del quale bagna Z1
#define SOGLIARH_Z2            51          // % limire umiditra al di sotto del quale bagna Z2

#define SECCO_RAW_VALUE      900           // valore lettura analogica corrispondente allo 0 % - terreno secco del tutto
#define BAGNATO_RAW_VALUE    200           // valore lettura analogica corrispondente al 100 % - terreno bagnato in ammollo

#define CORREZIONE_Z1   -12                // eventuale fattore sorrezione per zona 1
#define CORREZIONE_Z2   -7                 // eventuale fattore sorrezione per zona 2
