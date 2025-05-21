Implementácia útoku Brightness

V priečinku sa nachádza zdrojový kód na úpravu pixelov, čím bude efektívne exfiltrovať dáta. Zároveň v podpriečinku processData sa nachádzajú súbory na úpravu videa zachytávajúceho exfiltráciu.

V kóde Brightness.cpp sa nachádzajú vo funkcii apply_filter premenné, ktoré je možné meniť. Sú to:
    
addred, addblue, addgreen - konštanty filtra pre vysielanie hodnoty 1

addred2, addblue2, addgreen2 - konštanty filtra pre vysielanie hodnoty 0

pixelHeight, pixelWidth - šírka a výška pixelov, ktoré budú exfiltrovať dáta

fromX, fromY - posícia od ktorej bude pixelHeight a pixelWidth počítaná 

V súboroch stačí pre úspešné spustenie definovať názov súboru na úpravu a názov výsledného súboru.
