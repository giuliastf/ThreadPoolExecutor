/* IMBREA Giulia-Stefania - 311CB */ 
#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#define VF(a)(((ASt)(a)) -> vf)
#define DIMS(a)(((ASt)(a)) -> dime)
#define DIMQ(q)(((AQ)(q)) -> dime)
#define START(q)(((AQ)(q)) -> start)
#define END(q)(((AQ)(q)) -> end)
#define dimTEXT 1000
/*--------------------TLG----------------------------------------------------*/
typedef struct celulag {
    void * info; //adresa informatie 
    struct celulag * urm; //adresa urmatoarei celule 
}
TCelulaG, * TLG;

TLG alocaCelulaG(void * x) {
    TLG aux;
    aux = (TLG) malloc(sizeof(TCelulaG));
    if (!aux)
        return NULL;
    aux -> info = x;
    aux -> urm = NULL;
    return aux;
}

void DistrugeLG(TLG * aL) {
    while ( * aL != NULL) {
        TLG aux = * aL; // adresa celulei eliminate
        if (!aux)
            return;
        free(aux -> info); // eliberare spatiu ocupat de element
        * aL = aux -> urm; // deconecteaza celula din lista
        free(aux); // elibereaza spatiul ocupat de celula
    }
}

/*-----------------------------structuri-------------------------------------*/
typedef struct {
    int ID; //este atribuit automat unui task atunci cand este mutat in run
    int timp_ramas; //timpul_ramas pana la terminarea executiei unui task
}
Thread;

typedef struct {
    unsigned short ID; // este atribuit automat la crearea unui task
    Thread fir;
    unsigned char prioritate;
    int timp_exec;
}
Task;

/*--------------------------------stiva--------------------------------------*/
typedef struct stiva {
    size_t dime; //dim.element
    TLG vf; // adresa celulei din varf
}
TStiva, * ASt;

void * InitS(size_t d) //Initializare stiva vida
{
    ASt s;
    s = (ASt) malloc(sizeof(TStiva));
    if (!s) return NULL;
    DIMS(s) = d;
    VF(s) = NULL;
    return (void * ) s;
}

int Push(void * s, void * el) { // Inserare *el in stiva
    TLG aux = alocaCelulaG(el);
    if (!aux)
        return 0;
    aux -> urm = VF(s);
    VF(s) = aux;
    return 1;
}

int Pop(void * s, void * el) { // Scoatere din stiva in el
    if (VF(s) == NULL)
        return 0;
    memcpy(el, VF(s) -> info, DIMS(s));
    TLG aux = VF(s);
    VF(s) = aux -> urm;
    free(aux -> info);
    free(aux);
    return 1;
}

void DistrStiva(void * s) {
    TLG aux = VF(s);
    DistrugeLG( & aux);
    VF(s) = NULL;
    free(s);
}

int ConcatS(void * dest, void * src) { // muta toate elem. din src in dest 
    if (DIMS(src) != DIMS(dest))
        return -1;
    if (VF(src) == NULL)
        return 0;
    while (VF(src) != NULL) {
        void * el = malloc(DIMS(src));
        Pop(src, el);
        Push(dest, el);
    }
    return 1;
}

int PushsortInt(void * s, void * el) { // inserare in stID, respectand sortarea
    if (VF(s) == NULL) // stiva s este goala
        Push(s, el); // nu mai trebuie tinut cont de nicio sortare
    else {
        ASt temp = InitS(DIMS(s));
        int ok = 0; // este 0 daca nu s-a inserat el, 1 altfel
        while (VF(s) != NULL) { //cat timp am ce extrage din stiva s
            void * extr = malloc(DIMS(s));
            Pop(s, extr);
            if ( * ((unsigned short * ) extr) < * ((unsigned short * ) el))
                Push(temp, extr);
            else {
                Push(temp, el);
                Push(temp, extr);
                ConcatS(s, temp);
                ok = 1;
                break;
            }
        }
        if (ok == 0) {
            Push(temp, el); //este ultimul elem
            ConcatS(s, temp); //repar s ul
        }
        DistrStiva(temp); //asta nu mai trb
    }
    return 1;
}

