---
name: README Entwicklungsumgebung
overview: README.md um pioarduino/Arduino-IDE-2.x-Dokumentation und eine Versionshistorie erweitern — inkl. der vorgeschlagenen Wortlaute.
todos:
  - id: readme-build-section
    content: "README: Abschnitt Entwicklungsumgebung (pioarduino + Arduino IDE 2.x) mit Wortlaut einfügen"
    status: completed
  - id: readme-history
    content: "README: Versionshistorie am Ende anhängen"
    status: completed
isProject: true
---

# README: Entwicklungsumgebung, Arduino-IDE, Historie

Datei: [`README.md`](README.md). Bestehende Abschnitte (Web-UI, Config, URLs, Integration, Hard-Coded) bleiben unangetastet; der Block ab „# Build mit pioarduino“ wird ersetzt, Historie ans Ende.

---

## 1. Ersetzen: bisheriger Build-Kurzblock

**Entfernen** (aktuell ~Zeile 17–32): `# Build mit pioarduino` inkl. Kurztext.

**Einfügen** (vorgeschlagener Wortlaut):

````markdown
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
beiden Stellen konsistent gesetzt werden. Klartext im Repo zu belassen liegt in der
Verantwortung des Nutzers.

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
````

---

## 2. Neu am Ende der README: Versionshistorie

**Vorgeschlagener Wortlaut:**

```markdown
# Versionshistorie

| Version | Datum | Änderungen |
| --- | --- | --- |
| 2.1.0 | 2026-09-13 | Umstellung auf pioarduino/PlatformIO (VS Code/Cursor); ArduinoJson 7; ADC über `esp_adc/adc_oneshot` (wertgleiche Raw-Counts); SoftAP-Abschaltung nach STA-Connect mit 5‑Minuten-Schonfrist bei verbundenen SoftAP-Clients; Signal-LED-Queue gehärtet; Build weiterhin mit Arduino IDE 2.x möglich |
| 2.0.3 | 2022-11-20 | `waitms()` durch `vTaskDelay()` ersetzt; littleHelpers entfernt; Debug-`#define`s bereinigt; Signal-LED über eigenen FreeRTOS-Task |
| 2.0.x | 2022 | Version mit Config-Datei und Captive Portal |

Detailänderungen siehe Git-Historie und Kommentare in [`Source/Garage2/_sketchversion.h`](Source/Garage2/_sketchversion.h).
```

---

## 3. Bewusst nicht in die README

- Security-Findings (fehlende Auth, öffentliches `config.json`)
- Ausführliche Hinweise zu `compile_commands.json` (steht in `.gitignore`)
- Keine Überarbeitung des Abschnitts „Hard-Coded Konfiguration“ in diesem Plan

---

## Umsetzung

1. Build-Abschnitt ersetzen durch „Entwicklungsumgebung“ (Wortlaut oben)
2. Versionshistorie ans Dokumentende anhängen
