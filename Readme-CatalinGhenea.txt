Readme - Catalin Ghenea

Modificari strucutra sarpe
Au fost adaugate campurile "tail" si "prevTail" pentru a tine minte coordonatele actuale ale cozii sarpelui si pentru a tine minte
coordonatele precedente ale cozii dupa o mutare. Coordonatele precedente sunt necesare pentru a putea recupera starea precedenta
in cazul unei coliziuni.

Define modulo:
Din motive de eficienta exita un define pentru a redefini functia de modulo.
Acesta este necesar pentru a calcula coordonatele serpilor mult mai usor.
De exemplu daca avem o lume de 9x9 si sarpele are capul pe 0,5 si merge catre N,
urmatoarea coordonata va fi 8,5 N. 
Pentru directia N trebuie scazut 1, astfel operatia generica pentru modificarea coordonatei x ar fi (X - 1 ) mod MaxX
Totusi in C (-1)mod 9 nu da 8.
Astfel a trebui sa implementez o fuctie separata.
Am preferat un define in locul unei functii din motive de eficienta.

Algoritm secvential:

1. Determinarea coordonatelor cozii pentru fiecare sarpe.
Pentru a determina aceste coordonate se pleaca de la coordonale "head" si se navigheaza in urmatoarea casuta posibila.
Din specifiicatiile enuntului si preciziarile de pe forum serpii vor fi definiti astfel incat intr-un moment de timp sa 
existe o singura varianta de a merge mai departe in sarpe.
La fiecare pas se navigheaza mai departe in sarpe tinand cont de directia din care am venit, in momentul in care nu mai exista
o posibilitate de deplasare in alta casuta inseamna ca am ajuns in coordonatele cozii.

Pasii urmatori se aplica pentru fiecare pas k
Mutarea unui sarpe nu inseaman decat eliminarea cozii, calcularea noii cozi si mutarea capului.

2. Se sterg tailurile din world
Se parcurge fiecare sarpe si se sterge coada sa din world.
In plus se calculeaza si noua coada ca fiind singura casuta in care se poate ajunge printr-o mutare catre N,V,E sau S.

3. Se muta capul fiecarui sarpe
Pentru fiecare sarpe se calculeaza noua coordonata a sarpelui.
In noua coordonata noului cap din matricea world se adauga valoarea identificatorului sarpelui curent

4. Se verifica daca au existat coliziuni
Acest lucru este destul de simplu
Singurul loc in care ar putea sa apara coliziuni sunt in punctele definite de noile capete ale serpilor
Astfel se verifica fiecare punct pe rand.
In cazul in care valoarea prezenta la coordonatele capului unui sarpe este diferita de identificatorul lui inseamana ca a avut loc
o coliziune. Daca coordonata ar fi fost libera ar fi avut valoarea 0 si adaugand identificatoul sau la 0 ar fi rezultat
identificatorul.

5.Continuare sau refacere world
In cazul in care nu exista coliziuni se continua executia.
Daca exista coliziuni trebuie refacuta starea matricei world si a serpilor pentru a putea fi afisata.
Pentru refacere se fac pasii in ordine inversa
Se scot din matrice noile capete adaugate
Se recalculeaza coordonata veche
Se adauga vechia coada salvata in structura sarpelui.


Zone paralelizate:
Din algoritmul serial se poate vedea foarte clar ca toate operatiile pot fi paralelizate pentru fiecare sarpe.
Astfel applicatia va fi scalabila raportat la numarul de serpi.
Exista doua sectiuni in care trebuie facut accesul critic:
In momentul in care adaugi un head nou pentru un sarpe in matrice.
In momentul in care cresti numarul de coliziuni.
Se poate observa ca aceatea sunt facute zone critice in cod.

Inafara de zonele mentionate mai sus toate operatiile pot fi paralelizate pentru fiecare sarpe.
Astfel ideal ar fi sa avem un numar de thread-uri egal cu numarul de serpi.
Toate operatiile din algoritmul secvential sunt independente pentru fiecare sarpe.


Implementari multiple:
Tin sa mentionez ca varianta actuala este una la care s-a ajuns dupa mai multe incercari.
Primele incercari avea multe structuri suplimentare cum ar fi, o copie a lumii actuale si a vectorului de serpi speciala pentru fiecare pas.
La acestea s-au adaugat lock-uri pentru fiecare element din matrice pentru a nu exista probleme atunci cand se scrie un nou head.
Totusi o matrice de 3000x3000 doar pentru a tine lock-uri nu scala.
Urmatoarea varianta a presupus lock-uri doar pentru zone din matrice. De exemplu sa faci lock doar pentru o coloana. Acest lucru a adus o imbunatatire.

