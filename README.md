# MPI-Wordcount
Il Wordcount indica il numero di parole in un documento o in un file testuale; viene utilizzato quando è necessario che un testo rimanga entro un numero specifico di parole, ciò accade tipicamente nel mondo accademico, nei procedimenti legali, nel giornalismo e nella pubblicità. Esso viene utilizzato dai traduttore per determinare il prezzo di un lavoro di traduzione; può essere utilizzato anche per calcolare le misure di leggibilità e per misurare la velocità di digitazione e lettura (di solito in parole al minuto). È stata implementata una versione di map-reduce usanto MPI per eseguire il Word Counting su un enorme numero di files.

## Soluzione
Tre fasi sono stati necessarie per questa implementazione:
- Nella prima fase il nodo MASTER legge l'elenco dei file (o una directory), che conterrà i nomi di tutti i file da contare; in questo caso MASTER è l'unico nodo a leggere l'elenco dei file. Di ogni file vengono calcolate le linee da cui è composto in modo da dividerle per ogni processo; in seguito il MASTER invia ai processi chunks, strutture dotate del nome del file e dell'inizio e la fine delle linee su cui deve lavorare; quindi ogni processo riceve un numero totale di linee dato da questo stesso numero diviso il numero di processori totali, il tutto racchiuso in una serie di chunks. Ogni processo in seguito eseguirà il Word Counting delle sue linee ed alla fine produrrà un istogramma locale.
- Durante la seconda fase si combinano le frequenze delle parole attraverso i processi.
- Nell'ultima fase ciascuno dei processi invia i propri istogrammi locali al processo MASTER che ha solo bisogno di raccogliere tutte queste informazioni. In seguito il MASTER salva tutte queste informazioni in un file testuale(.txt) con le parole e le frequenze ordinate, e salva i tempi di esecuzione in un file testuale(.txt).

