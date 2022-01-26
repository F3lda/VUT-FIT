--zobrazeni data a casu--
ALTER SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD HH24:MI:SS';



--smazani tabulek--
DROP TABLE Pracovnik CASCADE CONSTRAINTS;
DROP TABLE Ctenar CASCADE CONSTRAINTS;

DROP TABLE Titul CASCADE CONSTRAINTS;
DROP TABLE Tema CASCADE CONSTRAINTS;
DROP TABLE Zanr CASCADE CONSTRAINTS;
DROP TABLE Autor CASCADE CONSTRAINTS;

DROP TABLE Titul_zanr CASCADE CONSTRAINTS;
DROP TABLE Zanr_autor CASCADE CONSTRAINTS;
DROP TABLE Titul_autor CASCADE CONSTRAINTS;
DROP TABLE Titul_tema CASCADE CONSTRAINTS;

DROP TABLE Vytisk CASCADE CONSTRAINTS;
DROP TABLE Rezervace CASCADE CONSTRAINTS;

DROP TABLE Vypujcka CASCADE CONSTRAINTS;



--vytvareni tabulek--
/* Osoby */
CREATE TABLE Pracovnik (
    id_pracovnika INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    jmeno VARCHAR(20) NOT NULL,
    prijmeni VARCHAR(20) NOT NULL,
    email VARCHAR(50),

    CONSTRAINT PK_Pracovnik PRIMARY KEY (id_pracovnika)
);

CREATE TABLE Ctenar (
    cislo_prukazu INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    platnost_prukazu DATE NOT NULL,
    jmeno VARCHAR(20) NOT NULL,
    prijmeni VARCHAR(20) NOT NULL,
    email VARCHAR(50),
    telefon VARCHAR(15) NOT NULL,
    ulice VARCHAR(20) NOT NULL,
    cislo_popisne INTEGER NOT NULL,
    mesto VARCHAR(20) NOT NULL,
    psc INTEGER NOT NULL,
    stat VARCHAR(20) NOT NULL,

    CONSTRAINT PK_Ctenar PRIMARY KEY (cislo_prukazu)
);



/* Titul */
CREATE TABLE Tema (
    id_tema INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    nazev VARCHAR(20) NOT NULL UNIQUE, -- UNIQUE


    CONSTRAINT PK_Tema PRIMARY KEY (id_tema)
);

CREATE TABLE Zanr (
    id_zanru INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    nazev VARCHAR(20) NOT NULL UNIQUE, -- UNIQUE


    CONSTRAINT PK_Zanr PRIMARY KEY (id_zanru)
);

CREATE TABLE Autor (
    id_autora INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    jmeno VARCHAR(20) NOT NULL,
    prijmeni VARCHAR(20) NOT NULL,
    rok_narozeni INTEGER, -- CHECK - CHK_Autor_rok
    rok_umrti INTEGER, -- CHECK - CHK_Autor_rok
    narodnost VARCHAR(20),


    CONSTRAINT CHK_Autor_rok CHECK (rok_umrti=NULL OR rok_narozeni<rok_umrti),
    
    CONSTRAINT PK_Autor PRIMARY KEY (id_autora)
);

CREATE TABLE Titul (
    id_titulu INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    typ VARCHAR(10) NOT NULL, -- CHECK - CHK_Titul_typ
    nazev VARCHAR(50) NOT NULL,
    podnazev VARCHAR(50),
    rok_vydani INTEGER NOT NULL,
    cislo_vydani INTEGER NOT NULL,
    vydavatel_nakladatelstvi VARCHAR(50),
    ISBN VARCHAR(20) UNIQUE,  -- CHECK - CHK_Titul_typ -- UNIQUE - každý titul má své unikátní identifikační číslo
    ISSN VARCHAR(20) UNIQUE,  -- CHECK - CHK_Titul_typ -- UNIQUE - každý titul má své unikátní identifikační číslo
    -- některé tituly mohou mít přidělena obě identifikační čísla


    CONSTRAINT CHK_Titul_typ CHECK (typ IN ('kniha', 'časopis')),
    CONSTRAINT CHK_Titul_ISBN_ISSN CHECK (ISBN<>NULL OR ISSN<>NULL), -- alespoň jedno identifikační číslo musí být zadané

    CONSTRAINT PK_Titul PRIMARY KEY (id_titulu)
);



