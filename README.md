# Garage2
Garagentorsteuerung mit ESP32 und Web-Frontend

Dieses Projekt ergänzt die Handsender eines SOMFY Garagentorantriebs um ein Web-Frontend, welches per Mobil-Telefon oder PC-Browser angesprochen werden kann.
Dabei läuft auf einem ESP32 ein Web-Server, welche die Tor-Stellung anzeigt und durch einen virtuellen Button ein Impuls an den Garagentorantrieb gibt.
Im Endeffekt wird der manuelle Schalter durch den ESP32 betätigt.

Die Torstellung wird dabei durch einen Spannungsteiler (10k Poti) an einem der ADC-Eingänge des ESP32 realisiert.
CAD+STL Daten für Winkelgebergehäuse finden sich im Verzeichnis [Winkelgeber-Gehaeuse](/Winkelgeber-Gehaeuse)

Durch das web-Frontende und entsprechende HTTP-GET und HTTP-POST Aufrufe, lässt sich diese Garagentorsteuerung dann auch via Home-Automation-Systeme (z.B. NodeRED) in andere Grafische Front-Ends integrieren.

# Ansicht im Web-Browser
## Animation mit simuliertem Push-Button
![AnimatedScreenPlay](Pictures/animationGIF.gif)
## Fehlerbilder
Temporärer Verbindungverlust: einzelne Statusabfragen an den Garagen-Server blieben ohne Antwort (Status also unbekannt).
Das Browser-Skript versucht aber weiterhin den Status abzufragen.
Ohne gültigen Garagen-Status wird der Button blockiert; symbolisiert durch Verbotszeichen).

![tmpConnectionLost](Pictures/tmpLostConnection.gif)

Dauerhafter Verbindungsverlust: Laufen mehrere Status-Abfragen an den Garagen-Server ins Leere (erhalten kein Antwort), dann wird das Abfrage-Skript im Browser komplett beendet und die Button-Abfrage dauerhaft deaktiviert.
In diesem Zusatnd erfolgt kein selbständiger Re-Connect zum Server; dies muss manuell durch ein Reload (Erneut laden) in Browser angestossen werden.

![brokenConnection](Pictures/brokenConnection.gif)


# URL Aufrufe
## Basis-Aufruf
Hat sich der ESP in Ihrem WLAN-Netz angemeldet, erfolgt der aufruf über die erhaltenen IP-Adresse oder den Hostnamen...

[http://garage](http://garage)  oder beispielhaft [http://192.168.178.70](http://192.168.178.70)
## Aufruf im Access-Point (AP) Modus
Sollte der ESP32 keine WLAN-Verbindung bekommen schaltet er automatisch zuusätzlich einen AP frei.
Sofern in der Konfiguration nicht geändert kann dann mit z.B. dem Mobil-Telefon nach der SSID "Garage_AP" gesucht und mit Passwort "garage_pass" ein Verbindung hergestellt werden. Falls Sie nicht direkt auf die Konfigurations-Seite gelangen (Landing Page), dann im Web-Browser irgendeine Web-Adresse eingeben.
## Konfiguration zur Laufzeit
Ergänzen Sie den [Basis-Aufruf](#basis-aufruf) um "/config" und Sie gelangen ins Konfigurations-Menü.

z.B. [http://garage/config](http://garage/config)  oder beispielhaft [http://192.168.178.70/config](http://192.168.178.70/config)

![ConfigScreen](Pictures/configscreen.gif)

<table cellspacing="0">
 <tr>
   <td> <img src="Pictures/config01.gif" alt="config01"></td>
   <td> SSID </td>
 </tr>
 <tr>
   <td> <img src="Pictures/config02.gif" alt="config02"></td>
   <td> SSID </td>
 </tr>
 <tr>
   <td> <img src="Pictures/config03.gif" alt="config03"></td>
   <td> SSID </td>
 </tr>
 <tr>
   <td> <img src="Pictures/config04.gif" alt="config04"></td>
   <td> SSID </td>
 </tr>
 <tr>
   <td> <img src="Pictures/config05.gif" alt="config05"></td>
   <td> SSID </td>
 </tr>

</table>


# Hard-Coded Konfiguration
Folgende Daten sind fest im Programm hinterlegt und vor dem Compiler-Aufruf anzupassen. Eine Änderung während der Laufzeit ist nicht möglich.
## Version Info
Datei: [_sketchversion.h](Source/_sketchversion.h)
```
#define SKETCHNAME    "Garagensteuerung"
#define SKETCHVERSION "$Ver 2.0"
#define SKETCHDATE    "2022-02-06"
```
Versions Informationen, die mittels `http://garage/version` abgefragt werden können; zusätzlich Datum/Zeit des Compiler-Runs

## OTA Update Passwort (Over-The-Air Update Passwort)
Datei: [myArduinoOTA.h](Source/myArduinoOTA.h)
```
#define PASSWORD_HASH  "f8695e6ee86ea0b76ebbbe9023f6ae70"; // MD5-Hash
// oder
#define PASSWORD_TEXT  "Garagensteuerung!";                // Passwort im Klartext
```
Over-The-Air Update via ArduinoIDE; Passwort als MD5-Hash *oder* im Klartext

## Hardware: Relais, LED, ADC...
Datei: [hardwareRelated.h](Source/hardwareRelated.h)
```
const int            cfg_relais_pin    = 25;             //  An diesem Pin hängt das Relais....
const int            cfg_relais_active = LOW;            // Schliesst das Relais bei LOW oder HIGH ?

const int            cfg_signal_led    = 13;             //  An diesem Pin hängt die Signal-LED
const int            cfg_signal_active = HIGH;           //  Leuchtet die LED bei HIGH oder LOW?

const adc1_channel_t cfg_adc_input     = ADC1_CHANNEL_4; //  ADC-Kanal hier: ADC1 channel 4 an GPIO32   
```