/*-------------------------------cozi----------------------------------------*/
typedef struct coada {
    size_t dime; // dim.element
    TLG start, end; // adr.prima, ultima celula
}
TCoada, * AQ;

void * InitQ(size_t d) { //Initializare coada vida
    AQ q;
    q = (AQ) malloc(sizeof(TCoada));
    if (!q)
        return NULL;
    START(q) = NULL;
    END(q) = NULL;
    DIMQ(q) = d;
    return (void * ) q;
}

int ExtrQ(void * q, void * el) { //Extragere element din coada in el
    TLG aux = START(q);
    if (START(q) == NULL) //coada este goala, nu am ce extrage
        return 0;
    aux = START(q);
    START(q) = aux -> urm;
    memcpy(el, aux -> info, DIMQ(q));
    free(aux -> info);
    free(aux);
    if (START(q) == NULL) //coada AVEA un element
        END(q) = NULL;
    return 1;
}

void DistrQ(void * q) { // Distrugere coada
    TLG aux = START(q);
    START(q) = NULL;
    END(q) = NULL;
    DistrugeLG( & aux);
    free(q);
}

int InsertQ(void * q, void * el) { // Insereaza pe *el in coada
    TLG aux = alocaCelulaG(el);
    if (!aux)
        return 0;
    if (START(q) == NULL) { //Inserare in coada vida
        START(q) = END(q) = aux;
    } else { //Inserare in coada nevida
        END(q) -> urm = aux;
        END(q) = aux;
    }
    return 1;
}

int ConcatQ(void * dest, void * src) { // muta toate elementele din src in dest
    if (DIMQ(src) != DIMQ(dest)) // elem. din src si din dest sunt diferite
        return -1;
    if (START(src) == NULL) // sursa este vida
        return 0;
    while (START(src) != NULL) {
        void * el = malloc(sizeof(Task));
        ExtrQ(src, el); //scot din sursa
        InsertQ(dest, el); //pun in destinatie
    }
    return 1;
}

int InsertQsort(void * q, void * el) { // Inserare sortata in run, wait, finish
    int ok = 0; // verifica daca el a fost inserat
    if (START(q) == NULL) { //Inserare in coada vida
        InsertQ(q, el);
    } else { //Inserare in coada nevida
        AQ temp = InitQ(DIMQ(q));
        while (START(q) != NULL) {
            void * extr = malloc(DIMQ(q));
            ExtrQ(q, extr);
            if (((Task * )(extr)) -> prioritate > ((Task * )(el)) -> prioritate)
                InsertQ(temp, extr); 
            if (((Task * )(extr)) -> prioritate < ((Task * )(el)) -> prioritate) {
                InsertQ(temp, el); // inserez el 
                InsertQ(temp, extr); // inserez la loc elementul extras
                ConcatQ(temp, q); // inserez restul cozii
                ok = 1; // am inserat el
                break;
            }
            if (((Task * )(extr)) -> prioritate == ((Task * )(el)) -> prioritate) { 
                if (((Task * )(extr)) -> timp_exec < ((Task * )(el)) -> timp_exec)
                    InsertQ(temp, extr);
                if (((Task * )(extr)) -> timp_exec > ((Task * )(el)) -> timp_exec) {
                    InsertQ(temp, el); // inserez el
                    InsertQ(temp, extr); // inserez la loc elementul extras
                    ConcatQ(temp, q); //  inserez restul cozii
                    ok = 1; // am inserat el
                    break;
                }
                if (((Task * )(extr)) -> timp_exec == ((Task * )(el)) -> timp_exec) { 
                    if (((Task * )(extr)) -> ID < ((Task * )(el)) -> ID)
                        InsertQ(temp, extr);
                    else {
                        InsertQ(temp, el); //inserez el
                        InsertQ(temp, extr); // inserez la loc elementul extras
                        ConcatQ(temp, q); //inserez restul cozii
                        ok = 1; // am inserat el
                        break;
                    }
                }
            }
        }
        if (ok == 0) //el nu a fost inserat =>trb inserat la sf
            InsertQ(temp, el);
        ConcatQ(q, temp);
        DistrQ(temp);
    }
    return 1;
}