/* Vazební tabulky */
CREATE TABLE Titul_zanr (
    id_titulu INTEGER NOT NULL, -- FOREIGN KEY - Titul
    id_zanru INTEGER NOT NULL, -- FOREIGN KEY - Zanr


    CONSTRAINT PK_Titul_zanr PRIMARY KEY (id_zanru, id_titulu),

    CONSTRAINT FK_Titul_zanr_id_titulu FOREIGN KEY (id_titulu) REFERENCES Titul(id_titulu),
    CONSTRAINT FK_Titul_zanr_id_zanru FOREIGN KEY (id_zanru) REFERENCES Zanr(id_zanru)
);

CREATE TABLE Zanr_autor (
    id_zanru INTEGER NOT NULL, -- FOREIGN KEY - Zanr
    id_autora INTEGER NOT NULL, -- FOREIGN KEY - Autor


    CONSTRAINT PK_Zanr_autor PRIMARY KEY (id_zanru, id_autora),

    CONSTRAINT FK_Zanr_autor_id_zanru FOREIGN KEY (id_zanru) REFERENCES Zanr(id_zanru),
    CONSTRAINT FK_Zanr_autor_id_autora FOREIGN KEY (id_autora) REFERENCES Autor(id_autora)
);

CREATE TABLE Titul_autor (
    id_titulu INTEGER NOT NULL, -- FOREIGN KEY - Titul
    id_autora INTEGER NOT NULL, -- FOREIGN KEY - Autor


    CONSTRAINT PK_Titul_autor PRIMARY KEY (id_titulu, id_autora),

    CONSTRAINT FK_Titul_autor_id_titulu FOREIGN KEY (id_titulu) REFERENCES Titul(id_titulu),
    CONSTRAINT FK_Titul_autor_id_autora FOREIGN KEY (id_autora) REFERENCES Autor(id_autora)
);

CREATE TABLE Titul_tema (
    id_titulu INTEGER NOT NULL, -- FOREIGN KEY - Titul
    id_tema INTEGER NOT NULL, -- FOREIGN KEY - Tema 


    CONSTRAINT PK_Titul_tema PRIMARY KEY (id_titulu, id_tema),

    CONSTRAINT FK_Titul_tema_id_titulu FOREIGN KEY (id_titulu) REFERENCES Titul(id_titulu),
    CONSTRAINT FK_Titul_tema_id_autora FOREIGN KEY (id_tema) REFERENCES Tema(id_tema)
);



/* Výtisk Titulu */
CREATE TABLE Vytisk (
    id_vytisku INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    stav VARCHAR(15) NOT NULL, -- CHECK - CHK_Vytisk_stav
    datum_pridani DATE, -- CHECK - CHK_Vytisk_datum
    datum_vyrazeni DATE, -- CHECK - CHK_Vytisk_datum

    id_titulu INTEGER NOT NULL, -- FOREIGN KEY - Titul


    -- KONTROLA V APLIKACI NEBO PŘES FUNKCI/TRIGGER -> při změně stavu zkontrolovat, že nový stav odpovídá (existuje rezervace/výpůjčka)
    CONSTRAINT CHK_Vytisk_stav CHECK (stav IN ('skladem', 'rezervován', 'vypůjčen', 'vyřazen')),
    CONSTRAINT CHK_Vytisk_datum CHECK (datum_pridani IS NULL OR datum_vyrazeni IS NULL OR datum_pridani<datum_vyrazeni),
    
    CONSTRAINT PK_Vytisk PRIMARY KEY (id_vytisku),
    
    CONSTRAINT FK_Cislo_casopisu_id_titulu FOREIGN KEY (id_titulu) REFERENCES Titul(id_titulu)
);

