# Project Tittel

Prosjektoppgave gruppe 6: kontaktløs antibacdispenser for å minske faren for smitte.
Ta den ene hånden over sensoren mens den andre hånden er under tuten på dispenseren for å få produkt.
Nivå oversikt og tilkoblede klienter og vedlikehold avleses og styres fra en nettside.

## Beskrivelse

Prosjektet består av en antibacdispenser som blir styrt Vha. en ultralyd sensor som brukes som motion sensor for kontaktløs bruk. 
I tillegg loggføres nivået i dispenseren på en nettside. På nettsiden kan man gå inn å se nivået akkurat nå, samt live grafer over nivået og når på døgnet dispenseren er mest brukt. 
Det er også en oversikt over tilkoblede klienter på nettsiden hvis man skulle ønske å utvide programmet til flere dispensere i ett bygg f.eks. med en felles oversikt.
Nettsiden har også en vedlikeholdsknapp som stopper programmet fra å gi ut produkt og sende målinger. Da kan man trygt kan gjøre vedlikehold på dispenseren, fylle i nytt innhold ol. 
uten at det påvirker målingene som sendes til serveren i perioden hvor vedlikehold er aktivert. Når man skrur av denne vil alt fungere som før.


## Oppstart

### Avhengigheter

for å bruke dette prosjektet må man ha:
* Arudino eller annen IDE som kan kjøre Arduino fil med h. header fil og .cpp fil for bibliotek.
* Flasken man bruker må ha en form som gjør at volumet er tilnærmet lineært med nivået for å få pålitelige målinger. 

### Installering

* Zip ut programmappen til en ønsket plass på pc-en. 
* Last ned ESPservo Bibliotek på Arduino IDEen. Denne skal man kunne finne enkelt under skisse --> inkluder bibliotek --> administer bibliotek --> søk esp32 og laste ned ESP32Servo
* Flytt bibliotekfilene/mappene fra zip filen sammen med de andre Arduino bibliotekene på pc-en. De skal hete WiFiMulti og Dispenser. 	F.eks.: dokumenter --> arduino --> librarys. 


### Utfører programmet

* Når alle bibliotekene er på plass kan man gjøre klar til å kjøre programmet 
* Koble til alt utstyret og plugg deg opp på ESP32.
* Øverst i programmet må man endre ‘Stringene’ for SSID og PASSWORD. disse skal skrives til navnet på din Wifi og passordet. Hermetegnene må stå.
	* const char* SSID = "Navn på wifi";
	* const char* PASSWORD = "Password til wifi";
* La det som står under "basic arudino setup", "Wifi setup" og "dispenser setup" stå i void setup()
* dispenser.websocket.loop(); må stå i setup for å kunne få kontakt med server. 
* I vårt eksempelprogram ligger det en funksjon kaldt dispenser.dispenserProgram(); i void loop. dette er et eksempel på ett fungerende program som styrer dispenseren og sender og mottar verdier fra server.
* Med alt dette stående kan man laste opp til ESP og kjøre programmet (så lenger server er oppe å går). 
* Hvis man ønsker kan man bruke funksjonene i libraryet og sy sammen sitt eget program i stedet.
* Etter man har tatt i bruk programmet må man kalibrere flasken. 
	* Gå inn på nettsiden og trykk "maintenance"-knapp for vedlikehold. 
	* kommenter ut mapping funksjonen i FsrRead().
	* Plasser tom flaske på trykk sensor og les av verdien.
	* Plasser flasken med fult innhold og les av verdien.
	* Disse to verdiene skrives til #define DISP_FSRMAX og DISP_FSRMAX.
	* Nå skal trykksensoren gi ut en korrekt prosentverdi av nivået i flasken. (så lenge man bruker en sylinderformet flaske eller lignende)
	* Skru av vedlikeholdsknappen.  	
* Programmet skal være klart til bruk. 


## Forfattere

Studentgruppe 6


## Version History

* 0.1
    * Første utgivelse


## Anerkjennelser

inspirasjon til kodensutt for avlesning av ultralydmåleren.
https://www.instructables.com/Simple-Arduino-and-HC-SR04-Example/
