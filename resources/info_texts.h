#pragma once

/* ------------------------------------------------------------------ */
/*  Info texts — scrollbare TextBox-Anzeige                            */
/*  TextBoxFontText: ~15 Zeilen sichtbar, vertikales Scrolling via Up/Down */
/* ------------------------------------------------------------------ */

#define INFO_MAIN \
    "Jammer Suite v1.2\n" \
    "\n" \
    "MODULE:\n" \
    "- SubGHz: CC1101\n" \
    "  (intern oder ext)\n" \
    "- NRF24: 2.4 GHz RF\n" \
    "  (Multiboard/HW-237)\n" \
    "- ESP32: Deauth via UART\n" \
    "  (Marauder Firmware)\n" \
    "- IR: intern oder ext\n" \
    "  (Rabbit Labs)\n" \
    "- RFID/NFC: eingebaut\n" \
    "- USB: eingebaut\n" \
    "\n" \
    "WICHTIG:\n" \
    "- NRF24 = echtes RF-Jamming\n" \
    "- ESP32 = nur Deauth (L2)\n" \
    "  funkt. nicht bei WPA3\n" \
    "\n" \
    "Hauptmenu zeigt erkannte\n" \
    "Module: [CC] [NRF]\n" \
    "\n" \
    "Nur fuer eigene Geraete!\n" \
    "Security-Testing/CTF\n" \
    "Keine illegale Stoerung!"

#define INFO_SUBGHZ \
    "SUBGHZ JAMMER\n" \
    "\n" \
    "MODUL:\n" \
    "Eingebaut ODER ext CC1101\n" \
    "(REK5Lab, FlipperMarket)\n" \
    "Auto-Erkennung: [CC] Tag\n" \
    "\n" \
    "FREQUENZEN:\n" \
    "- 315 MHz US-Sub\n" \
    "- 433.92 MHz EU/ISM\n" \
    "- 868.35 MHz EU-SRD\n" \
    "- 915 MHz US-ISM\n" \
    "\n" \
    "MODI:\n" \
    "- Carrier: Dauer-Traeger\n" \
    "- Noise: OOK Rauschen\n" \
    "- Sweep: Freq. Wobbeln\n" \
    "\n" \
    "ZIELE:\n" \
    "- Garagentore (433/868)\n" \
    "- Funkschluessel (KeeLoq)\n" \
    "- 433 MHz Sensoren\n" \
    "- Wetterstationen\n" \
    "- TPMS Reifendrucksens.\n" \
    "- ISM-Band-Geraete\n" \
    "\n" \
    "EXT CC1101 EMPFEHLUNG:\n" \
    "- Mehr Sendeleistung\n" \
    "- Externe Antenne\n" \
    "- Bessere Reichweite\n" \
    "- 2x-5x weiter als int"

#define INFO_NRF24 \
    "NRF24 2.4 GHz JAMMER\n" \
    "\n" \
    "MODUL:\n" \
    "NRF24L01+ (Standard 0dBm)\n" \
    "ODER PA+LNA (+20dBm!)\n" \
    "Auto-Erkennung: [NRF] Tag\n" \
    "\n" \
    "KANAL-SETS:\n" \
    "- ALL: 0-82 (volle 2.4GHz)\n" \
    "- Bluetooth: BT FH-Kanaele\n" \
    "- Zigbee: 802.15.4 (16ch)\n" \
    "- WiFi 1: Ch1 Spektrum\n" \
    "- WiFi 6: Ch6 Spektrum\n" \
    "- WiFi 11: Ch11 Spektrum\n" \
    "- Logitech: Unifying Ch\n" \
    "\n" \
    "ZIELE / WAS WIRD GESTOERT:\n" \
    "\n" \
    "DROHNEN-FERNSTEUERUNG:\n" \
    "- Spektrum DSM2/DSMX\n" \
    "- FlySky AFHDS/AFHDS2A\n" \
    "- FrSky D/X/XD\n" \
    "- Walkera DEVO\n" \
    "- Futaba S-FHSS\n" \
    "- DJI Mavic/Phantom\n" \
    "  (WiFi-basiert)\n" \
    "- DJI OcuSync (robust)\n" \
    "\n" \
    "FUNK-PERIPHERIE:\n" \
    "- Logitech Unifying Maus\n" \
    "- Logitech Unifying Tast\n" \
    "- Microsoft 2.4 GHz Maus\n" \
    "- Generische 2.4G Dongles\n" \
    "- Wireless Game Pads\n" \
    "\n" \
    "BLUETOOTH:\n" \
    "- BT Classic (mit AFH)\n" \
    "- BLE neue Verbindungen\n" \
    "- BT Audio (A2DP) -> ALL\n" \
    "\n" \
    "ZIGBEE/IoT:\n" \
    "- Philips Hue Bridge\n" \
    "- IKEA Tradfri\n" \
    "- Smart-Home Sensoren\n" \
    "\n" \
    "WIFI 2.4 GHz:\n" \
    "- ECHTES Jamming (RF-Layer)\n" \
    "- Funktioniert auch bei WPA3\n" \
    "- Kein Geraet immun"