/* Rezervace Titulu */
CREATE TABLE Rezervace (
    id_rezervace INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    stav VARCHAR(20) NOT NULL, -- CHECK - CHK_Rezervace_stav
    rezervovano_od DATE NOT NULL, -- CHECK - CHK_Rezervace_datum
    rezervovano_do DATE NOT NULL, -- CHECK - CHK_Rezervace_datum

    id_titulu INTEGER NOT NULL, -- FOREIGN KEY - Titul
    id_vypujcky INTEGER, -- FOREIGN KEY - Vypujcka
    -- id_vypujcky je NULL pokud Rezervace ještě nebyla vyzvednuta

    id_ctenare INTEGER NOT NULL, -- FOREIGN KEY - Ctenar
    id_pracovnika_zrusil INTEGER, -- FOREIGN KEY - Pracovnik
    -- pokud je stav='zrušena' a id_pracovnika_zrusil=NULL, tak Rezervaci zrušil Čtenář; jinak Rezervaci zrušil Pracovník


    -- KONTROLA V APLIKACI NEBO PŘES FUNKCI/TRIGGER -> id_titulu.id_vytisku.stav='skladem' (existuje nějaký výtisk titulu skladem) + změna stavu na 'rezervován'
    CONSTRAINT CHK_Rezervace_stav CHECK (stav IN ('platná', 'zrušena', 'ukončena')), -- zrušena = zrušena/nevyzvednuta; ukončena = vyzvednuta
    CONSTRAINT CHK_Rezervace_datum CHECK (rezervovano_od<rezervovano_do),

    CONSTRAINT PK_Rezervace PRIMARY KEY (id_rezervace),

    CONSTRAINT FK_Rezervace_id_titulu FOREIGN KEY (id_titulu) REFERENCES Titul(id_titulu),
    CONSTRAINT FK_Rezervace_id_ctenare FOREIGN KEY (id_ctenare) REFERENCES Ctenar(cislo_prukazu),
    CONSTRAINT FK_Rezervace_id_pracovnika_zrusil FOREIGN KEY (id_pracovnika_zrusil) REFERENCES Pracovnik(id_pracovnika)
);

/* Výpůjčka Výtisku */
CREATE TABLE Vypujcka (
    id_vypujcky INTEGER GENERATED ALWAYS as IDENTITY(START with 1 INCREMENT by 1) NOT NULL, -- PRIMARY KEY
    stav VARCHAR(20) NOT NULL, -- CHECK - CHK_Vypujcka_stav
    vypujceno_od DATE NOT NULL, -- CHECK - CHK_Vypujcka_datum
    vypujceno_do DATE NOT NULL, -- CHECK - CHK_Vypujcka_datum

    id_vytisku INTEGER NOT NULL, -- FOREIGN KEY - Vytisk
    id_rezervace INTEGER NOT NULL, -- FOREIGN KEY - Rezervace

    id_ctenare INTEGER NOT NULL, -- FOREIGN KEY - Ctenar
    id_pracovnika_vydal INTEGER NOT NULL, -- FOREIGN KEY - Pracovnik
    id_pracovnika_prijal INTEGER, -- FOREIGN KEY - Pracovnik


    -- KONTROLA V APLIKACI NEBO PŘES FUNKCI/TRIGGER -> id_vytisku.stav='rezervován' a id_rezervace.stav='platná' (+ změna id_vytisku.stav='vypůjčen' a id_rezervace.stav='ukončena')
    CONSTRAINT CHK_Vypujcka_stav CHECK (stav IN ('vypůjčeno','vráceno')),
    CONSTRAINT CHK_Vypujcka_datum CHECK (vypujceno_od<vypujceno_do),
    
    CONSTRAINT PK_Vypujcka PRIMARY KEY (id_vypujcky),
    
    CONSTRAINT FK_Vypujcka_id_vytisku FOREIGN KEY (id_vytisku) REFERENCES Vytisk(id_vytisku),
    CONSTRAINT FK_Vypujcka_id_ctenare FOREIGN KEY (id_ctenare) REFERENCES Ctenar(cislo_prukazu),
    CONSTRAINT FK_Vypujcka_id_pracovnika_vydal FOREIGN KEY (id_pracovnika_vydal) REFERENCES Pracovnik(id_pracovnika),
    CONSTRAINT FK_Vypujcka_id_pracovnika_prijal FOREIGN KEY (id_pracovnika_prijal) REFERENCES Pracovnik(id_pracovnika)
);

/* Cizí klíče relace mezi tabulkami Rezervace a Výpůjčka */
ALTER TABLE Rezervace ADD CONSTRAINT FK_Rezervace_id_vypujcky FOREIGN KEY (id_vypujcky) REFERENCES Vypujcka(id_vypujcky);
ALTER TABLE Vypujcka ADD CONSTRAINT FK_Vypujcka_id_rezervace FOREIGN KEY (id_rezervace) REFERENCES Rezervace(id_rezervace);








--vlozeni udaju do tabulek--
INSERT INTO Pracovnik VALUES(DEFAULT,'Jan','Novák','xnovak01@knihovna.cz');
INSERT INTO Pracovnik VALUES(DEFAULT,'Andrej','Bureš','xbures00@knihovna.cz');
INSERT INTO Pracovnik VALUES(DEFAULT,'Gregor','Johann Mendel','xmendel03@knihovna.cz');
INSERT INTO Pracovnik VALUES(DEFAULT,'František','Palacký','xpalacky00@knihovna.cz');
INSERT INTO Pracovnik VALUES(DEFAULT,'Karel','IV.','xkarel04@knihovna.cz');

