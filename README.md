#### Sima Alexandru 312CA 2022-2023
# Tema #3 - Editor de imagini

--------------------------------------------------------------------------------
## Functionalitate
    Programul este un editor de imagini care poate incarca, salva si modifica
imagini prin rotire, decupare, aplicare filtre si egalizare, cat si genera
histograme ale acestora. Acesta poate citi imagini in format PPM si PGM.
    Programul va citi de la tastatura comenzi, cate una pe un rand. Comenzile
sunt de forma "<NUME_COMANDA> [parametri...]". In cazul unei erori de alocare,
programul va elibera toate resursele alocate si va intoarce codul de eroare 1.

--------------------------------------------------------------------------------
## Fisiere
- `main.c`: entry-pointul programului si functii pentru verificarea carei
comenzi a fost introdusa.
- `structuri.h`: declarari ale structurilor si enumerarilor folosite in mai
multe fisiere din cadrul programului.

- `alocari`: functii destinate alocarii/eliberarii in/din memorie a imaginilor.
- `filtre`: functii pentru operatia de aplicat efecte pe imagini.
- `fisiere`: functii destinate citirii/scrierii in/din fisiere a imaginilor.
- `histograme`: functii pentru egalizarea imaginilor si calcularea histogramelor
acestora.
- `operatii`: functii pentru restul operatiilor realizate pe imagini (selectarea
suprafetei, decupare si rotire).
- `utilitare`: alte functii cu scop general.

--------------------------------------------------------------------------------
## Comenzi
- `LOAD <cale>`: Incarca in memorie fisierul aflat la locatia `cale`. Pentru
aceasta, se parseaza primii 2 octeti pentru a se afla tipul fisierului
(color/alb-negru si binar/ascii) apoi se citesc dimensiunile si, eventual,
valoarea maxima a unui pixel, iar apoi matricea efectiva de pixeli.

- `SAVE <cale> [ascii]`: Se scrie la locatia `cale` imaginea din memorie (daca
exista), respectandu-se formatul PPM/PGM. Daca in comanda este prezent cuvantul
"ascii", fisierul va fi salvat in mod text. Altfel, fisierul va fi scris in mod
binar.

- `SELECT`:
    - `SELECT ALL`: Selecteaza intreaga imagine din memorie.
    - `SELECT <x1> <x2> <y1> <y2>`: Selecteaza subimaginea determinata de 2
    colturi (`x1`, `y1`) si (`x2`, `y2`).
Intern, selectia este retinuta memorandu-se colturile din stanga-sus si
dreapta-jos ale suprafetei, astfel ca coltul din stanga-sus va avea coordonatele
minime, iar celalalt colt, coordonatele maxime.
Afiseaza un mesaj de eroare daca nu este incarcata o imagine sau daca nu este
furnizata o selectie.

- `CROP`: Se inlocuieste imaginea din memorie cu subimaginea selectata, apoi
este selectata integral noua imagine.
Se afiseaza un mesaj de eroare daca nu este incarcata nicio imagine.

- `ROTATE <unghi>`: Roteste selectia imaginii cu unghiul specificat, daca este
posibil. Pentru a se face rotirea:
    - Unghiul trebuie sa fie un multiplu de 90.
    - Unghiul trebuie sa fie cuprins in intervalul [-360; 360].
    - In cazul in care nu este selectata intreaga imagine, selectia trebuie sa
    fie patratica.
Rotirea se efectueaza alocandu-se o matrice auxiliara in care se copiaza pixelii
din selectie, pentru fiecare pixel calculandu-se pozitia originala (inainte de
rotire). O rotire cu un unghi negativ `x` este echivalenta cu o rotire
`360 + x`. Daca toata imaginea este selectata, noua imagine va fi matricea
auxiliara (se inverseaza dimensiunile daca este necesar). In caz contrar, se
copiaza valorile din matricea auxiliara, in imagine si se elibereaza matricea.
In cazul in care rotirea nu se poate efectua (nu exista o imagine incarcata,
unghiul este invalid sau pe selectia data nu se poate aplica acea rotire), se va
afisa un mesaj de eroare.

- `APPLY <filtru>`: Aplica un filtru pe selectia imaginii. Filtrele disponibile
sunt:
    - `EDGE`
    - `SHARPEN`
    - `BLUR`
    - `GAUSSIAN_BLUR`
    Fiecare filtru are un nucleu corespunzator (o matrice 3x3). Pentru aplicare
    se calculeaza noua valoare a fiecarui pixel care nu este pe margine
    inmultindu-se valorile acelui pixel si a vecinilor cu valorile
    corespunzatoare din nucleu, adunandu-se valorile rezultate, limitate in
    intervalul [0; 255]. Apoi pixelii vechi sunt inlocuiti cu valorile noi noua.
Se afiseaza un mesaj de eroare in cazul in care nu este incarcata o imagine, nu
este dat un filtru sau acesta este invalid.

- `HISTOGRAM <numar stelute> <numar intervale>`: Calculeaza si afiseaza
histograma imaginii selectate. Pentru aceasta, se parcurge imaginea si se
calculeaza frecventa tuturor pixelilor din selectie, pe intervale, apoi se
calculeaza intervalul cu frecventa maxima, acesta fiind reprezentat cu numarul
maxim de stelute, iar restul folosindu-se regula de 3 simpla.
Afiseaza un mesaj de eroare daca histograma nu se poate realiza (imaginea nu
este incarcata sau este color, nu sunt suficiente argumente).

- `EQUALIZE`: Egalizeaza imaginea selectata, calculand frecventa tuturor
valorilor pixelilor din imagine (folosind aceeasi functie ca la HISTOGRAM), apoi
aplicand formula de egalizare.
Afiseaza un mesaj de eroare daca imaginea este color sau nu este incarcata.

- `EXIT`: elibereaza toate resursele alocate si termina executia programului (cu
codul de eroare 0).

--------------------------------------------------------------------------------
## Particularitati
Exista sisteme diferite de coordonate ale pixelilor, de aceea, coordonatele in
imagine (coloana, linie) sunt notate cu (x, y), iar cele in matrice
(linie, coloana) cu (i, j).

Deoarece nu se stie lungimea liniilor citite, un rand se va citi in transe de
cate `BUFSIZ` caractere (lungimea bufferului functiilor de I/O predefinite), si
se concateneaza la stringul rezultat. Linia se termina cand ultimul caracter
citit este un Line Feed, care se scoate din stringul rezultat.
