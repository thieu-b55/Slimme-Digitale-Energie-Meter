# Slimme-Digitale-Energie-Meter met ESP32 Devkit module geprogrammeerd met Arduino IDE

Gebruik van P1 poort voor het inlezen van elektrisch verbruik en eventueel sturen in functie van vermogen afkomstig van zonnepanelen.

Voor elk programma de keuze tussen ESP32 in Access Point mode (ESP32 eigen netwerk) of Station Mode ESP32 geïntegreerd in je huisnetwerk.

Slimme_meter_web_AP.ino / Slimme_meter_web_STA.ino

Eenvoudigste versie

Totaal verbruik, totaal injectie, totaal gas (ndien digitale gasmeter), verbruik nu en injectie nu worden doorgestuurd naar webpagina

Slimme_meter_esp32_AP.ino / Slimme_meter_esp32_STA.ino

Zelfde mogelijkheden als hierboven

Weergave data op LCD scherm indien gewenst.

Tevens de mogelijkheid voor het sturen van 3 digitale uitgangen in functie van opgewekt vermogen of op tijdbasis.

PWM uitgang om bijvoorbeeld een boiler op te warmen in functie van opgewekt vermogen of op tijdbasis. Kan eventueel ook gebruikt

worden om een kleine huishoudboiler op te warmen die is aangesloten op de warmwateringang van een hotfill wasmachine of vaatwasser.

Slimme_meter_esp32_data_AP.ino / Slimme_meter_esp32_data_STA.ino

Zelfde mogelijkheden als hierboven maar kan de gegevens opslaan op een SD kaart en terug zichtbaar maken op een webpagina

Gegevens worden opgeslagen op uur, dag, maand en jaarbasis.

1 jaar volledige dataopslag gebruikt +/- 140KB op een SD kaart. Op een SD kaart van 16GB kan je al wel enkele (honderden) jaren bewaren.