## Implementazione
Per questa implementazione sono state necessarie i seguenti file C:
- [*mpi_chunk.c*](https://github.com/Peppen/Wordscount/tree/main/src/mpi_chunk.c): associato all'header file [*mpi_chunk.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_chunk.h), contiene la funzione *createChunkDatatype(MPI_Datatype \*chunkData)* per la creazione dell'MPI_Datatype chunkData, una funzione *getChunksNumber(counter \*chunkList)* che ritorna il numero di chunks, una funzione *\*divideLines(file \*fileNames, int fileNumber, int totalLines, int workers)* di spartizione delle linee di parole estratte dai file da dividere per ogni processore; essa ritorna un struttura counter contenente il nome del file, l'inizio e la fine della linee su cui deve lavorare il processore ed un puntatore al prossimo counter; l'ultima funzione è *\*createChunkList(int chunksNumber, counter \*count)* ed una che converte la lista di chunks in un array.
- [*mpi_info.c*](https://github.com/Peppen/Wordscount/tree/main/src/mpi_info.c): associato all'header file [*mpi_info.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_info.h), contiene la funzione *wordcount_info(word \*maps, int words)* che stampa su file le informazioni riguardanti tutte le occorrenze delle parole analizzate, e la funzione *time_info(int p, int fileNumber, int totalLines, double time)* che stampa su file le informazioni riguardanti i tempi del Wordcounting.
- [*mpi_file_information.c*](https://github.com/Peppen/Wordscount/blob/main/src/mpi_file_information.c): associato all'header file [*mpi_file_information.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_file_information.h), contiene una funzione *getFilesName(file \**fileNames, char \*dirPath)* che ritorna il numero di file presenti nella directory dei file da esaminare, ed una funzione *getLinesNumber(file \*fileNames, int fileNumber, char \*dirPath)* che ritorna il numero di linee totali ottenuta sommando le linee di ogni file presente nella directory.
- [*mpi_helper.c*](https://github.com/Peppen/Wordscount/blob/main/src/mpi_helper.c): associato all'header file [*mpi_helper.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_helper.h), contiene una funzione *toSend(int \*sendCounts, chunk \*list, int size, int totalLines, int p)* che indica le linee da inviare ad ogni processore, incluso il master.
- [*mpi_logic.c*](https://github.com/Peppen/Wordscount/blob/main/src/mpi_logic.c): associato all'header file [*mpi_logic.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_logic.h), contiene una funzione *createWordDatatype(MPI_Datatype \*wordData)* per la creazione dell'MPI_Datatype wordData, la funzione *toLower(char \*s)* che pone in minuscolo tutte le lettere che compongono l'array in input, la funzione *del(char *line)* che elimina tutti i caratteri di interpunzione e i caratteri speciali, la funzione *getWordsNumber(occurrence \*words)* che ritorna il numero di parole contenuto in un oggetto occurrence, e la funzione di *count_words(occurrence \**maps, chunk \*chunks, int myChunkNumber, char \*dirPath)* ritorna il numero di parole sulle linee ricevute da ogni processore.
- [*main.c*](https://github.com/Peppen/Wordscount/blob/main/src/main.c): contiene il main di esecuzione dell'algoritmo. Il processo MASTER legge tutti i file contenuti nella directory [*files*](https://github.com/Peppen/Wordscount/tree/main/files) e calcola il numero totale di linee da inviare ad ogni processore sotto forma di chunk. Se il numero di processori è maggiore di uno, quindi non siamo in un'esecuzione sequenziale, si esegue una MPI_Scatter() ed in seguito una MPI_Scatterv(); una volta inviati i chunks ad ogni processore, ognuno di loro può eseguire il Word Counting. I dati raccolti da ogni processore possono essere uniti tramite la chiamata di una MPI_Gather() ed in seguito di una MPI_Gatherv(). Per il calcolo dei tempi di esecuzione dell'algoritmo, tramite una MPI_Reduce vengono sommati i tempi di esecuzione di ogni processore ed in seguito divisi per il numero di processori totali.
## Esecuzione
Sia per la creazione dell'eseguibile sia per l'esecuzione del programma è stato creato un [*Makefile*](https://github.com/Peppen/Wordscount/blob/main/src/Makefile):
- Eseguendo il comando `make` viene creato l'eseguibile sotto il nome di [**wordscount**](https://github.com/Peppen/Wordscount/blob/main/src/wordscount),
- Eseguendo il comando `make run` in seguito verrà chiamato il comando `mpirun -np 2 ./main`.

Per eseguire il programma in maniera sequenziale basterà eseguire il comando `mpirun -np 1 ./main` mentre per eseguirlo con un numero X di processori il basterà eseguire il comando `mpirun -np X --oversubscribe ./main`.

Per eseguire il programma tramite un cluster AWS di istanze EC2:
* Creare il numero di istanze EC2 con il quale si vuole eseguire l'algoritmo
  * Scegliere l'AMI Ubuntu Server 18.04 
  * Selezionare il tipo di istanza, nel nostro caso è stata utilizzata *m4.xlarge*
  * Configurare il gruppo di sicurezza utilizzando una regola TCP personalizzata il cui intervallo delle porte è 0-65535
  * Selezionare la stessa copia di chiavi private per ogni istanza
* Tramite il comando `scp -i "keyspair.pem" keyspair.pem ubuntu@masterIP` passare la chiave privata al MASTER NODE dove masterIP coincide con il DNS IPv4 pubblico dell'istanza
* Tramite il comando `ssh -i "keyspair.pem" ubuntu@masterIP` effettuare il login sul nodo MASTER 
* Installare su ogni istanza l'ambiente Ubuntu 18.04
* Dopo l'installazione di Ubuntu, eseguire l'installazione di OpenMPI 4.0
* Tramite il MASTER installare sui nodi Slave sia l'ambiente Ubuntu 18.04 sia OpenMPI 4.0
* Inviare al MASTER node scelto l'eseguibile *wordscount* ed i file sul quale si vuole eseguire il Word Counting
  * Nel nostro caso è stata inviata la cartella [*files*](https://github.com/Peppen/Wordscount/tree/main/files) contenente tutti i file su cui è stato testato lo script
* Copiare i files su pcpc@MASTER dove MASTER è l'indirizzo IP privato del nodo MASTER
* Da pcpc@MASTER bisognerà inviare sia l'eseguibile sia i file ad ogni slave tramite il comando `scp FILE_NAME pcpc@masterIP`
* Da pcpc@MASTER basterà ora eseguire il comando `*mpirun -np X --hostfile hostfile ./main*` dove X è il numero di processori che si vogliono utilizzare

Il file *hostfile* è un file con la seguente struttura:
```
  NODE slots=4 max_slots=4
  NODE slots=4 max_slots=4
  NODE slots=4 max_slots=4
  NODE slots=4 max_slots=4
```
A NODE corrisponde l'indirizzo IP privato di ogni istanza EC2 di AWS.

## Correttezza
L'algoritmo esegue il Word Counting correttamente salvando tutte le informazioni in un file [*data.txt*]((https://github.com/Peppen/Wordscount/blob/main/output/data.txt)) nella cartella [*output*](https://github.com/Peppen/Wordscount/blob/main/output) mentre tutte le informazioni riguardanti i tempi di esecuzione vengono salvati nel file *times.txt* nella cartella [*times*](https://github.com/Peppen/Wordscount/blob/main/times). L'algoritmo quando trova una nuova parola la inizializza a 0 mentre quando la parola è già presente nell'istogramma incrementa la sua occorrenza di 1. In seguito l'istogramma verrà stampato e salvato su file. Per quanto riguarda i tempi di esecuzione, prima della lettura dei file nella directory *files*, viene inizializzata la variabile *startTime* a MPI_Time(), alla fine della MPI_Gather() viene poi calcolato lo *stopTime* sottraendo *startTime* ad MPI_Time(); i tempi vengono poi salvati nel file [*times.txt*](https://github.com/Peppen/Wordscount/blob/main/times/times.txt).  <br>

Il file [*data.txt*](https://github.com/Peppen/Wordscount/blob/main/output/data.txt) ha la seguente struttura:
```
  *************************************
  ***Collected Data after WordsCount***
  Word:      And   -> Occurrence: 12735
  Word:      earth -> Occurrence:   328
  Word:      was   -> Occurrence:  4448
  Word:      with  -> Occurrence:   385
  *************************************
```
Il file [*times.txt*](https://github.com/Peppen/Wordscount/blob/main/times/times.txt) ha la seguente struttura:
```
  *********************************************************
  2 Process(es) Working on 8 File(s) Containing 25000 Lines
  Average Global Time for Wordcounting ->  35.67894 Seconds
  *********************************************************
```
Viene calcolato il tempo di esecuzione globale per il Word Counting eseguendo una MPI_Reduce() ed in seguito viene diviso per il numero p di processori di cui si è fatto utilizzo.

## Benchmarks
L'algoritmo è stato testato attraverso 4 istanze AWS EC2 *m4.xlarge*; i files in input all'algoritmo sono presenti nella cartella *files* e sono stati utilizzati per testare le performance valutandole in termini di: 
* *Strong Scalability*: la dimensione del problema rimane fissa ma aumenta il numero di elementi di elaborazione.
* *Weak Scalability*: la dimensione del problema (carico di lavoro) assegnata a ciascun elemento di elaborazione rimane costante e vengono utilizzati elementi aggiuntivi per risolvere un problema più ampio.

<br> Ogni test è stato eseguito 10 volte per ogni possibile combinazione in input di grandezza e numero di processori; i risultati sono stati rappresentati sotto forma di grafo.

### Strong Scalability
![Strong Scalability](https://github.com/Peppen/Wordscount/blob/main/figures/Strong_Scalability.png)
Il grafo mostra come i tempi di esecuzione diminuiscono all'aumentare del numero di processori stabilizzandosi su una media che varia in base alla dimensione dei file sottoposti ai test. <br>
Quando la dimensione dell'input aumenta, si arriva ad un picco di miglioramento del tempo di esecuzione del 38%, ottenuto utilizzando 16 processori mentre con il passaggio dall'esecuzione sequenziale all'esecuzione parallela(con soli 2 processori) si ottiene un miglioramento del 33.5%.
![Efficiency_Strong](https://github.com/Peppen/Wordscount/blob/main/figures/Efficiency_Strong.png)
Il grafo mostra come l'efficienza tende ad decrescere all'aumentare del numero dei processori ma decresce più lentamente quando la dimensione dei file cresce; si nota come su file di piccola dimensione(100KB) decresce più velocemente rispetto a file di dimensione maggiore(250KB, 500KB e 750KB) mentre meno velocemente ma non troppo nei file di dimensione 1MB.
![SpeedUp](https://github.com/Peppen/Wordscount/blob/main/figures/SpeedUp.png)
Questo grafo mostra come le performance migliorano all'aumentare della size dell'input ed all'aumentare del numero di processori. Si nota come, dati tutte le dimensioni degli input, lo speed sia super lineare, aspetto che si nota man mano che la dimensione degli input cresce.

## Weak Scalability
![Weak_Scalability](https://github.com/Peppen/Wordscount/blob/main/figures/Weak_Scalability.png)
Il grafo mostra un buon comportamento dell'algoritmo; infatti i tempi di esecuzione non variano molto tra i processori essendo che ricevono tutti la stessa mole di lavoro. <br>
![Efficiency_Weak](https://github.com/Peppen/Wordscount/blob/main/figures/Efficiency_Weak.png)
Anche per quanto riguarda l'efficienza l'algoritmo ha un buon comportamento nonostante per diversi input abbia dei picchi improvvisi.
