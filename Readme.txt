Pasare Catalin-Ciprian 334CB

                                                            Tema 3 APD

    Voi prezenta pe rand cele 3 cerinte rezolvate:
    1. Pentru prima cerinta imi creez o functie numita deisplay care va afisa in consola un mesaj
       corespunzator pentru fiecare send/calup de send-uri. Apoi, fiecare dintre cele 4 procese lider
       isi va cunoaste workerii citind rangurile lor din fisierele de intrare si stocand aceste ranguri
       in niste vectori de tip vx (x este rangul procesului lider). Dupa ce fiecare lider isi cunoaste
       workerii, le trimite acestora rangul lor pentru ca fiecare worker sa stie cine este parintele lor.
       Dupa aceea liderul 0 incepe sa formeze topologia intr-un string si sa o trasnmita mai departe la procesul
       vecin 1. Acesta isi trece si el rangul precum si workerii in string si transmite mai departe la 2,
       care transmite la 3. La procesul cu rangul 3 topologia este formata, iar acum trebuie doar afisata.
       Procesul 3 trimite topologia finala procesului 0 care o afiseaza, iar acesta la randul lui o trimite
       workerilor lui, precum si procesului 1. Actiunea se repeta pentru restul proceselor pe acelasi traseu
       (0 -> 1 -> 2 -> 3).
    
    2. Pentru aceasta cerinta procesul 0 va prelua dimensiunea vectorului ca argument si va initializa
       vectorul. Mai departe va imparti sarcinile intre toti workerii din topologie, astfel ca fiecare
       va avea (nr_iteratii / workeri) operatii de realizat initial. Pe exemplul cu 12 elemente
       avem (12/8) = o iteratie per worker. La procesul cu rangul 3 vectorul va fi gata, insa
       daca impartirea e cu rest vom mai avea elemente neprelucrate, astfel ca vom relua 
       procesul de mai sus, de data asta fiecare worker va face fix o operatie pana cand
       vectorul va fi gata format. Dupa aceea vectorul va fi transmis procesului 0
       care il va afisa in consola.

    3. Pentru cerinta 3 procesul 0 va citi argumentul 2 din linia de comanda si il va transmite mai departe
       celorlalti lideri pe traseul 0 -> 3 -> 2 -> 1. Pentru a nu interactiona cu codul scris la primele
       2 cerinte, rezolvarea va fi scrisa de la 0, dezavantajul fiind codul duplicat. Pentru topologie,
       procesul 0 va transmite topologia initiala catre procesul 3 care nu o va modifica deocamdata pentru
       a pastra ordinea 0,1,2,3. Procesul 3 va transmite lui 2, iar 2 lui 1. Procesul 1 va completa topologia
       si o va transmite lui 2, apoi 3 (si acestia vor completa topologia), ca mai apoi topologia completa
       (si in ordine) sa ajunga la 0. De aici se va mai face un circuit pe acelasi traseu pentru afisarea
       topologiei finale de catre fiecare proces. Pentru partea a doua cu vectorul se procedeaza similar,
       calculele realizandu-se pe traseul 0 -> 3 -> 2 -> 1, apoi daca mai raman iteratii (impartire inexacta) 
       se mai face cate o interatie per worker pe traseul 1 -> 2 -> 3 -> 0 apoi vectorul format e afisat
       in terminal de procesul 0.