INSERT INTO Ctenar VALUES(DEFAULT,TO_DATE('2022-06-05', 'YYYY-MM-DD'),'Franta','Vomáčka','vomac@email.cz','606909007','Jugoslávská',29,'Brno',61300,'CZE');
INSERT INTO Ctenar VALUES(DEFAULT,TO_DATE('2022-06-30', 'YYYY-MM-DD'),'Miloš','Pudding','mipad@gmail.com','723658965','Vídeňská',49,'Brno',63900,'CZE');
INSERT INTO Ctenar VALUES(DEFAULT,TO_DATE('2022-07-15', 'YYYY-MM-DD'),'Milan','Rastislav Štefánik','genstef@tatry.sk','456987321','Brněnská',7,'Bratislava',11300,'SVK');
INSERT INTO Ctenar VALUES(DEFAULT,TO_DATE('2022-02-01', 'YYYY-MM-DD'),'Edvard','Beneš','benes84@gov.csr','32546','Exilová',13,'Praha',00700,'CZE');
INSERT INTO Ctenar VALUES(DEFAULT,TO_DATE('2022-10-12', 'YYYY-MM-DD'),'Dennis','MacAlistair Ritchie','cdennis@comps.com','11332489963','Harvard',69,'Berkeley',10110,'USA');

INSERT INTO Tema VALUES(DEFAULT,'Hobby');
INSERT INTO Tema VALUES(DEFAULT,'Zahrada');
INSERT INTO Tema VALUES(DEFAULT,'Sport');
INSERT INTO Tema VALUES(DEFAULT,'Domov');
INSERT INTO Tema VALUES(DEFAULT,'Technika');

INSERT INTO Zanr VALUES(DEFAULT,'Drama');
INSERT INTO Zanr VALUES(DEFAULT,'Science fiction');
INSERT INTO Zanr VALUES(DEFAULT,'Novela');
INSERT INTO Zanr VALUES(DEFAULT,'Fantasy');
INSERT INTO Zanr VALUES(DEFAULT,'Horor');



/* Přidání Knih a jejich Autorů */
-- John Ronald Reuel Tolkien --
INSERT INTO Autor VALUES(DEFAULT, 'John Ronald', 'Reuel Tolkien', 1892, 1973, 'ENG');



-- Ladislav Fuks --
INSERT INTO Autor VALUES(DEFAULT, 'Ladislav', 'Fuks', 1923, 1994, 'CZE');

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'Spalovač mrtvol', '', 1967, 1, 'Československý spisovatel', '456-25-951-7853-6', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='456-25-951-7853-6') as id_titulu, id_autora FROM Autor WHERE jmeno='Ladislav' AND prijmeni='Fuks' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='456-25-951-7853-6') as id_titulu, id_zanru FROM Zanr WHERE nazev='Novela' FETCH FIRST 1 ROWS ONLY;

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'Spalovač mrtvol', '', 2003, 3, 'Odeon', '448-55-456-2589-3', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='448-55-456-2589-3') as id_titulu, id_autora FROM Autor WHERE jmeno='Ladislav' AND prijmeni='Fuks' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='448-55-456-2589-3') as id_titulu, id_zanru FROM Zanr WHERE nazev='Novela' FETCH FIRST 1 ROWS ONLY;



-- Václav Havel --
INSERT INTO Autor VALUES(DEFAULT, 'Václav', 'Havel', 1936, 2011, 'CZE');

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'Audience', '', 1975, 1, 'Šafrán', '978-80-7483-080-8', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='978-80-7483-080-8') as id_titulu, id_autora FROM Autor WHERE jmeno='Václav' AND prijmeni='Havel' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='978-80-7483-080-8') as id_titulu, id_zanru FROM Zanr WHERE nazev='Drama' FETCH FIRST 1 ROWS ONLY;



-- Božena Němcová --
INSERT INTO Autor VALUES(DEFAULT, 'Božena', 'Němcová', 1820, 1862, 'CZE');

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'Babička', '', 1940, 1, 'L. Mazáč', '698-96-637-2469-4', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='698-96-637-2469-4') as id_titulu, id_autora FROM Autor WHERE jmeno='Božena' AND prijmeni='Němcová' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='698-96-637-2469-4') as id_titulu, id_zanru FROM Zanr WHERE nazev='Novela' FETCH FIRST 1 ROWS ONLY;

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'Babička', '', 1940, 2, 'František Strnad', '9598-56-657-0154-2', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='9598-56-657-0154-2') as id_titulu, id_autora FROM Autor WHERE jmeno='Božena' AND prijmeni='Němcová' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='9598-56-657-0154-2') as id_titulu, id_zanru FROM Zanr WHERE nazev='Novela' FETCH FIRST 1 ROWS ONLY;



