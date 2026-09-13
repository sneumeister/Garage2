# Garage2

Garagentorsteuerung mit ESP32 und Web-Frontend

Dieses Projekt ergänzt die Handsender eines SOMFY Garagentorantriebs um ein Web-Frontend, welches per Mobil-Telefon oder PC-Browser angesprochen werden kann.
Dabei läuft auf einem ESP32 ein Web-Server, welche die Tor-Stellung anzeigt und durch einen virtuellen Button ein Impuls an den Garagentorantrieb gibt.
Im Endeffekt wird der manuelle Schalter durch den ESP32 betätigt.

Die Torstellung wird dabei durch einen Spannungsteiler (10k Poti) an einem der ADC-Eingänge des ESP32 realisiert.
CAD+STL Daten für Winkelgebergehäuse finden sich im Verzeichnis [Winkelgeber-Gehaeuse](/Winkelgeber-Gehaeuse/)

Durch das web-Frontende und entsprechende HTTP-GET und HTTP-POST Aufrufe, lässt sich diese Garagentorsteuerung dann auch via Home-Automation-Systeme (z.B. NodeRED) in andere Grafische Front-Ends integrieren.

Bilder meine Installation im [Pictures](/Pictures) Verzeichnis.

Schematische Darstellung der Schaltung und Stückliste (BOM) im [Schematics](/Schematics) Verzeichnis.

# Entwicklungsumgebung

Primär wird mit **VS Code/Cursor** und **pioarduino** (PlatformIO) gebaut.
Der Sketch bleibt eine `.ino`-Struktur und läuft **weiterhin unter Arduino IDE 2.x**, sofern Board-Paket und Libraries den Angaben unten entsprechen.

## Build mit pioarduino (empfohlen)