/*-----------------------------alte-functii-utile ---------------------------*/
ASt THpool(int N) { //creeaza o Stiva de thread-uri
    ASt s = InitS(sizeof(Thread));
    int i;
    for (i = N - 1; i >= 0; i--) {
        Thread * th = (Thread * ) malloc(sizeof(Thread));
        th -> ID = i;
        th -> timp_ramas = 0;
        Push((void * ) s, (void * ) th);
    }
    return s;
}

int stiva_ID(ASt s) { //creaza o stiva de ID-uri
    unsigned short i;
    for (i = 32767; i >= 1; i--) {
        unsigned short * aux = malloc(sizeof(unsigned short));
        * aux = i;
        Push((void * ) s, (void * ) aux);
    }
    return 1;
}

int Max(int a, int b) {
    if (a < b)
        return 1;
    else return 0;
}

int Min(int a, int b) {
    if (a > b)
        return 1;
    else return 0;
}

int timpMaxMin(void * q, int fcmp(int, int)) { // det. timpul maxim/minim ramas din run
    AQ temp = InitQ(sizeof(Task));
    int nr = 0;
    while (START(q) != NULL) {
        void * el = malloc(DIMQ(q));
        ExtrQ(q, el);
        if (nr == 0)
            nr = ((Task * ) el) -> fir.timp_ramas;
        if (fcmp(nr, ((Task * ) el) -> fir.timp_ramas) == 1)
            nr = ((Task * ) el) -> fir.timp_ramas;
        InsertQ(temp, el);
    }
    ConcatQ(q, temp);
    DistrQ(temp);
    return nr;
}

void Time(int dif, void * run, void * finish, ASt pool, ASt stID) {
    AQ temp = InitQ(DIMQ(run));
    while (START(run) != NULL) {
        void * el = malloc(DIMQ(run));
        ExtrQ(run, el);
        ((Task * ) el) -> fir.timp_ramas -= dif; // 1) scade timpul ramas din fiecare task din run
        if (((Task * ) el) -> fir.timp_ramas <= 0) { // 2) verifica daca vreun task s a terminat
            Thread * newth = malloc(sizeof(Thread));
            * newth = (((Task * ) el) -> fir);
            unsigned short * newID = malloc(sizeof(unsigned short));
            * newID = (((Task * ) el) -> ID);
            Push(pool, newth); // 3) baga threadul la loc
            PushsortInt(stID, newID); // 4) baga ID ul task-ului la loc
            InsertQ(finish, el); // il duc in finished
        } else
            InsertQ(temp, el);
    }
    ConcatQ(run, temp);
    DistrQ(temp);
}