#define INFO_NRF24_CANNOTJAM \
    "NRF24 KANN NICHT:\n" \
    "\n" \
    "5 GHz WiFi (5G-Band)\n" \
    "- 5170-5835 MHz\n" \
    "- NRF24 nur 2.4 GHz\n" \
    "\n" \
    "LTE / GSM / 5G\n" \
    "- 700 MHz - 3.8 GHz\n" \
    "- Spezial-Frequenzen\n" \
    "- Hohe Leistung notwendig\n" \
    "\n" \
    "DECT (Schnurlostelefon)\n" \
    "- 1880-1900 MHz\n" \
    "- Ausserhalb 2.4 GHz\n" \
    "\n" \
    "GPS / GNSS\n" \
    "- L1: 1575 MHz\n" \
    "- NRF24 falsches Band\n" \
    "\n" \
    "DJI OcuSync 3 / 4\n" \
    "- Frequenz-Adaptiv\n" \
    "- Sehr robust gegen RF\n" \
    "- Nutzt 5.8 GHz Backup\n" \
    "\n" \
    "REICHWEITEN-LIMITS:\n" \
    "\n" \
    "Standard NRF24L01+:\n" \
    "- 0 dBm = 1 mW Leistung\n" \
    "- Effektiv: 1-3m\n" \
    "- Indoor: 2-5m\n" \
    "\n" \
    "NRF24 PA+LNA Modul:\n" \
    "- +20 dBm = 100 mW\n" \
    "- 100x mehr Leistung\n" \
    "- Reichweite: 10-30m\n" \
    "- Mit Antenne: bis 100m\n" \
    "\n" \
    "EMPFEHLUNG:\n" \
    "Fuer ernsthafte Tests\n" \
    "PA+LNA Modul holen!"

#define INFO_BLE \
    "BLE JAMMER\n" \
    "\n" \
    "MODUL:\n" \
    "NRF24L01+ am Multiboard\n" \
    "Auto-Erkennung: [NRF] Tag\n" \
    "\n" \
    "PINBELEGUNG:\n" \
    "CE   = Pin 6 (PB2)\n" \
    "CSN  = Pin 4 (PA4)\n" \
    "SCK  = Pin 5 (PB3)\n" \
    "MOSI = Pin 2 (PA7)\n" \
    "MISO = Pin 3 (PA6)\n" \
    "VCC  = Pin 9 (3.3V)\n" \
    "GND  = Pin 11 oder 18\n" \
    "\n" \
    "FUNKTION:\n" \
    "Floodet die 3 BLE\n" \
    "Advertising-Kanaele:\n" \
    "- CH 37: 2402 MHz\n" \
    "- CH 38: 2426 MHz\n" \
    "- CH 39: 2480 MHz\n" \
    "\n" \
    "1 ms pro Kanal Rotation\n" \
    "2 Mbps Rate, max. Power\n" \
    "\n" \
    "ZIELE:\n" \
    "- BLE Beacons (iBeacon,\n" \
    "  Eddystone, AltBeacon)\n" \
    "- BLE Sensoren / IoT\n" \
    "- BLE Tags (AirTag etc.)\n" \
    "- BLE neue Verbindungen\n" \
    "- BLE Mesh Werbung\n" \
    "- Smart-Home BLE\n" \
    "\n" \
    "WIRKUNG:\n" \
    "Neue BLE-Verbindungen\n" \
    "werden verhindert. Tags/\n" \
    "Beacons werden nicht mehr\n" \
    "von Apps erkannt.\n" \
    "\n" \
    "Schon bestehende BLE-\n" \
    "Datenverbindungen laufen\n" \
    "auf Data-Kanaelen (0-36)\n" \
    "und sind weniger gestoert"

