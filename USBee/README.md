Implementácia útoku USBee

V tomto priečinku sa nachádzajú:
- výsledky implementácie (obr. usbee-init, usbee-bezKabla, usbee-sKabom)
- obrázok použitéj pásmovéj zádrže a zosilovača
- zdrojový kód, ktorý generuje podľa vstupu súbor pripravený na exfiltrovanie. Vstup do funkcie je názvu výsledného súboru, veľkosti, a frekvencia, na ktorej bude možné zachytiť signál pri prenose súboru na USB kľúč.

Tento zdrojový kód obsahuje funkciu, ktorá je volaná nasledovne:

fill_buffer_freq("output.bin", 1, 4600)

kde, output.bin je názov výsledného súboru, 1 je veľkosť súboru v Gb, 4 600 je frekvencia v kHz (je to násobok 100 kHz = výslená frekvencia bude 460 MHz) na ktorej bude vysielaný signál