void * Run(int * time, int T, int Q, void * run, void * wait, void * finish, ASt pool, ASt stID) {
    if (T <= 0)// folosita cu comanda run, functia incepe procesul de scurgere a timpului
        return NULL;
    if (START(wait) == NULL && START(run) == NULL)
        return NULL;
    Thread * th = malloc(sizeof(Thread));
    while (START(wait) != NULL && VF(pool) != NULL) { //verific daca mai pot insera elemente in run
        void * el = malloc(DIMQ(run));
        ExtrQ(wait, el);
        Pop(pool, th); //atribui un thread task-ului
        ((Task * ) el) -> fir = * th;
        ((Task * ) el) -> fir.timp_ramas = ((Task * ) el) -> timp_exec; //initializez timpul ramas
        InsertQsort(run, el);
    }
    int max = timpMaxMin(run, Max);
    if (T < Q) {
        if (max < T) {
            * time = * time + max;
            Time(max, run, finish, pool, stID); //scazatorul este max
            Run(time, T - max, Q, run, wait, finish, pool, stID); //reapelez functia, cu T = T-max
        } else {
            * time = * time + T;//scazatorul este T
            Time(T, run, finish, pool, stID);//nu mai este necesara reapelarea functiei (T=0) 
        }
    }
    if (T > Q) {
        if (max < Q) {
            * time = * time + max;
            Time(max, run, finish, pool, stID); //scazatorul este max
            Run(time, T - max, Q, run, wait, finish, pool, stID); //reapelez functia, cu T = T-max
        } else {
            * time = * time + Q;
            Time(Q, run, finish, pool, stID); //scazatorul este Q
            Run(time, T - Q, Q, run, wait, finish, pool, stID); //reapelez functia, cu T = T-Q
        }
    }
    if (T == Q) {
        * time = * time + T;
        Time(T, run, finish, pool, stID);//nu mai este necesara reapelarea functiei (T=0) 
    }
    free(th);
    return NULL;
}

void * Finish(int * time, int Q, void * run, void * wait, void * finish, ASt pool, ASt stID) {
    if (START(wait) == NULL && START(run) == NULL) { // nu mai am ce executa
        return NULL;
    }
    Thread * th = malloc(sizeof(Thread));
    while (START(wait) != NULL && VF(pool) != NULL) { //verific daca mai pot insera elemente run
        void * el = malloc(DIMQ(run));
        ExtrQ(wait, el);
        Pop(pool, th); //atribui un thread task-ului
        ((Task * ) el) -> fir = * th;
        ((Task * ) el) -> fir.timp_ramas = ((Task * ) el) -> timp_exec; //initializez timpul ramas
        InsertQsort(run, el);
    }
    free(th);
    int min = timpMaxMin(run, Min);
    if (min < Q) {
        Time(min, run, finish, pool, stID);
        * time = * time + min;
        Finish(time, Q, run, wait, finish, pool, stID);
    } else {
        Time(Q, run, finish, pool, stID);
        * time = * time + Q;
        Finish(time, Q, run, wait, finish, pool, stID);
    }
    return NULL;
}