#define INFO_BLE_CANNOTJAM \
    "BLE JAMMER GRENZEN\n" \
    "\n" \
    "Dieser BLE-Modus floodet\n" \
    "NUR die 3 Advertising-\n" \
    "Kanaele (37/38/39).\n" \
    "\n" \
    "DAFUER NICHT GEEIGNET:\n" \
    "\n" \
    "BT CLASSIC AUDIO (A2DP)\n" \
    "- Nutzt FHSS 1600 hops/s\n" \
    "- 79 Kanaele 2402-2480\n" \
    "- Adaptive FH (AFH)\n" \
    "- Weicht 'schlechten'\n" \
    "  Kanaelen aus\n" \
    "\n" \
    "-> Stattdessen NRF24\n" \
    "   ALL-Modus nutzen!\n" \
    "   ALL-Modus floodet\n" \
    "   alle 2.4 GHz -> AFH\n" \
    "   findet keine sauberen\n" \
    "   Kanaele mehr.\n" \
    "\n" \
    "BT CLASSIC HID (Tastat./\n" \
    "Maus mit Verbindung)\n" \
    "- Bestehende Verbindung\n" \
    "  laeuft auf Data-Ch.\n" \
    "- Nur neue Pairings sind\n" \
    "  durch BLE-Modus betroff\n" \
    "\n" \
    "-> Auch hier: NRF24 ALL\n" \
    "\n" \
    "BLE DATA-PHASE\n" \
    "- Verbundene BLE-Geraete\n" \
    "  hoppen auf 37 Data-Ch.\n" \
    "- BLE-Modus stoert nur\n" \
    "  Advertising/Connect\n" \
    "\n" \
    "-> Auch hier: NRF24 ALL\n" \
    "\n" \
    "ZUSAMMENFASSUNG:\n" \
    "\n" \
    "BLE-Modus  = Adv-Channels\n" \
    "NRF24 ALL  = Komplettes\n" \
    "             2.4 GHz Band\n" \
    "             -> Stoert ALLES\n" \
    "             inkl. BT Audio"

#define INFO_WIFI \
    "WIFI DEAUTH (Layer 2)\n" \
    "\n" \
    "MODUL:\n" \
    "ESP32 mit Marauder FW\n" \
    "ODER eigenes Deauth-\n" \
    "Sketch (resources/\n" \
    "deauth_esp32/deauth.ino)\n" \
    "\n" \
    "VERBINDUNG:\n" \
    "UART LPUART1\n" \
    "PA2 (TX) <-> ESP32 RX\n" \
    "PA3 (RX) <-> ESP32 TX\n" \
    "115200 Baud 8N1\n" \
    "JSON-Befehle\n" \
    "\n" \
    "WICHTIG:\n" \
    "Dies ist KEIN echtes\n" \
    "Jammen! Der ESP32 sendet\n" \
    "802.11 Deauth-Frames\n" \
    "(Layer 2 Protokoll-\n" \
    "Angriff, kein RF-Rauschen)\n" \
    "\n" \
    "FUNKTIONSWEISE:\n" \
    "ESP32 spoofed die BSSID\n" \
    "des Ziel-APs und sendet\n" \
    "an alle Clients ein\n" \
    "Deauthentication-Frame.\n" \
    "Clients werden vom AP\n" \
    "getrennt und muessen\n" \
    "neu connecten.\n" \
    "\n" \
    "KANAL:\n" \
    "- ALL: Zyklisch alle\n" \
    "  WiFi Channels (1-13)\n" \
    "- 1-13: Gezielter Channel\n" \
    "\n" \
    "MODI:\n" \
    "- Broadcast: Alle APs\n" \
    "- Targeted: BSSID Ziel"

