# FlipperJammer Suite

8-Modul Jammer-App für Flipper Zero (Momentum Firmware, API 87.1).

## Module

| Modul | Was es macht | Modul-Hardware |
|---|---|---|
| **SubGHz** | Carrier / Noise / Sweep Jammer auf 315/433/868/915 MHz | CC1101 (intern oder ext) |
| **NRF24** | 2.4 GHz Constant Carrier Jammer (ALL/BT/Zigbee/WiFi/Logitech) | NRF24L01+ Multiboard |
| **BLE** | BLE Advertising Flooder (Kanäle 37/38/39) | NRF24L01+ Multiboard |
| **WiFi Deauth (L2)** | 802.11 Deauth-Frames via ESP32 — wirkt nicht bei WPA3/PMF | ESP32 mit Marauder FW |
| **IR** | IR-Jammer (Noise/TV/NEC/Samsung/Sony) | intern oder Rabbit Labs Blaster |
| **RFID** | 125 kHz LF Jammer/Spoofer/Fuzzer | Eingebaut |
| **NFC** | 13.56 MHz Jammer/Spoofer/Fuzzer | Eingebaut |
| **USB Fuzzer** | HID Flood / Malformed / Reconnect | USB Host |

## Features

- **Hardware-Erkennung** beim Start: ext CC1101 + NRF24 werden automatisch geprüft, Hauptmenü zeigt `[OK]` / `[!HW]`
- **Settings-Persistenz**: Modus/Frequenz/Power/Dauer überleben App-Neustart (`/ext/apps_data/jammer_suite/config.bin`)
- **NRF24 Power-Slider**: -18 / -12 / -6 / 0 dBm
- **NRF24 Diagnose-Modus**: Schritt-für-Schritt-Tester für LED-Verhalten + OTG-Status
- **Live Channel-Display**: 100ms-Update im NRF24/Sweep-Modus
- **ESP32-Status-Rückkanal**: zeigt Online-Status + RX-Bytes vom Marauder
- **Antennen-Umschaltung**: SubGHz INT/EXT wählbar
- **Scrollbare Info-Pages**: pro Modul ausführliche Doku zu Funktion, Reichweite, Grenzen

## Hardware-Voraussetzungen

### NRF24L01+ Modul

**Pinbelegung** (kompatibel mit REK5Lab v2, HW-237 Kasiin, Ebyte E01C-ML01SP4):

| NRF24 | Flipper GPIO | Symbol |
|---|---|---|
| CE   | Pin 6 | `gpio_ext_pb2` |
| CSN  | Pin 4 | `gpio_ext_pa4` |
| SCK  | Pin 5 | `gpio_ext_pb3` |
| MOSI | Pin 2 | `gpio_ext_pa7` |
| MISO | Pin 3 | `gpio_ext_pa6` |
| VCC  | Pin 9 | 3.3V (intern) ODER Pin 1 OTG-5V (für PA-Module) |
| GND  | Pin 11/18 | GND |

**Wichtig:**
- **PA+LNA Module** (z.B. Ebyte E01C-ML01SP4) ziehen bis 130mA bei +20dBm — der 3.3V-GPIO-Pin reicht nicht. Die App schaltet automatisch OTG-5V ein.
- Standard NRF24L01+ ohne PA läuft mit 3.3V.
- **Hardware-SPI** (`furi_hal_spi_bus_handle_external`) wird verwendet — kein Bit-Bang.
- **PC3 NFC-CS** wird HIGH gehalten während NRF24 aktiv — verhindert NFC-SPI-Konflikt.

### ESP32 (WiFi Deauth)

- UART: `LPUART1` (Pin 13/14 = PA2/PA3)
- Baud: 115200 8N1
- JSON-Protokoll: `{"cmd":"deauth","ch":1,"bcast":true}`
- ESP32 Marauder Firmware oder eigenes Deauth-Sketch (`resources/deauth_esp32/deauth.ino`)

### CC1101 (SubGHz)

- Eingebaut funktioniert immer
- Extern: REK5Lab oder FlipperMarket CC1101-Modul für mehr Reichweite

## Build

```bash
ufbt
```