/*------------------------------------------main-----------------------------*/
int main(int argc, char ** argv) {
    FILE * in = fopen(argv[1], "r"); //argv[0] = ./tema1; argv[1] = fisier.txt
    if ( in == NULL)
        exit(EXIT_FAILURE);
    FILE * out = fopen(argv[2], "w");
    if (out == NULL)
        exit(EXIT_FAILURE);
    int T; // timpul dat ca parametru de comanda run.
    unsigned char prioritate;
    int Q; //cuanta de timp, timpul maxim de rulare continuă a unui thread
    char C; //numărul de core-uri disponibile
    char * line = (char * ) malloc(dimTEXT * sizeof(char));
    char * cuv;
    int nr_tasks;
    int timp_exec;
    unsigned short task_ID;
    int i, ok, thread_id;
    int time = 0; //timpul de executie al task urilor
    int * time_ptr = & time;
    fscanf( in , "%d", & Q);
    fscanf( in , "%d", (int * )( & C));
    unsigned char N = 2 * C; // nr max de threaduri ce pot rula in paralel
    ASt stID = InitS(sizeof(unsigned short));
    stiva_ID(stID);
    if (!stID)
        return -1;
    ASt pool = THpool(N); // creez pool-ul de thread-uri available in fct de N
    if (!pool) {
        DistrStiva(stID);
        return -1;
    }
    AQ wait = InitQ(sizeof(Task));
    if (!wait) {
        DistrStiva(stID);
        DistrStiva(pool);
        return -1;
    }
    AQ run = InitQ(sizeof(Task));
    if (!run) {
        DistrStiva(stID);
        DistrStiva(pool);
        DistrQ(wait);
        return -1;
    }
    AQ finish = InitQ(sizeof(Task));
    if (!finish) {
        DistrStiva(stID);
        DistrStiva(pool);
        DistrQ(wait);
        DistrQ(run);
        return -1;
    }
    fgets(line, dimTEXT, in );
    while (fgets(line, dimTEXT, in )) { //citire linie cu linie din fisier
        cuv = strtok(line, " \n");
        /*------------------ADD TASK-----------------------------------------*/
        if (strcmp(cuv, "add_tasks") == 0) {
            void * ID_disponibil = malloc(sizeof(unsigned short));
            cuv = strtok(NULL, " \n");
            nr_tasks = atoi(cuv);
            cuv = strtok(NULL, " \n");
            timp_exec = atoi(cuv);
            cuv = strtok(NULL, " \n");
            prioritate = atoi(cuv);
            for (i = 0; i < nr_tasks; i++) {
                Task * task = (Task * ) malloc(sizeof(Task));
                task -> timp_exec = timp_exec;
                task -> prioritate = prioritate;
                Pop((void * ) stID, ID_disponibil); //extrag din stiva de ID uri
                task -> ID = * ((unsigned short * )(ID_disponibil));
                if (InsertQsort(wait, task) == 1) {
                    fprintf(out, "Task created successfully : ID %hu.\n", *((unsigned short * )(ID_disponibil)));
                }
            }
            free(ID_disponibil);
        }
        /*-----------------------GET TASK------------------------------------*/
        if (strcmp(cuv, "get_task") == 0) {// get_task <task_id>
            cuv = strtok(NULL, " \n");
            task_ID = atoi(cuv);
            ok = 0;
            AQ aux = InitQ(sizeof(Task));
            while ((START(run) != NULL)) { // cat timp mai am ce extrage din run 
                Task * el = (Task * ) malloc(sizeof(Task));
                ExtrQ(run, el);
                InsertQ(aux, el);
                if (el -> ID == task_ID) {
                    fprintf(out, "Task %d is running (remaining_time = %d).\n", el -> ID, el -> timp_exec);
                    ConcatQ(aux, run);
                    ok = 1; // am gasit task-ul
                }
            }
            ConcatQ(run, aux);
            DistrQ(aux);
            if (ok == 0) {
                aux = InitQ(sizeof(Task));
                while ((START(wait) != NULL)) { // cat timp mai am ce extrage din wait
                    Task * el = (Task * ) malloc(sizeof(Task));
                    ExtrQ(wait, el);
                    InsertQ(aux, el);
                    if (el -> ID == task_ID) {
                        fprintf(out, "Task %d is waiting (remaining_time = %d).\n", el -> ID, el -> timp_exec);
                        ConcatQ(aux, wait);
                        ok = 1;
                    }
                }
                ConcatQ(wait, aux);
                DistrQ(aux);
            }
            if (ok == 0) {
                aux = InitQ(sizeof(Task));
                while ((START(finish) != NULL)) { // cat timp mai am ce extrage din finish
                    Task * el = (Task * ) malloc(sizeof(Task));
                    ExtrQ(finish, el);
                    InsertQ(aux, el);
                    if (el -> ID == task_ID) {
                        fprintf(out, "Task %d is finished (executed_time = %d).\n", el -> ID, el -> timp_exec);
                        ConcatQ(aux, finish);
                        ok = 1;
                    }
                }
                ConcatQ(finish, aux);
                DistrQ(aux);
            }
            if (ok == 0)
                fprintf(out, "Task %d not found.\n", task_ID);
        }
        /*-------------------GET THREAD--------------------------------------*/
        if (strcmp(cuv, "get_thread") == 0) {
            cuv = strtok(NULL, " \n");
            thread_id = atoi(cuv);
            ok = 0;
            if (thread_id <= N - 1) {
                AQ aux = InitQ(sizeof(Task));
                while ((START(run) != NULL)) { // cat timp mai am ce extrage din run 
                    Task * el = (Task * ) malloc(sizeof(Task));
                    ExtrQ(run, el);
                    InsertQ(aux, el);
                    if (el -> fir.ID == thread_id) {
                        fprintf(out, "Thread %d is running task %d (remaining_time = %d).\n", thread_id, el -> ID, el -> fir.timp_ramas);
                        ConcatQ(aux, run);
                        ConcatQ(run, aux); // am reparat wait 
                        DistrQ(aux);
                        ok = 1;
                        break;
                    }
                }
                if (ok == 0) {
                    fprintf(out, "Thread %d is idle.\n", thread_id);
                    DistrQ(aux);
                }
            }
        }
        /*-------------------PRINT-------------------------------------------*/
        if (strcmp(cuv, "print") == 0) {
            cuv = strtok(NULL, " \n");
            AQ aux = InitQ(sizeof(Task));
            if (strcmp(cuv, "waiting") == 0) {
                fprintf(out, "====== Waiting queue =======\n[");
                if (START(wait) == NULL)
                    fprintf(out, "]\n");
                while (START(wait) != NULL) {
                    Task * el = (Task * ) malloc(sizeof(Task));
                    ExtrQ(wait, el);
                    InsertQ(aux, el);
                    if (START(wait) == NULL) {
                        fprintf(out, "(%hu: priority = %d, remaining_time = %d)]\n", el -> ID, el -> prioritate, el -> timp_exec);
                    } else
                        fprintf(out, "(%hu: priority = %d, remaining_time = %d),\n", el -> ID, el -> prioritate, el -> timp_exec);
                }
                ConcatQ(wait, aux);
                DistrQ(aux);
            }

            if (strcmp(cuv, "running") == 0) {
                fprintf(out, "====== Running in parallel =======\n[");
                if (START(run) == NULL)
                    fprintf(out, "]\n");
                while (START(run) != NULL) {
                    Task * el = (Task * ) malloc(sizeof(Task));
                    ExtrQ(run, el);
                    InsertQ(aux, el);
                    if (START(run) == NULL) {
                        fprintf(out, "(%hu: priority = %d, remaining_time = %d, running_thread = %d)]\n", el -> ID, el -> prioritate, el -> fir.timp_ramas, el -> fir.ID);
                    } else
                        fprintf(out, "(%hu: priority = %d, remaining_time = %d, running_thread = %d),\n", el -> ID, el -> prioritate, el -> fir.timp_ramas, el -> fir.ID);
                }
                ConcatQ(run, aux);
                DistrQ(aux);
            }

            if (strcmp(cuv, "finished") == 0) {
                fprintf(out, "====== Finished queue =======\n[");
                if (START(finish) == NULL)
                    fprintf(out, "]\n");
                while (START(finish) != NULL) {
                    Task * el = (Task * ) malloc(sizeof(Task));
                    ExtrQ(finish, el);
                    InsertQ(aux, el);
                    if (START(finish) == NULL) {
                        fprintf(out, "(%hu: priority = %d, executed_time = %d)]\n", el -> ID, el -> prioritate, el -> timp_exec);
                    } else
                        fprintf(out, "(%hu: priority = %d, executed_time = %d),\n", el -> ID, el -> prioritate, el -> timp_exec);
                }
                ConcatQ(finish, aux);
                DistrQ(aux);
            }
        }
        /*----------------------------RUN------------------------------------*/
        if (strcmp(cuv, "run") == 0) {
            cuv = strtok(NULL, " \n");
            T = atoi(cuv);
            fprintf(out, "Running tasks for %d ms...\n", T);
            Run(time_ptr, T, Q, run, wait, finish, pool, stID);
        }
        /*----------------------------FINISH---------------------------------*/
        if (strcmp(cuv, "finish") == 0) {
            Finish(time_ptr, Q, run, wait, finish, pool, stID);
            fprintf(out, "Total time: %d", time);
        }
    }

    free(line);
    fclose( in );
    fclose(out);
    DistrQ(wait);
    DistrQ(run);
    DistrQ(finish);
    DistrStiva(stID);
    DistrStiva(pool);
    return 0;
}