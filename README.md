# MPI-Wordcount
Il Wordcount indica il numero di parole in un documento o in un file testuale; viene utilizzato quando è necessario che un testo rimanga entro un numero specifico di parole, ciò accade tipicamente nel mondo accademico, nei procedimenti legali, nel giornalismo e nella pubblicità. Esso viene utilizzato dai traduttore per determinare il prezzo di un lavoro di traduzione; può essere utilizzato anche per calcolare le misure di leggibilità e per misurare la velocità di digitazione e lettura (di solito in parole al minuto). È stata implementata una versione di map-reduce usanto MPI per eseguire il Word Counting su un enorme numero di files.

## Struttura della soluzione
L'implementazione del programma ha previsto la seguente struttura dotata dei seguenti file C:
* [*mpi_chunk.c*](https://github.com/Peppen/Wordscount/tree/main/src/mpi_chunk.c): associato all'header file [*mpi_chunk.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_chunk.h), contiene la funzione *createChunkDatatype(MPI_Datatype \*chunkData)* per la creazione dell'MPI_Datatype chunkData, una funzione *getChunksNumber(counter \*chunkList)* che ritorna il numero di chunks ed una funzione *\*divideLines(file \*fileNames, int fileNumber, int totalLines, int workers)* per la suddivisione delle linee di parole estratte dai file da dividere per ogni processore che ritorna una struttura counter.
   * La stuttura *Counter* contiene un oggetto *file*, due int, uno corrispondente alla *startLine* ed un altro corrispondente all'*endLine* di un file, e un puntatore al prossimo oggetto Counter.
   * La struttura *FileName* contiene un char corrispondente al nome del file ed il numero di linee di quel file. 
* [*mpi_info.c*](https://github.com/Peppen/Wordscount/tree/main/src/mpi_info.c): associato all'header file [*mpi_info.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_info.h), contiene la funzione *time_info(int p, int fileNumber, int totalLines, double time)* che stampa su file le informazioni riguardanti i tempi del Wordcounting.
   * La struttura *Word* contiene un char, corrispondente alla parola analizzata, ed un int corrispondente al numero di occorrenze di quella parola.
* [*mpi_file_information.c*](https://github.com/Peppen/Wordscount/blob/main/src/mpi_file_information.c): associato all'header file [*mpi_file_information.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_file_information.h), contiene una funzione \**getFilesName(file \**fileNames*, char \*dirPath)* che ritorna il numero di file presenti nella directory dei file da esaminare, ed una funzione *getLinesNumber(file \*fileNames, int fileNumber, char \*dirPath)* che ritorna il numero di linee totali ottenuto sommando le linee di ogni file presente nella directory.
* [*mpi_logic.c*](https://github.com/Peppen/Wordscount/blob/main/src/mpi_logic.c): associato all'header file [*mpi_logic.h*](https://github.com/Peppen/Wordscount/blob/main/include/mpi_logic.h), contiene una funzione *createWordDatatype(MPI_Datatype \*wordData)* per la creazione dell'MPI_Datatype wordData, la funzione *toLower(char \*s)* che pone in minuscolo tutte le lettere che compongono l'array in input, la funzione *del(char \*line)* che elimina tutti i caratteri di interpunzione e i caratteri speciali, la funzione *getWordsNumber(occurrence \*words)* che ritorna il numero di parole contenuto in un oggetto occurrence, e la funzione di *count_words(occurrence \**maps, chunk \*chunks, int myChunkNumber, char \*dirPath)* che ritorna il numero di parole sulle linee ricevute da ogni processore.
   * La struttura *Occurrence* contiene un word ed un puntatore alla prossima Occurrence.
   * La struttura *Chunk* contiene un array di char, corrispondente al nome del file, e due int, uno corrispondente alla *startLine* ed un altro corrispondente all'*endLine* del        file.
* [*main.c*](https://github.com/Peppen/Wordscount/blob/main/src/main.c): contiene il main di esecuzione dell'algoritmo. Il processo MASTER legge tutti i file contenuti nella directory [*files*](https://github.com/Peppen/Wordscount/tree/main/files) e calcola il numero totale di linee da inviare ad ogni processore sotto forma di chunk. Se il numero di processori è maggiore di uno, quindi non siamo in un'esecuzione sequenziale, si esegue una MPI_Scatter() ed in seguito una MPI_Scatterv(); una volta inviati i chunks ad ogni processore, ognuno di loro può eseguire il Word Counting. I dati raccolti da ogni processore possono essere uniti tramite la chiamata di una MPI_Gather() ed in seguito di una MPI_Gatherv(). Per il calcolo dei tempi di esecuzione dell'algoritmo, tramite una MPI_Reduce vengono sommati i tempi di esecuzione di ogni processore ed in seguito divisi per il numero di processori totali.

## Implementazione
* La soluzione sequenziale è banale: un solo processore legge tutti i file presenti nella directory da analizzare ed esegue il wordcounting di ognuno.
* Per quanto riguarda la soluzione sequenziale, sono state necessarie tre fasi:
    *  Il nodo MASTER legge l'elenco dei file presenti in una directory, esso è l'unico nodo a leggere l'elenco dei file. In seguito, di ogni file vengono calcolate le linee da cui è      composto in modo da dividerle per ogni processo in modo tale che il MASTER possa inviare ai processi dei chunks, strutture dotate del nome del file e dell'inizio e la fine delle      linee su cui oguno dei processi deve lavorare; il numero totale di linee ricevute da ogni processo sarà dato da questo stesso numero diviso il numero di processori totali, il        tutto racchiuso in una serie di chunks. Il numero di chunks che ogni processo dovrebbe ricevere viene inviato tramite una *Mpi_Scatter()* ed essi vengono inviati tramite una          *Mpi_Scatterv()*. 
    *  Durante la seconda fase ogni processo eseguirà il Word Counting delle sue linee ed alla fine produrrà una mappa locale.
    *  Nell'ultima fase ogni processo invia al master la dimensione di ogni mappa che dovrebbe aspettarsi con una *MPI_Gather()*; ogni mappa verrà inviata poi con una                        *MPI_Gatherv()*. Il Master alla fine ha solo bisogno di raccogliere tutte queste informazioni e salvarle in un file testuale(.txt) con le parole e le frequenze ordinate; i            tempi di esecuzione verranno salvati in seguito in un secondo file testuale(.txt).

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
* L'asse X rappresenta il numero di processi utilizzati.
* L'asse Y rappresenta il tempo di esecuzione calcolato in millisecondi(ms).

### Strong Scalability
![Strong Scalability](https://github.com/Peppen/Wordscount/blob/main/figures/Strong_Scalability.png)
Il grafo mostra come i tempi di esecuzione diminuiscono all'aumentare del numero di processori stabilizzandosi su una media che varia in base alla dimensione dei file sottoposti ai test. <br>
Il comportamento descescente è corretto dato che mantenere fisso l'input implica che ogni processo riceverà meno lavoro all'aumentare del numero di processi totali.
![SpeedUp](https://github.com/Peppen/Wordscount/blob/main/figures/SpeedUp.png)
Questo grafo mostra come le performance migliorano all'aumentare della size dell'input ed all'aumentare del numero di processori. Si nota come, dati tutte le dimensioni degli input, lo speed sia super lineare, aspetto che si nota man mano che la dimensione degli input cresce.

### Weak Scalability
![Weak_Scalability](https://github.com/Peppen/Wordscount/blob/main/figures/Weak_Scalability.png)
Il grafo mostra un buon comportamento dell'algoritmo; infatti i tempi di esecuzione non variano molto tra i processori essendo che ricevono tutti la stessa mole di lavoro. <br>