#define INFO_WIFI2 \
    "WIFI DEAUTH GRENZEN\n" \
    "\n" \
    "Seit 2013 gibt es 802.11w\n" \
    "(PMF - Protected Mgmt.\n" \
    "Frames). PMF schuetzt\n" \
    "Management-Frames per\n" \
    "Verschluesselung.\n" \
    "\n" \
    "IMMUN GEGEN DEAUTH:\n" \
    "\n" \
    "- WPA3 (PMF Pflicht)\n" \
    "- WPA2 mit PMF (optional)\n" \
    "- Alle modernen Smart-\n" \
    "  phones (iOS 14+, Android\n" \
    "  ab 11)\n" \
    "- Aktuelle Router (FRITZ!\n" \
    "  Box, ASUS, Cisco etc.)\n" \
    "- Enterprise WLANs\n" \
    "- Mesh-Netze (oft PMF)\n" \
    "\n" \
    "FUNKTIONIERT NUR NOCH:\n" \
    "\n" \
    "- Alte Router (>5J alt)\n" \
    "- WPA2 ohne PMF\n" \
    "- IoT-Geraete (oft alt)\n" \
    "- Sehr alte Smartphones\n" \
    "- Specifisch konfig.\n" \
    "  Lab-Setups\n" \
    "\n" \
    "PRAKTISCHE TIPPS:\n" \
    "\n" \
    "- IoT-Kamera/Smart-Plug:\n" \
    "  oft noch anfaellig\n" \
    "- Heim-Drucker:\n" \
    "  meist anfaellig\n" \
    "- Smart-TVs (alt):\n" \
    "  oft anfaellig\n" \
    "- Gaming-Konsolen:\n" \
    "  modern = immun"

#define INFO_WIFI3 \
    "ECHTES WIFI JAMMING\n" \
    "\n" \
    "WiFi-Deauth funktioniert\n" \
    "bei modernen Geraeten\n" \
    "nicht mehr. Fuer echtes\n" \
    "Stoeren stattdessen:\n" \
    "\n" \
    "==> NRF24 ALL-Modus <==\n" \
    "\n" \
    "Funktionsweise:\n" \
    "- NRF24 sendet auf 0-82\n" \
    "  Kanaelen (2.4-2.526 GHz)\n" \
    "- Floodet das gesamte\n" \
    "  2.4 GHz WiFi-Band\n" \
    "- Stoert auf PHY-Layer\n" \
    "- Kein WiFi-Geraet immun\n" \
    "\n" \
    "WARUM ES FUNKTIONIERT:\n" \
    "PMF schuetzt nur die\n" \
    "Management-Frames. Wenn\n" \
    "die Funkphysik gestoert\n" \
    "ist, kommen die Frames\n" \
    "gar nicht erst durch.\n" \
    "\n" \
    "LIMITS NRF24:\n" \
    "\n" \
    "5 GHz WiFi: NICHT moegl.\n" \
    "- 5G-Band ist out-of-scope\n" \
    "- Fuer 5G: Spezial-HW\n" \
    "\n" \
    "Reichweite:\n" \
    "- Standard: 1-5m\n" \
    "- PA+LNA: 10-30m\n" \
    "\n" \
    "VERGLEICH:\n" \
    "\n" \
    "ESP32 Deauth:\n" \
    "- Reichweite: 50-100m\n" \
    "- WPA3: 0% Wirkung\n" \
    "- Alte WPA2: 100% Wirkung\n" \
    "\n" \
    "NRF24 ALL:\n" \
    "- Reichweite: 1-30m\n" \
    "- WPA3: 100% Wirkung\n" \
    "- WPA2: 100% Wirkung\n" \
    "- BT/BLE/Zigbee: alle"

#define INFO_IR \
    "IR JAMMER\n" \
    "\n" \
    "MODUL:\n" \
    "- Intern: eingebaute LED\n" \
    "- Extern: Rabbit Labs\n" \
    "  IR Blaster auf PA6\n" \
    "\n" \
    "MODI:\n" \
    "- Noise Burst: Random\n" \
    "  IR-Pulse Floods\n" \
    "- TV Codes: Power-Cmds\n" \
    "  von LG, Samsung, Sony,\n" \
    "  Philips, Panasonic etc.\n" \
    "- Random RAW: zufaellige\n" \
    "  Mark/Space Sequenzen\n" \
    "- NEC Flood: NEC mit\n" \
    "  random 32-bit codes\n" \
    "- Samsung Flood\n" \
    "- Sony SIRC Flood\n" \
    "\n" \
    "ZIELE:\n" \
    "- TV/Beamer\n" \
    "- Verstaerker/Receiver\n" \
    "- Klimaanlagen\n" \
    "- Set-Top-Boxen\n" \
    "- Smart-Home IR-Steuer.\n" \
    "\n" \
    "REICHWEITE:\n" \
    "- Intern: 2-5m\n" \
    "- Ext Blaster: 10-20m\n" \
    "\n" \
    "TV Codes-Modus ist am\n" \
    "effektivsten gegen TVs:\n" \
    "Schaltet aus dem Stand-\n" \
    "by an/aus, wechselt\n" \
    "Inputs, etc."