Erfordert: Flipper Build Tool ([ufbt](https://github.com/flipperdevices/flipperzero-ufbt)) + Momentum SDK (API 87.1).

## Installation

```
copy dist\jammer_suite.fap F:\apps\GPIO\jammer_suite.fap
```

Anschließend Flipper neu starten und unter **Apps → GPIO → Jammer Suite** öffnen.

## Bedienung

### NRF24-Jammer starten

1. Hauptmenü → "2.4GHz NRF24" (`[OK]` = Modul erkannt, `[!HW]` = nicht erkannt aber Start trotzdem möglich)
2. Modus wählen (ALL für maximale 2.4 GHz Störung)
3. Power-Level setzen (0 dBm für max., -18 dBm für Lab-Tests)
4. Dauer wählen
5. Start drücken → OTG schaltet sich automatisch ein

### NRF24 Diagnose (LED testet nicht)

Hauptmenü → "NRF24 Diagnose"

| Schritt | Was passiert | Erwartet |
|---|---|---|
| 0 | Alles aus | LED aus |
| 1 | OTG 5V an | Power-LED sollte leuchten |
| 2 | PWR_UP=1 | Module-LED leuchtet |
| 3 | CE HIGH | TX-LED leuchtet (sofern vorhanden) |
| 4 | CONT_WAVE | Vollständiges Jamming aktiv |

Mit Rechts/Links durchschalten, Back zum Beenden.

## Was wirklich gejammt wird

| Ziel | Funktioniert? | Modul / Modus |
|---|---|---|
| WiFi 2.4 GHz (modern, WPA3) | ✓ | NRF24 ALL |
| WiFi 2.4 GHz (alt, WPA2 ohne PMF) | ✓ | ESP32 Deauth ODER NRF24 ALL |
| WiFi 5 GHz | ✗ | Hardware kann's nicht |
| BLE Advertising (neue Verbindungen) | ✓ | BLE-Modus (Kanal 37/38/39) |
| BT Classic Audio (A2DP) | ✓ | NRF24 ALL (überfordert AFH) |
| Bluetooth HID (verbunden) | teilweise | NRF24 ALL |
| Zigbee | ✓ | NRF24 ALL oder Zigbee-Modus |
| Drohnen (DSM2/DSMX/FlySky/FrSky) | ✓ | NRF24 ALL — PA+LNA empfohlen |
| Logitech Unifying Maus/Tastatur | ✓ | NRF24 Logitech |
| Generische 2.4 GHz Funkmäuse | ✓ | NRF24 ALL |
| 2.4 GHz Game-Pads | ✓ | NRF24 ALL |
| 5.8 GHz FPV Drohnen | ✗ | Falsches Band |
| DJI OcuSync 3 | begrenzt | sehr robust gegen RF |
| LTE / GSM / 5G | ✗ | Falsches Band |
| DECT-Schnurlostelefon | ✗ | 1.88 GHz, falsches Band |
| 433 MHz Garagentore | ✓ | SubGHz Carrier/Sweep |
| 433/868 MHz Funkschlüssel | ✓ | SubGHz Carrier/Sweep |
| TV/Beamer/Klima IR-Fernbed. | ✓ | IR Jammer |
| RFID-Zutritt (125 kHz) | ✓ | RFID-Jammer (Nahfeld) |
| NFC-Reader (13.56 MHz) | ✓ | NFC-Jammer (Nahfeld) |

## Reichweiten

- **Standard NRF24L01+**: 1-3m (0 dBm)
- **NRF24 PA+LNA** (E01C-ML01SP4): 10-30m (+20 dBm), mit 5dBi-Antenne bis 100m
- **CC1101 intern**: 50-80m
- **CC1101 extern**: 100-300m
- **IR intern**: 2-5m
- **IR Rabbit Blaster**: 10-20m
- **RFID/NFC**: 3-8 cm

## Rechtliches

⚠️ **Funkstörung ist in DE/CH/AT illegal ohne Erlaubnis (BNetzA / BAKOM / BMVIT).**

Diese App ist für:
- ✅ Eigene Geräte / Heim-Lab
- ✅ Security-Forschung mit Erlaubnis
- ✅ CTF-Wettkämpfe
- ✅ Pentest-Engagements (NDA + Scope)

Nicht für:
- ❌ Öffentliche Räume
- ❌ Fremde Geräte
- ❌ Drohnen-Abschuss
- ❌ Stalking / Belästigung

## Architektur

```
flipper-jammer/
├── application.fam          # FAP Manifest
├── jammer_app.h/c           # App State + Hardware-Erkennung
├── modules/                 # RF/Protokoll-Module
│   ├── subghz_jam.c         # CC1101 via subghz_devices_*
│   ├── nrf24_jam.c          # NRF24 Constant Carrier
│   ├── nrf24_spi.c          # Hardware-SPI Wrapper
│   ├── ble_jam.c            # BLE 37/38/39 Flood
│   ├── wifi_jam.c           # ESP32 UART + Async RX
│   ├── ir_jam.c             # IR async TX (38 kHz)
│   ├── rfid_jam.c           # 125 kHz Carrier
│   ├── nfc_jam.c            # 13.56 MHz Field
│   ├── usb_fuzz.c           # HID Flood/Malformed
│   ├── fuzzer.c             # RFID/NFC Fuzzer
│   ├── spoofer.c            # RFID/NFC Spoofer
│   └── jammer_storage.c     # Settings Save/Load
├── scenes/                  # Scene Manager
│   ├── scenes.c             # SceneManagerHandlers
│   ├── scene_list.h         # Scene Enum
│   ├── scene_main_menu.c    # Hauptmenü
│   ├── scene_nrf24_diag.c   # Diagnose-Modus
│   ├── scene_*.c            # Pro-Modul Config/Run
│   └── ...
└── resources/
    ├── info_texts.h         # Scrollbare Hilfe-Texte
    └── deauth_esp32/        # ESP32 Sketch (Arduino)
```

## Lizenz

MIT — siehe Quellcode-Header.

## Credits

- [Momentum Firmware](https://github.com/Next-Flip/Momentum-Firmware) — SDK + API 87.1
- [Next-Flip/Momentum-Apps/nrf24mousejacker](https://github.com/Next-Flip/Momentum-Apps/tree/main/nrf24mousejacker) — NRF24 Hardware-SPI Referenz
- [d1mov/FlipperzeroNRFJammer](https://github.com/d1mov/FlipperzeroNRFJammer) — Channel-Set Inspiration
- [ESP8266Deauther](https://github.com/SpacehuhnTech/esp8266_deauther) — Deauth-Protokoll

## Verwandte Projekte

Teil der **G4MEOVER Flipper Zero Toolchain**:

- [G4MEOVER-FW](https://github.com/G4MEOVER18/G4MEOVER-FW) — Custom Firmware
- [RollForge](https://github.com/G4MEOVER18/RollForge) — RollJam + RollLab + Jammer (unified)
- [RollJam](https://github.com/G4MEOVER18/RollJam) — Rolling-Code Jam+Capture+Replay
- [RollLab](https://github.com/G4MEOVER18/RollLab) — Signal-Analyzer Research
- [ProtoPirate](https://github.com/G4MEOVER18/ProtoPirate) — SubGHz Protocol Analyzer
- [lora-ukfe](https://github.com/G4MEOVER18/lora-ukfe) — LoRa USB Army Knife Edition

## Donations

Wenn dir das Projekt gefallen hat, freue ich mich über eine Spende:

[![PayPal](https://img.shields.io/badge/PayPal-Spenden-0070ba?style=flat-square&logo=paypal)](https://paypal.me/Freakbank1)
[![Bitcoin](https://img.shields.io/badge/BTC-39vZWmnUwDReQ15BwqQXzyqVQ6U8LardEf-f7931a?style=flat-square&logo=bitcoin)](bitcoin:39vZWmnUwDReQ15BwqQXzyqVQ6U8LardEf)

```
BTC: 39vZWmnUwDReQ15BwqQXzyqVQ6U8LardEf

**Kontakt:** [g4me.over.18@gmail.com](mailto:g4me.over.18@gmail.com)
```

---

*Teil der G4MEOVER Security Toolchain · [github.com/G4MEOVER18](https://github.com/G4MEOVER18)*

Built by **[G4MEOVER18](https://github.com/G4MEOVER18)** • 2026