-- Karel Čapek --
INSERT INTO Autor VALUES(DEFAULT, 'Karel', 'Čapek', 1887, 1945, 'CZE');

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'R.U.R.', '', 2010, 4, 'Argo', '369-66-823-1456-4', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='369-66-823-1456-4') as id_titulu, id_autora FROM Autor WHERE jmeno='Karel' AND prijmeni='Čapek' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='369-66-823-1456-4') as id_titulu, id_zanru FROM Zanr WHERE nazev='Science fiction' FETCH FIRST 1 ROWS ONLY;

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'R.U.R.', '', 1900, 6, 'Odeon', '978-80-7033-157-6', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='978-80-7033-157-6') as id_titulu, id_autora FROM Autor WHERE jmeno='Karel' AND prijmeni='Čapek' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='978-80-7033-157-6') as id_titulu, id_zanru FROM Zanr WHERE nazev='Science fiction' FETCH FIRST 1 ROWS ONLY;

INSERT INTO Titul VALUES(DEFAULT, 'kniha', 'Válka s Mloky', '', 1958, 10, 'Československý spisovatel', '7886-364-53-366', NULL);
INSERT INTO Titul_autor (id_titulu, id_autora) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='7886-364-53-366') as id_titulu, id_autora FROM Autor WHERE jmeno='Karel' AND prijmeni='Čapek' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_zanr (id_titulu, id_zanru) SELECT (SELECT id_titulu FROM Titul WHERE ISBN='7886-364-53-366') as id_titulu, id_zanru FROM Zanr WHERE nazev='Science fiction' FETCH FIRST 1 ROWS ONLY;



/* Doplnění vazební tabulky mezi Žánrem a Autorem */
INSERT INTO Zanr_autor
    SELECT * FROM
        (SELECT DISTINCT Titul_zanr.id_zanru, Titul_autor.id_autora
        FROM Titul_autor
        LEFT JOIN Titul_zanr
        ON Titul_autor.id_titulu = Titul_zanr.id_titulu) table1
    WHERE NOT EXISTS
        (SELECT * FROM Zanr_autor table2 WHERE table2.id_zanru = table1.id_zanru AND table2.id_autora = table1.id_autora);





/* Přidání Časopisů */
-- Můj bylinkový diář --
INSERT INTO Titul VALUES(DEFAULT, 'časopis', 'Můj bylinkový diář', '', 2018, 5, 'Poznání', NULL, '2307-7301');
INSERT INTO Titul_tema (id_titulu, id_tema) SELECT (SELECT id_titulu FROM Titul WHERE ISSN='2307-7301') as id_titulu,
    id_tema FROM Tema WHERE nazev='Zahrada' FETCH FIRST 1 ROWS ONLY;
INSERT INTO Titul_tema (id_titulu, id_tema) SELECT (SELECT id_titulu FROM Titul WHERE ISSN='2307-7301') as id_titulu,
    id_tema FROM Tema WHERE nazev='Hobby' FETCH FIRST 1 ROWS ONLY;