#define INFO_RFID \
    "RFID JAMMER / SPOOFER\n" \
    "\n" \
    "MODUL: Eingebaut\n" \
    "125 kHz LF-RFID\n" \
    "\n" \
    "MODI:\n" \
    "- Jammer: Dauertraeger\n" \
    "  ueberlagert echte Tags\n" \
    "- Spoofer: Emuliert\n" \
    "  EM4100/HID/Indala Tag\n" \
    "  mit konfig. ID\n" \
    "- Fuzzer: Zufalls-IDs\n" \
    "  als Brute-Force\n" \
    "\n" \
    "ZIELE:\n" \
    "- Zutrittskontrollen\n" \
    "  (Buero, Garage etc.)\n" \
    "- Zeit-Erfassung\n" \
    "- Hotel-Karten (alt)\n" \
    "- Implant-Tags\n" \
    "- Tier-Chips (kompat.)\n" \
    "\n" \
    "PROTOKOLLE:\n" \
    "- EM4100/EM4102\n" \
    "- HID Prox (26/35/37 bit)\n" \
    "- Indala\n" \
    "- AWID\n" \
    "- Custom RAW\n" \
    "\n" \
    "REICHWEITE: ~3-8 cm\n" \
    "Sehr kurz, da LF\n" \
    "\n" \
    "NICHT KOMPATIBEL:\n" \
    "- 13.56 MHz NFC -> NFC-Modul\n" \
    "- UHF RFID (860-960 MHz)"

#define INFO_NFC \
    "NFC JAMMER / SPOOFER\n" \
    "\n" \
    "MODUL: Eingebaut\n" \
    "13.56 MHz HF-NFC\n" \
    "ISO14443A/B\n" \
    "\n" \
    "MODI:\n" \
    "- Jammer: Feld aktiv\n" \
    "  stoert Reader\n" \
    "- Spoofer: Emuliert\n" \
    "  Tag (MIFARE etc.)\n" \
    "- Fuzzer: Zufalls-UIDs\n" \
    "\n" \
    "EMULIERBARE TYPEN:\n" \
    "- MIFARE Classic 1K/4K\n" \
    "- MIFARE Ultralight\n" \
    "- MIFARE DESFire (teilw.)\n" \
    "- ISO14443A Raw\n" \
    "- NTAG213/215/216\n" \
    "\n" \
    "ZIELE:\n" \
    "- Bezahlsysteme (Test!)\n" \
    "- Zutritt mit NFC-Karte\n" \
    "- Smart-Locks\n" \
    "- Werbe-NFC-Tags\n" \
    "- Hotel-Schluessel\n" \
    "- OV-Karten (lokale)\n" \
    "\n" \
    "REICHWEITE: ~3-5 cm\n" \
    "Nahfeld-Kommunikation\n" \
    "\n" \
    "GRENZEN:\n" \
    "- Cryptografie z.B.\n" \
    "  MIFARE Plus, DESFire\n" \
    "  EV2: nicht emulierbar\n" \
    "- EMV (Bank-Karten):\n" \
    "  PIN/Cryptogramm noetig"

#define INFO_USB_FUZZ \
    "USB FUZZER\n" \
    "\n" \
    "MODUL: Eingebaut USB\n" \
    "Kein externes Modul!\n" \
    "Flipper muss am Ziel-\n" \
    "Host (PC/Mac) per USB\n" \
    "angeschlossen sein.\n" \
    "\n" \
    "MODI:\n" \
    "- HID Flood: Tastatur-\n" \
    "  Inputs fluten\n" \
    "- Malformed: ungueltige\n" \
    "  HID-Reports (Driver-\n" \
    "  Test)\n" \
    "- Descriptor: HID re-\n" \
    "  enumeration Floods\n" \
    "- Reconnect: USB an/aus\n" \
    "  Cycling\n" \
    "\n" \
    "ZIELE:\n" \
    "- USB HID Driver-Tests\n" \
    "- BadUSB Detection-Test\n" \
    "- HID Sicherheit pruefen\n" \
    "- Lockscreen-Bypass-Test\n" \
    "\n" \
    "WICHTIG:\n" \
    "Nach Test wird standard\n" \
    "CDC-Config wiederherge-\n" \
    "stellt. Kein perma-\n" \
    "nenter Schaden.\n" \
    "\n" \
    "NUR AUF EIGENEN GERAETEN\n" \
    "ODER MIT EXPLIZITER\n" \
    "ERLAUBNIS!"