In una din variante exista si o lista simplu inlantuita in care se tinea legaturile dintre punctele sarpelui.
In cazul in care algoritmul actual nu ar fi fost suficient pentru a detecta cu exactitate coada.
Acest lucru adauga o complexitate suplimentare deoarece la fiecare pas pentru fiecare sarpe trebuiau actualizate toate 
punctele din componenta sa. Acest lucru adaugand o complexitate de O(L) / sarpe unde L este lungimea celui mai lung sarpe.
Totusi si aceasta operatiune putea fi facuta paralel pentru fiecare sarpe.

Intr-un final am decis sa renunt la partea de liste simplu inlantuite si sa calculez de fiecare data coada sarpului.
Din lipsa testelor cred ca nu ar exista un cornen case in care algoritmul meu sa nu functioneze.

Dupa acest pas am decis sa nu mai tin o copie a matricei si a serpilor.
Copierea lor dura un timp semnificativ si nu putea fi foarte usor paralelizata din cauza acesului la memorie.
Am decis ca o operatiune care dureaza O(n) unde n este numarul de serpi in momentul in care detectez o coliziune scaleaza mai bine.

In acest moment timpul era imbunatatit
Dar tot era destul de mare. (undeva de 3 ori mai mare decat este acum)
In plus nu scala. Chiar daca cresteam numarul de fire de executie timpul nu scadea semnificativ.

Codul parea destul de bine organizat si toate zonele care putea fi paralelizate fusesera parelelizate.
Totusi foarte multe din operatiile pe care le faceam se intamplau in functii auxiliare.
Existau functii precum:

getNextPoint (point, direction) - calcula urmatorul punct in functie de punctul actual si directie 
getNextTailPoint - similar cu metoda de mai sus doar ca era o implementare particulara pentru a calcula urmatorul pentru pentru coada
unui sarpe
undoChanges - functia care recalcula world si snakes pentru pasul anterior in cazul unei coliziuni.
modulo - calculeaza modulo pe un numar, acum este un define

Am decis ca nu are rost sa tin functii separate si ca pot adauga cod inline
Chiar daca claritatea codului ar putea fi compromisa eficienta sa va creste.
Copierea contextului si alocarea spatiului pentru o functie noua dureaza timp si necesita access suplimentar la memorie


Dupa modificarea functiilor in cod inline codul ruleaza mult mai repede si scaleaza mult mai bine.


Timpi obtinuti pentru big_input:

OMP_NUM_THREADS = 1 , nr pasi = 2.000
fep.grid.pub.ro: 0.278
local: 0.162

OMP_NUM_THREADS = 2 , nr pasi = 2.000
fep.grid.pub.ro: 0.175
local: 0.094

OMP_NUM_THREADS = 4 , nr pasi = 2.000
fep.grid.pub.ro: 0.085
local: 0.063

OMP_NUM_THREADS = 6 , nr pasi = 2.000
fep.grid.pub.ro: 0.065
local:-

OMP_NUM_THREADS = 8 , nr pasi = 2.000
fep.grid.pub.ro: 0.088
local: - 



OMP_NUM_THREADS = 1 , nr pasi = 20.000
fep.grid.pub.ro: 2.739
local: 1.613

OMP_NUM_THREADS = 2 , nr pasi = 20.000
fep.grid.pub.ro: 1.440
local: 0.822

OMP_NUM_THREADS = 4 , nr pasi = 20.000
fep.grid.pub.ro: 0.831
local: 0.525

OMP_NUM_THREADS = 6 , nr pasi = 20.000
fep.grid.pub.ro: 0.618
local:-

OMP_NUM_THREADS = 8 , nr pasi = 20.000
fep.grid.pub.ro: 0.547
local:-



OMP_NUM_THREADS = 1 , nr pasi = 100.000
fep.grid.pub.ro: 13.6658 
local: 7.852

OMP_NUM_THREADS = 2 , nr pasi = 100.000
fep.grid.pub.ro: 6.886
local: 4.204

OMP_NUM_THREADS = 4 , nr pasi = 100.000
fep.grid.pub.ro: 5.681
local: 2.586

OMP_NUM_THREADS = 6 , nr pasi = 100.000
fep.grid.pub.ro: 3.163
local:-

OMP_NUM_THREADS = 8 , nr pasi = 100.000
fep.grid.pub.ro: 2.678 
local: - 