Voraussetzung: [pioarduino IDE](https://marketplace.visualstudio.com/items?itemName=pioarduino.pioarduino-ide) (VS Code/Cursor) oder PlatformIO-CLI.

Platform, Board, Framework, Flash/SPIFFS/Partitionen, Libraries und OTA-Env stehen in [`platformio.ini`](platformio.ini).

Schreiben via USB:

```bash
pio run -t upload
pio run -t uploadfs
```

Schreiben via OTA (Over The Air):

```bash
pio run -e esp32dev_ota -t upload
pio run -e esp32dev_ota -t uploadfs
```

**Hinweis:** Das OTA-Passwort liegt als Beispiel im Klartext bzw. als MD5-Hash in
[`Source/Garage2/myArduinoOTA.h`](Source/Garage2/myArduinoOTA.h) und für pioarduino
zusätzlich in [`platformio.ini`](platformio.ini) (`upload_flags` / `--auth=`).
Vor dem produktiven Kompilieren und Flashen sollte das Passwort geändert und an
beiden Stellen konsistent gesetzt werden.

## Build mit Arduino IDE 2.x

Sketch öffnen: [`Source/Garage2/Garage2.ino`](Source/Garage2/Garage2.ino) (Ordner `Source/Garage2/`).

### Board-Paket

- **esp32 by Espressif Systems**, Version **3.x** (Arduino-ESP32 Core 3 / IDF 5)
- Core **2.x** ist für diesen Stand ungeeignet (`esp_adc/adc_oneshot.h` u. a.)

### Board-Einstellungen

| Einstellung | Wert |
| --- | --- |
| Board | ESP32 Dev Module |
| Upload Speed | 921600 (bei Problemen niedriger) |
| Flash Size | 4MB (32Mb) |
| Partition Scheme | Default (mit OTA-App-Partitionen) |
| Port | COMx (USB) bzw. Netzwerk-Port (OTA) |

### Benötigte Libraries (Library Manager)

| Library | Hinweis |
| --- | --- |
| ArduinoJson | **7.x** (bblanchon) |
| ESP Async WebServer | Paket von **ESP32Async** (nicht ältere me-no-dev-Forks) |
| Async TCP | Paket von **ESP32Async** (Dependency der WebServer-Library) |

WiFi, SPIFFS, DNSServer, ArduinoOTA kommen mit dem ESP32-Core — nicht separat installieren.

### SPIFFS / Web-Dateien

Inhalt von [`Source/Garage2/data/`](Source/Garage2/data/) wie bisher per Plugin (z. B. „ESP32 Sketch Data Upload“) flashen. Die Arduino-IDE kennt PlatformIO-`uploadfs` nicht.

### Hinweis

[`platformio.ini`](platformio.ini) wird von der Arduino-IDE ignoriert. Library- und Core-Versionen dort manuell pflegen.

**Hinweis:** Das OTA-Passwort liegt als Beispiel im Klartext bzw. als MD5-Hash in
[`Source/Garage2/myArduinoOTA.h`](Source/Garage2/myArduinoOTA.h).
Vor dem produktiven Kompilieren und Flashen sollte das Passwort geändert werden.

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

- Standard AP SSID: ***Garage_AP***
- Standard AP Pass: ***garage_pass***

Diese Vorgaben sind in der Datei [config.json](Source/Garage2/data/config.json) hinterlegt. Eine Änderung via [Config-Menü](README.md#konfiguration-zur-laufzeit) zur Laufzeit ist nicht vorgesehen.

Somit kann dann z.B. mit dem Mobil-Telefon nach der SSID "Garage_AP" gesucht und mit Passwort "garage_pass" ein Verbindung hergestellt werden. Falls Sie nicht direkt auf die Konfigurations-Seite gelangen (Landing Page), dann im Web-Browser irgendeine Web-Adresse eingeben. Der integrierte DNS-Server liefert immer die selbe URL zurück.

## Konfiguration zur Laufzeit

Ergänzen Sie den [Basis-Aufruf](#basis-aufruf) um "/config" und Sie gelangen ins Konfigurations-Menü.

z.B. [http://garage/config](http://garage/config)  oder beispielhaft [http://192.168.178.70/config](http://192.168.178.70/config)

![ConfigSSID](Pictures/config02.gif)

SSID und Passwort von bis zu 2 Access-Points. Die Garagensteuerung versucht nacheinander einen der beiden AP zu kontaktieren. Sind SSID und Passwort leer, dann wird dieser Eintrag übersprungen. Die Settings für SSID1 und SSID2 können getrennt mit den entsprechenden Button gespeichert werden.

![ConfigHostname](Pictures/config03.gif)

Hostname, der zum Login an vorhandes WLAN benuztzt wird (falls vom Access-point unterstützt; sonst im Router konfigurieren).

![ConfigLevel](Pictures/config04.gif)

Die Werte des Analaog-Digital-Converters dienen zur grafischen Darstellung der Torstellung. Dabei ist es egal, wie der Poti verdrahtet ist, also ob ***door_up***&nbsp;>&nbsp;***door_down*** oder ***door_up***&nbsp;<&nbsp;***door_down***. Dies wird von der Software bereücksichtigt. Um die Konfiguration zu vereinfachten, kann der aktuelle ADC-Wert per Button ausgelesen und ins entsprechende Feld eingetragen werden. Sprich: Tor in entsprechende Stellung fahren und Wert auslesen durch ***Akt. ADC Wert***.
<table>
  <tr>
    <td><img src="Pictures/garage1_small.gif" Alt="ConfigDoor_Up"></td>
    <td><b><i>ADC-Wert</i></b><br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Up<br>
      .<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Middle<br>
      .<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Down<br>
      .
    </td>
  </tr>
  <tr>
    <td><img src="Pictures/garage2_small.gif" Alt="ConfigDoor_Not_Up"></td>
    <td>.<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Up<br>
      <b><i>ADC-Wert</i></b><br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Middle<br>
      .<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Down<br>
       .
    </td>
  </tr>
  <tr>
    <td><img src="Pictures/garage3_small.gif" Alt="ConfigDoor_Not_Down"></td>
    <td>.<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Up<br>
      .<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Middle<br>
      <b><i>ADC-Wert</i></b><br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Down<br>
      .
    </td>
  </tr>
  <tr>
    <td><img src="Pictures/garage4_small.gif" Alt="ConfigDoor_Down"></td>
    <td>.<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Up<br>
      .<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Middle<br>
      .<br>
                       &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Door_Down<br>
      <b><i>ADC-Wert</i></b>
  </tr>
</table>

# Hard-Coded Konfiguration

Folgende Daten sind fest im Programm hinterlegt und vor dem Compiler-Aufruf anzupassen. Eine Änderung während der Laufzeit ist nicht möglich.

## Version Info

Datei: [_sketchversion.h](Source/_sketchversion.h)

```text
#define SKETCHNAME    "Garagensteuerung"
#define SKETCHVERSION "$Ver 2.0"
#define SKETCHDATE    "2022-02-06"
```

Versions Informationen, die mittels `http://garage/version` abgefragt werden können; zusätzlich Datum/Zeit des Compiler-Runs

## OTA Update Passwort (Over-The-Air Update Passwort)

Datei: [myArduinoOTA.h](Source/myArduinoOTA.h)

```text
#define PASSWORD_HASH  "f8695e6ee86ea0b76ebbbe9023f6ae70"; // MD5-Hash
// oder
#define PASSWORD_TEXT  "Garagensteuerung!";                // Passwort im Klartext
```

Over-The-Air Update via ArduinoIDE; Passwort als MD5-Hash *oder* im Klartext

## Hardware: Relais, LED, ADC

Datei: [hardwareRelated.h](Source/hardwareRelated.h)

```text
const int            cfg_relais_pin    = 25;             //  An diesem Pin hängt das Relais....
const int            cfg_relais_active = LOW;            // Schliesst das Relais bei LOW oder HIGH ?

const int            cfg_signal_led    = 13;             //  An diesem Pin hängt die Signal-LED
const int            cfg_signal_active = HIGH;           //  Leuchtet die LED bei HIGH oder LOW?

const adc1_channel_t cfg_adc_input     = ADC1_CHANNEL_4; //  ADC-Kanal hier: ADC1 channel 4 an GPIO32
```

# Signal-LED

Da der mein ESP32U keine programmierbare on-board LED hat, ist eine zusätzliche LED auf der Platine verbaut. Diese dient zur Visualisierung verschiedener Events. Dies geschieht durch kurzes und langes aufblinken der LED. In der folgenden Tabelle symbolisiert...<br>
*&ndash;* = langes Aufblitzen<br>
*.*       = kurzes Aufblitzen

|Event                                | Signal                                  |
| ---                                 | ---                                     |
| "/doorlevel" Abfrage                | .&nbsp;.&nbsp;.                         |
| "/push_the_button" Anforderung      | &ndash;&nbsp;&ndash;&nbsp;&ndash;       |

# Integration in Home-Automation-Systeme

Im Folgenden ein paar nutzliche URL-Aufrufe, die zur Integration in Systeme wie [Node-RED](https://nodered.org/) benutzt werden.
Für die Beispiele wird von einen Hostnamen "garage" ausgegangen.

## Data: ***Version***

- HTTP-URL: http://garage/version
- HTTP-Methode: *GET*
- HTTP-Parameter: ---
- Response-MIME-Type: *text/plain*
- Bemerkung: Versions-Informationen aus [_sketchversion.h](Source/_sketchversion.h) und Kompilierungsdatum
- Beispiel-Ausgabe:<br> `Garagensteuerung $Ver 2.0.1 (2022-03-26)`<br> `Compiled 2022-03-26 - 17:00:19`

## Data: ***RSSI-Wert***

- HTTP-URL: http://garage/raw_rssi
- HTTP-Methode: *GET*
- HTTP-Parameter: ---
- Response-MIME-Type: *text/plain*
- Response-Code: *200*
- Bemerkung: Gibt den RSSI der WiFi-Verbindung zurück.
- Beispiel-Ausgabe: `-80`

## Data: ***ADC-Wert***

- HTTP-URL: http://garage/raw_adc
- HTTP-Methode: *GET*
- HTTP-Parameter: ---
- Response-MIME-Type: *text/plain*
- Response-Code: *200*
- Bemerkung: Gibt den aktuellen Wert des Analog-Digital-Converters zurück und damit Winkelstellung des Tores.
- Beispiel-Ausgabe: `4095`

## Data: ***Tor-Stellung***

- HTTP-URL: http://garage/doorlevel
- HTTP-Methode: *GET*
- HTTP-Parameter: ---
- Response-MIME-Type: *text/plain*
- Response-Code: *200*
- Bemerkung: Ein Wert zwischen 0 und 3, der die aktuelle Stellung des Tores darstellt. Man könnte auch "bewerteter ADC-Wert" sagen.
  - 0 = komplett offen
  - 1 = nicht ganz offen
  - 2 = nicht ganz geschlossen
  - 3 = komplett geschlossen
- Beispiel-Ausgabe: `0`

## Data: ***Knopf-druck senden***

- HTTP-URL: http://garage/push_the_button
- HTTP-Methode: *POST*
- HTTP-Parameter: action=push
- Response-MIME-Type: *text/plain*
- Response-Code:
- *202* = OK
- *400* = Fehler
- Bemerkung: Der Aufruf mit dem Parameter "action=push" schaltet das Relais zum Öfnne/Schliessen des Garagentors.
- Beispiel-Ausgabe:
  - `Done.` mit Response-Code *202*, wenn OK.
  - `Error: wrong magic word!` mit Response-Code *400*, wenn Fehlgeschlagen.

## Data: ***Hallo***

- HTTP-URL: http://garage/hello
- HTTP-Methode: *GET*
- HTTP-Parameter: --
- Response-MIME-Type: *text/plain*
- Response-Code: *200*
- Bemerkung: Kein tieferer Sinn.... :wink:
- Beispiel-Ausgabe: `Hello World`

# Versionshistorie

| Version | Datum | Änderungen |
| --- | --- | --- |
| 2.1.0 | 2026-09-13 | Umstellung auf pioarduino/PlatformIO (VS Code/Cursor); ArduinoJson 7; ADC über `esp_adc/adc_oneshot` (wertgleiche Raw-Counts); SoftAP-Abschaltung nach STA-Connect mit 5‑Minuten-Schonfrist bei verbundenen SoftAP-Clients; Signal-LED-Queue gehärtet; Build weiterhin mit Arduino IDE 2.x möglich |
| 2.0.3 | 2022-11-20 | `waitms()` durch `vTaskDelay()` ersetzt; littleHelpers entfernt; Debug-`#define`s bereinigt; Signal-LED über eigenen FreeRTOS-Task |
| 2.0.x | 2022 | Version mit Config-Datei und Captive Portal |

Detailänderungen siehe Git-Historie und Kommentare in [`Source/Garage2/_sketchversion.h`](Source/Garage2/_sketchversion.h).
