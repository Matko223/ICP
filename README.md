# ICP
Nástroj pre vizuálnu editáciu, generovanie kódu a monitorovanie behu interpretovaných konečných automatov.

## Autori

*   **xpalesr00**
*   **xsedot00**
*   **xvalapm00**

## Popis
- Aplikácia slúži na tvorbu, editáciu a simuláciu konečných Moore automatov pomocou grafického rozhrania.
- Používateľ môže vizuálne navrhovať automaty pomocou drag-and-drop prvkov a jednoducho definovať stavy s výstupnými hodnotami.
- Systém podporuje vytváranie prechodov medzi stavmi s možnosťou konfigurácie podmienok a časových oneskorení.

### Hlavné funkcie
- Interaktívna tvorba konečných automatov s drag-and-drop rozhraním
- Definovanie stavov s vlastnými výstupnými výrazmi
- Vytváranie prechodov medzi stavmi s podmienkami a časovými oneskoreniami
- Simulácia behu automatu s krokovaním
- Export a import návrhu automatu vo formáte JSON
- Generovanie kódu v C++, ktorý reprezentuje daný automat 

## Spustenie
- Aplikáciu je možné spustiť z koreňového adresára projektu príkazom `make run`
- Alternatívne možno najprv skompilovať príkazom `make` a následne spustiť binárku príkazom `./proj`

## Obmedzenia
- Neimplementované pripojenie cez UDP sockety 
- Nefunkčné operácie s premennými počas simulácie

## Použité knižnice
Na parsovanie JSON bola použitá knižnica nlohmann/json.