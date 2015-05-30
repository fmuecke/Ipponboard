
Programmbeschreibung
====================

ist ein fortschrittliches Kampfzeit- und Kampfpunkt-Anzeigesystem, das
speziell für den Judowettkampf entwickelt wurde. Bei der Entwicklung
wird auf die folgenden Punkte besonderes Augenmerk gelegt:

-   ausgezeichnete Lesbarkeit
-   einfache Bedienung
-   unkomplizierter Einsatz

Das Programm wird prinzipiell von einem PC (Laptop) aus bedient und mit
Gamepad oder Maus gesteuert. nutzt zur Darstellung zwei Anzeigen, eine
externe für die Kämpfer, Betreuer, Publikum und eine für die Zeitnehmer.
Die Anzeige der Zeitnehmer ist dabei gespiegelt dargestellt, damit sie
den Kämpfern besser zugeordnet werden kann.

## Anzeige
Die Anzeige besteht im Wesentlichen aus fünf Bereichen:  
-   *Kampfzeit*: Diese befindet sich am unteren Rand der Anzeige. Ist
    der Kampf unterbrochen wird die Kampfzeit *rot* dargestellt,
    ansonsten *gelb*.
-   *Wertungen*: Die Wertungen sind auf der Seite des jeweiligen
    Kämpfers gruppiert und in der jeweiligen Farbkombination gehalten
    (*Weiß auf Blau* bzw.* Schwarz auf Weiß*). Die Strafen sind durch
    *rote Punkte* symbolisiert.
-   *Haltegriffzeit*: Die noch zu haltende Zeit wird im oberen Bereich
    dargestellt und ist durch eine Sanduhr symbolisiert. Die Farben
    werden dabei dem Haltenden zugeordnet.
-   *Kampfinformationen* (Mattennummer, aktuelle Gewichtsklasse)
-   *Namen der Kämpfer* (können auch leer gelassen werden)

### Primäre Anzeige

![Primäre Anzeige](images/primary_view-DE.jpg)

Die primäre Anzeige dient als zentrale Steuereinheit. Auf ihr sind alle
Informationen verfügbar und einstellbar:

-   Kampf- und Haltegriffzeit starten/stoppen/(zurück-)setzen
-   Kampf zurücksetzen
-   Namen der Kämpfer ändern
-   Wertungen setzen/zurücknehmen
-   Kampfinformationen (Mattennummer, aktuelle Gewichtsklasse) ändern

### Sekundäre (externe) Anzeige

Im Unterschied zur primären Anzeige werden auf der sekundären nur die
für das Kampfgeschehen wesentlichen Details angezeigt:

-   nur die Wertungen bis Waza-ari (Ippon wird blinkend darübergelegt)
-   nur die aktiven Strafen
-   nur die aktive Haltegriffzeit

Eine weitere Einschränkung der sekundären Anzeige ist, dass sie bewusst
nicht auf Eingaben mit der Maus reagiert. Ob die zweite Anzeige beim
Programmstart gleich angezeigt werden soll, oder auf welchem Bildschirm
diese ausgegeben wird, lässt sich in den *Programmeinstellungen*
festlegen. Wie der Rechner für den Zweischirmbetrieb (“Dual-View”)
konfiguriert werden muss, kann im Anhang: *Computer für
Zweischirmbetrieb vorbereiten* nachgelesen werden.

