/* IMBREA Giulia-Stefania - 311CB */ 
TEMA 2 SDA

[14-42] TLG
-aici am declarat structura unei liste generice
-am realizat functia de alocare a unei celule si cea de distrugere

[42-56] STRUCTURI
- am declarat structura Thread, care are in plus, fata de specificatiile din tema, campul timp ramas, 
pentru a realiza scurgerea timpului odata cu comanda run si pentru a tine in evidenta thread-urile care pot
fi eliberate
- am declarat structura Task, conform cerintei

[57-141] STIVA
- am declarat structura unei stive
- am realizat functii specifice stivei, precum: Initializarea unei stive, Inserarea unui element in stiva(Push), Scoaterea
unui element din stiva(Pop), Distrugerea unei stive
- pe langa functiile "clasice", am implementat:
    > functia ConcatS(void * dest, void * src), care pur si simplu muta toate elementele din sursa in destinatie, cu ajutorul lui Pop si Push
    > functia PushsortInt(void * s, void * el), folosita pentru a reintroduce ORDONAT in stiva de id-uri, un ID eliberat in urma comezii run
        - pentru a intelege mai bine, voi folosi un exemplu:
        Stiva cu id :   < 2 3 7 8 9 10 ]
        vreau sa pun la loc in stiva id ul 4, el = 4
        @ extr = 2, stiva devine < 3 7 8 9 10]
          2 < 4 da => stiva temporara devine <2]
        @ extr = 3, stiva devine < 7 8 9 10]
          3 < 4 da => stiva temporara devine <3 2]
        @ extr = 7, stiva devine < 8 9 10]
          7 < 4 nu => stiva temporara devine <7 5 3 2]
          in urma concatenarii stiva initiala devine < 2 3 5 7 8 9 10]

        daca nu se gaseste niciun element mai mare decat id ul de inserat, id ul va fi inserat la sfarsitul lui temp, iar stiva sursa va fi 
        refacuta

[142-256] COZI 
-am declarati structura unei cozi 
- am realizat functii specifice cozii, precum: Initializarea unei cozi, Inserarea unui element in coada(InsertQ), Scoaterea
unui element din coada(ExtrQ), Distrugerea unei cozi
- pe langa functiile "clasice", am implementat:
    > ConcatQ(void * dest, void * src), folosita pentru a muta toate elementele din sursa in destinatie, se bazeaza pur si simplu pe algoritmii 
    de ExtrQ si de InsertQ
    > InsertQsort(void * q, void * el), folosit pentru a insera un task in cozile de wait, run sau finished, respectand oridinea ceruta:
        -descrescator dupa prioritate
        -crescator dupa tim de executie
        -crescator dupa ID
     -aceasta inserare se bazeaza pe extragerea din coada a unui element, compararea cu elementul de inserat si folosirea unei cozi auxiliare 
     pentru a nu pierde informatia

[257-400] ALTE FUNCTII UTILE
-THpool(int N), creeaza o stiva de N thread uri si o returneaza;
-stiva_ID(ASt s), creeaza in s o stiva de id uri, returneaza 1 daca a mers Inserarea
-Max, Min(a,b) sunt functii de comparare returneaza 1 daca a este Max/Min
-timpMaxMin, determina timpul maxim sau minim ramas de executat intr-o coada de running
-Time(dif, ...), este apelata atucni cand se apeleaza comanda run, practic face urmatoarele:
    1)mai intai scade cu "dif" campul timp_ramas din thread-ul fiecarui task din run
    2)in urma acestei scaderi,vor fi task-uri cu timpul rams <=0 => trebuie sa eliberam id-ul lor, thread-ul si sa le mutam in finish
-Run(*time, T, Q, ...) 
    @ este prima functie(functia mama) apelata atunci cand se da comanda run
    @ verifica mai intai daca mai are ce executa (adica coada de wait sa fie goala, la fel si cea de run)
    @ daca da, verifica daca mai pot insera ceva in coada run( exista thread-uri disponibile si mai e ceva in coada de waiting)
    @ daca da, preiau elementul din coada de waiting, initializez timpul ramas cu timpul de executie, ii atribui un thread, si inserez taskul 
    scos in coada de running
    @ in functie de T, Q, timpul maxim ramas de executat din coada de running, si tinand cont de faptul ca thread-ul poate executa task-uri 
    pentru un ciclu de maxim Q ms, am apelat functia Time, pentru a realiza scaderea potrivita, am marit timpul primit ca paramentru in Run,
     si eventual(daca nu s-a scazut T) am reapelat functia Run
    @ prin efect lateral, voi stie cat timp a trecut (*time!!!)
-Finish(*time, Q, ...)
    @ este folosita la citirea comenzii finish
    @ asemanator ca la Run, verifica daca mai are ceva de executat, insereaza eventual in coada de run noi task-uri
    @ calculeaza timpul minim ramas in executarea task-urilor din coada run
    @ stiind ca thread-ul poate executa pt maxim Q ms., voi apela dupa caz functia Time, voi mari parametru *time cu timpul scazut, si voi 
    reapela din nou Finish, avand cozile modificate
    @ prin efect lateral, voi stie cat timp a trecut (*time!!!)
OBS: acest int *time , pointeaza la valoarea 0, ea este marita odata cu functiile de Run si Finish

[400-456] MAIN
-contine declaratii, initializari de cozi si stive;
-deschide fisierul de input, de output, si incepe citirea din in
[457-477] ADD TASK 
-add_tasks <number_of_tasks> <exec_time_in_ms> <priority>
-citeste fiecare cuvant din linie
-adauga <number_of_tasks> in coada de wait, mai intai initializandu-le cu un ID unic, scos din stiva de ID-uri
-printeaza in out "Task created successfully : ID <ID-ul taskului>."
[478-528] GET TASK
-get_task <task_id>
-Citeste task_ID
-parcurg cozile de run, wait, finish, iar cu ajutorul unei cozi temporaree, extrag cate un element din fiecarea coada 
pana cand gasesc un task cu ID ul cerut(folosesc ExtrQ si InsertQ), daca l-am gasit, refac cozile originale
[529-554] GET THREAD
-celasi lucru ca la get_task, doar ca l fac in raport cu thread-urile task-urilor din coad run
[555-609] PRINT 
-printeaza coada si parametri ceruti, pentru a face asta, parcurge coada cu ajutorul unei cozi temporare , extrage si introduce elementul
coada temp., la final reconstruieste coada originala
[610-616] RUN 
-citesc T, afisez mesajul si apelez functia Run
[617-622] Finish
-apelez functia Finish pt a executa restul task-urilor ramase
-printez timpul total de executie al tuturor task-urilor(time)

OBS !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11
1) time = 0
   time_ptr este un pointer la time
   apeland run si finish, time ul va creste cu ajutorul pointer-ului time_ptr
2) legat de numarul de coloane, majoritatea liniilor au in jur de 80 de caractere, insa la partea de printare, intr-adevar
acolo se depasesc 120 de coloane per linie, insa mi s-ar fi parut mult mai inestetic sa rup fprintf-ul :))
3) in urma lui ./check.sh, am primit 135/135 puncte si 20/20 bonus
