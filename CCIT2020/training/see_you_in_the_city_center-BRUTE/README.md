# Chi non ha testa abbia FLOPS

## Introduzione
Qualche tempo fa mi sono imbattuto in una CTF il cui scopo era abbastanza semplice:
avevi a disposizione un testo in chiaro, il corrispondente cipher in base 64 e l'algoritmo di encrittazione (una specie di vignere via xor).
Avevi a disposizione anche la dimensione della chiave, e la lista di caratteri da cui veniva presa (`string.ascii_lowercase` in python).
La particolarità è che l'encrittazione veniva fatta due volte, utilizzando prima la prima metà della chiave e poi la seconda metà.
La chiave corrispondeva alla flag.
Con il senno di poi la soluzione era fruttare il doppio passaggio per ridurre il numero di esecuzioni da `n^8` a `2(n^4)`, con n numero di caratteri.
Nonostante questo, appena visto il codice non ho resistito a provare semplicemente tutte le possibili combinazioni, e vedere se fosse fattibile.

## Descrizione problema
__Numero chiavi:__ 26 ^ 8 = 208827064576

## STEP 1
Modificare leggermente il testo dato per provare ogni singola combinazione.
__Risultato:__ 41k keys/s

Decisamente non una soluzione decente

## STEP 2
Riscrivere l'algoritmo in C.
E' stato necessario copiare da internet una libreria per codificare il risultato in base 64.
__Risultato:__ 155k keys/s

## STEP 3
Utilizzare la flag -O3 di gcc.
__Risultato:__ --- keys/s

A questo punto ero già soddisfatto, ho partizionato il lavoro sui quattro core del mio pc principale e su 3 core del computer che uso come home server.
In 6h circa ho trovato la chiave.
A questo punto, ho iniziato a pensare "si può fare di meglio?" questa repo è la mia ricerca del meglio.

## STEP 4
Eliminata seconda passata di base64.
__Risultato:__ 6,6M key/s

## STEP 5
Riscritta funzione encrypt per utilizzare il parallelismo AVX2.
Ricordo che stiamo ancora parlando di performance single core.
__Risultato:__ 11M key/s

## STEP 6
Riscritta gran parte del loop, per spremere ogni possibile ciclo di clock.
__Risultato:__ 17M key/s

### How nice?
Ho provato a giocare con la niceness del programma, per vedere l'effetto sulle performance
niceness -20 da i seguenti risultati:
__Risultato:__ ~100k key/s in più

## STEP 7
Copiata da github una libreria per fare base64 velocemente.
Forse posso spremere ancora qualcosa non passando dalla memoria, ma per ora sono abbastanza soddisfatto, visto che trovo la soluzione in circa 10 minuti.
__Risultato:__ 37M key/s

### whoami
timmykill