Einstellungen {#bkm:Programmeinstellungen}
-------------

Die Programmeinstellungen finden sind im Anwendungsmenü unter
Einstellungen zu finden. Sie bieten Zugriff auf verschiedene
*allgemeine* Optionen zur Anpassung des Programms:

-   Sekundäre Anzeige konfigurieren
-   Farben und Schriftart für den Info-Bereich ändern
-   Farben für Kämpfer/Wertungen ändern
-   Sounddatei für das Signal des Zeitnehmertisches auswählen

Neben den allgemeinen Optionen lassen sich im Einstellungsmenü auch die
Knöpfe des Gamepads neu belegen.

Steuerung
=========

Gamepad-Steuerung
-----------------

Die Steuerung über die Maus mag zunächst einfacher erscheinen, da dieses
Eingabegerät für die meisten vertraut ist und man die Anzeige intuitiv
via Klicks ansprechen kann.

Unsere Erfahrungen haben jedoch gezeigt, dass mit dem Gamepad ein
wesentlich entspannteres Bedienen als mit der Maus möglich ist. Daher
möchten wir Ihnen die Steuerung mit dem Gamepad mit folgenden Gründen
besonders ans Herz legen:

### Aktionen

##### Punkte vergeben und zurücknehmen

Die Punkte werden über die beiden Analog-Sticks vergeben. Dabei
entsprechen für den *blauen* Kämpfer (linker Daumen!) folgende
Richtungen den jeweiligen Punkten:

Für den weißen Kämpfer sind die Richtungen einfach spiegelverkehrt.

##### Zeit starten/stoppen (Hajime/Matte)

Die Hauptzeit wird mittels der *Nach-Unten*-Taste des Drehkreuzes des
Gamepads gestartet oder gestoppt.

##### Haltegriffzeit starten/stoppen (Osaekomi / Toketa)

Die Haltegriffzeit wird in der Standardeinstellung durch Drücken der
hinteren oberen Feuertaste (Knopf 7 und Knopf 8) gesetzt. Dabei ist die
linke für den blauen und die rechte für den weißen Kämpfer. Durch
nochmaliges Drücken der Haltegrifftaste wird der Haltegriff angehalten
(Toketa). Wird die Taste des anderen Kämpfers gedrückt, wird die Zeit
für diesen übernommen.

##### Haltegriffzeit zurücksetzen

Dies erfolgt mit den hinteren unteren Feuertasten. Dabei haben beide
Tasten die selbe Funktion.[^3]

##### Anzeige zurücksetzen

Um alle Werte zurückzusetzen, müssen die dafür definierten Knöpfe
*gleichzeitig* gedrückt werden (Knopf 1 + Knopf 4).

### Tastenbelegung konfigurieren

![Übersicht Knopfbelegung](images/p380.jpg)

[fig:Belegung]

Die Belegung der Knöpfe kann unter in den Programmeinstellungen unter
dem Reiter *Eingabe* geändert werden (Abb. [fig:Gampad~S~ettings]). Die
aktuelle Belegung ist in Abbildung [fig:Belegung] bildlich dargestellt.

Maus-Steuerung
--------------

Das Programm kann komplett mit der Maus gesteuert werden. Hierfür muss
lediglich auf die jeweiligen Felder in der primären Anzeige oder auf die
entsprechenden Knöpfe in der Oberfläche geklickt werden.

### Aktionen

##### Punkte vergeben und zurücknehmen

Um Punkte zu vergeben bzw. diese wieder zurückzunehmen muss lediglich in
das jeweilige Feld geklickt werden. Dabei gilt Folgendes:

-   *Punkt geben:* linke Maustaste

-   *Punkt zurücknehmen:* rechte Maustaste

##### Zeit starten/stoppen (Hajime/Matte)

Die Kampfzeit kann mit Linksklick gestartet und gestoppt werden. Ist die
Zeit angehalten ist sie *rot*, läuft die Uhr dagegen ist sie *gelb*.

##### Haltegriffzeit starten/stoppen (Osaekomi / Toketa)

Zum Starten der Haltezeit muss auf das “00”-Feld neben der Sanduhr
geklickt werden. Der Haltegriff wird hierbei zunächst automatisch für
Blau angezeigt. über das Kontextmenü dieses Feldes (Rechtsklick) kann
der Haltegriff dann Weiß zugeordnet werden, falls nötig. Erneutes
Anklicken des Feldes mit links stoppt die Haltegriffzeit. Die Zeit wird
jetzt so lange angezeigt, bis entweder erneut ein Haltegriff ausgelöst
wird, oder die Hauptzeit nach Anhalten wieder läuft (=*Hajime*).


[^1]: Die genauen Änderungen zur jeweiligen Vorgängerversion sind auf
    der Projektseite im Internet aufgelistet.

[^2]: Die Steuerung über die Tastatur ist für spätere Versionen geplant.

[^3]: Die erste Version konnte die Zeit automatisch bei *Hajime* oder
    erneutem *Osaekomi* zurücksetzen. Da dies aber nicht dem gewohnten
    Bedienverhalten entspricht, wurde dies dahingehend geändert, dass
    die Haltegriffzeit nun manuell zurückgesetzt werden kann und muss.