/* Přidání Výtisků */
-- Spalovač mrtvol
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'vypůjčen', TO_DATE('2010-06-07', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='456-25-951-7853-6';
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'skladem', TO_DATE('2011-03-02', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='448-55-456-2589-3';
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'skladem', TO_DATE('2009-05-04', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='448-55-456-2589-3';
-- Audience
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'rezervován', TO_DATE('2002-03-04', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='978-80-7483-080-8';
-- Babička
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'vyřazen', TO_DATE('2002-09-01', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='698-96-637-2469-4';
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'skladem', TO_DATE('2005-04-12', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='9598-56-657-0154-2';
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'vypůjčen', TO_DATE('2011-07-03', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='9598-56-657-0154-2';
-- R.U.R.
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'skladem', TO_DATE('2009-05-04', 'YYYY-MM-DD'), NULL, id_titulu FROM Titul WHERE ISBN='978-80-7033-157-6';
-- Válka s Mloky
INSERT INTO Vytisk (stav, datum_pridani, datum_vyrazeni, id_titulu) SELECT 'vypůjčen', NULL, NULL, id_titulu FROM Titul WHERE ISBN='7886-364-53-366';





/* Přidání Rezervací */
INSERT INTO Rezervace (stav, rezervovano_od, rezervovano_do, id_titulu, id_vypujcky, id_ctenare, id_pracovnika_zrusil) 
SELECT 'platná', TO_DATE('2021-03-03', 'YYYY-MM-DD'), TO_DATE('2023-05-04', 'YYYY-MM-DD'), id_titulu, NULL, 1, NULL 
FROM Titul WHERE ISBN='978-80-7483-080-8';
-- R.U.R.
INSERT INTO Rezervace (stav, rezervovano_od, rezervovano_do, id_titulu, id_vypujcky, id_ctenare, id_pracovnika_zrusil) 
SELECT 'ukončena', TO_DATE('2021-05-03', 'YYYY-MM-DD'), TO_DATE('2021-05-04', 'YYYY-MM-DD'), id_titulu, NULL, 1, 1
FROM Titul WHERE ISBN='369-66-823-1456-4';
-- Válka s Mloky
INSERT INTO Rezervace (stav, rezervovano_od, rezervovano_do, id_titulu, id_vypujcky, id_ctenare, id_pracovnika_zrusil) 
SELECT 'ukončena', TO_DATE('2021-07-03', 'YYYY-MM-DD'), TO_DATE('2021-09-04', 'YYYY-MM-DD'), id_titulu, NULL, 2, NULL
FROM Titul WHERE ISBN='7886-364-53-366';
-- Babička
INSERT INTO Rezervace (stav, rezervovano_od, rezervovano_do, id_titulu, id_vypujcky, id_ctenare, id_pracovnika_zrusil) 
SELECT 'ukončena', TO_DATE('2021-01-01', 'YYYY-MM-DD'), TO_DATE('2021-09-01', 'YYYY-MM-DD'), id_titulu, NULL, 4, NULL
FROM Titul WHERE ISBN='9598-56-657-0154-2';
-- Spalovač mrtvol
INSERT INTO Rezervace (stav, rezervovano_od, rezervovano_do, id_titulu, id_vypujcky, id_ctenare, id_pracovnika_zrusil) 
SELECT 'ukončena', TO_DATE('2021-02-01', 'YYYY-MM-DD'), TO_DATE('2021-10-01', 'YYYY-MM-DD'), id_titulu, NULL, 5, NULL
FROM Titul WHERE ISBN='456-25-951-7853-6';
-- Spalovač mrtvol
INSERT INTO Rezervace (stav, rezervovano_od, rezervovano_do, id_titulu, id_vypujcky, id_ctenare, id_pracovnika_zrusil) 
SELECT 'ukončena', TO_DATE('2021-03-01', 'YYYY-MM-DD'), TO_DATE('2021-11-01', 'YYYY-MM-DD'), id_titulu, NULL, 3, NULL
FROM Titul WHERE ISBN='448-55-456-2589-3';



/* Přidání Výpůjček */
INSERT INTO Vypujcka VALUES(DEFAULT, 'vypůjčeno', TO_DATE('2021-03-20', 'YYYY-MM-DD'), TO_DATE('2021-7-04', 'YYYY-MM-DD'),  9, 3, 2, 2, NULL); --Válka s mloky
INSERT INTO Vypujcka VALUES(DEFAULT, 'vypůjčeno', TO_DATE('2021-03-20', 'YYYY-MM-DD'), TO_DATE('2021-8-04', 'YYYY-MM-DD'),  6, 4, 4, 3, NULL); -- Babička, vydani 2
INSERT INTO Vypujcka VALUES(DEFAULT, 'vypůjčeno', TO_DATE('2021-03-26', 'YYYY-MM-DD'), TO_DATE('2021-10-04', 'YYYY-MM-DD'), 1, 5, 5, 4, NULL); -- Spalovac mrtvol, vydani 1
INSERT INTO Vypujcka VALUES(DEFAULT, 'vráceno', TO_DATE('2021-03-10', 'YYYY-MM-DD'), TO_DATE('2021-03-28', 'YYYY-MM-DD'),   2, 6, 3, 5, 3); -- Spalovac mrtvol, vydani 3





--zobrazeni tabulek--
SELECT * FROM Pracovnik;
SELECT * FROM Ctenar;
SELECT * FROM Tema;
SELECT * FROM Zanr;
SELECT * FROM Autor;
SELECT * FROM Titul;
SELECT * FROM Vytisk;
SELECT * FROM Rezervace;
SELECT * FROM Vypujcka;