#define INFO_DRONE \
    "DROHNEN 2.4 GHz\n" \
    "\n" \
    "MODUL: NRF24L01+\n" \
    "PA+LNA EMPFOHLEN!\n" \
    "\n" \
    "Standard 0dBm = 1-3m\n" \
    "PA+LNA +20dBm = 10-30m\n" \
    "\n" \
    "GESTOERTE PROTOKOLLE:\n" \
    "\n" \
    "Spektrum DSM2/DSMX\n" \
    "- 2.402-2.479 GHz\n" \
    "- 23 oder 80 Kanaele\n" \
    "- NRF24 ALL-Modus\n" \
    "\n" \
    "FlySky AFHDS/AFHDS2A\n" \
    "- 2.4-2.483 GHz\n" \
    "- 16 Kanaele\n" \
    "- NRF24 ALL-Modus\n" \
    "\n" \
    "FrSky D/X/XD\n" \
    "- 2.404-2.473 GHz\n" \
    "- 47 FHSS Kanaele\n" \
    "- NRF24 ALL-Modus\n" \
    "\n" \
    "Walkera DEVO\n" \
    "- 2.4-2.4835 GHz\n" \
    "- 100 Kanaele\n" \
    "- NRF24 ALL-Modus\n" \
    "\n" \
    "Futaba S-FHSS\n" \
    "- 2.4 GHz Frequency\n" \
    "  Hopping Spread Spect.\n" \
    "- NRF24 ALL-Modus\n" \
    "\n" \
    "DJI WIFI-DROHNEN:\n" \
    "(Mavic Mini, etc)\n" \
    "- Nutzt WiFi 2.4 GHz\n" \
    "- NRF24 ALL stoert\n" \
    "\n" \
    "DJI OcuSync 3/4:\n" \
    "- Adaptiv, wechselt\n" \
    "  zu 5.8 GHz Backup\n" \
    "- Sehr robust\n" \
    "- Hier hilft NRF24 nur\n" \
    "  bedingt\n" \
    "\n" \
    "EMPFEHLUNG:\n" \
    "- Modus: ALL (Volle 2.4)\n" \
    "- PA+LNA Modul\n" \
    "- Externe Antenne 5dBi+\n" \
    "\n" \
    "RECHTLICHES:\n" \
    "Drohnen-Stoeren ist in DE\n" \
    "und CH ohne Erlaubnis\n" \
    "ILLEGAL. Nur fuer eigene\n" \
    "Drohnen in geschlossenen\n" \
    "Lab-Umgebungen!"

#define INFO_PERIPHERALS \
    "MAUS / TASTATUR 2.4 GHz\n" \
    "\n" \
    "MODUL: NRF24L01+\n" \
    "\n" \
    "PROTOKOLLE:\n" \
    "\n" \
    "Logitech Unifying:\n" \
    "- 24 Hop-Kanaele\n" \
    "- 2.405-2.475 GHz\n" \
    "- Verschluesselt (AES)\n" \
    "- Modus: Logitech\n" \
    "\n" \
    "Microsoft 2.4 GHz:\n" \
    "- 79 Kanaele FHSS\n" \
    "- Teils unverschluess.\n" \
    "- Modus: ALL\n" \
    "\n" \
    "Generische Dongles:\n" \
    "- Meist NRF24-basiert\n" \
    "- Verschiedene Kanaele\n" \
    "- Modus: ALL\n" \
    "\n" \
    "Razer/Logitech G-Reihe:\n" \
    "- 2.4 GHz Lightspeed\n" \
    "- Eigene Protokolle\n" \
    "- Modus: ALL\n" \
    "\n" \
    "REICHWEITE:\n" \
    "Standard NRF24: 1-5m\n" \
    "PA+LNA: 10-20m\n" \
    "\n" \
    "WIRKUNG:\n" \
    "Maus/Tastatur reagiert\n" \
    "verzoegert oder gar nicht.\n" \
    "User merkt Stoerung.\n" \
    "\n" \
    "NICHT BETROFFEN:\n" \
    "- BT/BLE Maus (->BLE Modus)\n" \
    "- USB-C kabelgebund.\n" \
    "- 27 MHz alte Geraete"
