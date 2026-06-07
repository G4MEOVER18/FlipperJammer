#pragma once

/* ------------------------------------------------------------------ */
/*  Info texts — pro Modul: Modul-Anforderung + was jammt / was nicht  */
/* ------------------------------------------------------------------ */

#define INFO_MAIN \
    "Jammer Suite v1.1\n" \
    "MODUL-UEBERSICHT:\n" \
    "SubGHz: CC1101 ext\n" \
    " -> 315/433/868/915 MHz\n" \
    "NRF24: 2.4GHz RF-Jam\n" \
    " -> BLE/BT/WiFi/Drone\n" \
    "ESP32: Deauth (Layer2)\n" \
    " -> kein echtes Jammen!"

#define INFO_SUBGHZ \
    "MODUL: CC1101\n" \
    "Intern ODER REK5Lab ext\n" \
    "---\n" \
    "JAMMT: Garagentore,\n" \
    "Funkschluessel, 433MHz\n" \
    "Sensoren, ISM-Band\n" \
    "---\n" \
    "EMPFEHLUNG: ext CC1101\n" \
    "= mehr Reichweite+Leistg"

#define INFO_NRF24 \
    "MODUL: NRF24L01+\n" \
    "PA+LNA Modul empfohlen!\n" \
    "---\n" \
    "JAMMT: Drohnen (DSM2/\n" \
    "DSMX/FlySky/FrSky),\n" \
    "Maus+Tastatur (Logitech\n" \
    "Unifying, 2.4GHz Dongle)\n" \
    "Zigbee, Bluetooth Classic\n" \
    "Standard NRF: ~1-3m"

#define INFO_NRF24_CANNOTJAM \
    "NRF24 KANN NICHT:\n" \
    "WiFi 5 GHz (5G-Band)\n" \
    "LTE / GSM / DECT\n" \
    "DJI OcuSync 3 (robust)\n" \
    "---\n" \
    "PA+LNA Modul:\n" \
    "+20dBm statt 0dBm\n" \
    "= 100x mehr Leistung\n" \
    "Reichweite ~10-30m"

#define INFO_BLE \
    "MODUL: NRF24L01+\n" \
    "Pins: CE=PA7 CSN=PA4\n" \
    "SCK=PB3 MOSI=PB2\n" \
    "MISO=SWCLK (PB4)\n" \
    "---\n" \
    "JAMMT: BLE Advertising\n" \
    "Kanaele 37/38/39\n" \
    "BT Low Energy Geraete,\n" \
    "Beacons, BLE Sensoren"

#define INFO_BLE_CANNOTJAM \
    "BLE KANN NICHT:\n" \
    "BT Classic Audio (A2DP)\n" \
    "BT HID mit Verbindung\n" \
    "(nur neue Verbindungen!)\n" \
    "---\n" \
    "WIRKUNG:\n" \
    "Neue BLE-Verbindungen\n" \
    "werden verhindert.\n" \
    "Bestehende bleiben."

#define INFO_WIFI \
    "MODUL: ESP32 Marauder\n" \
    "UART: PA2(TX) PA3(RX)\n" \
    "Baud: 115200\n" \
    "---\n" \
    "KEIN ECHTES JAMMEN!\n" \
    "Sendet Deauth-Frames\n" \
    "(Layer 2, Protokollebene)\n" \
    "---\n" \
    "FUNKTIONIERT NUR gegen:"

#define INFO_WIFI2 \
    "WiFi Deauth wirkt NUR:\n" \
    "Alte Router/Geraete\n" \
    "ohne 802.11w (PMF)\n" \
    "---\n" \
    "IMMUN (kein Effekt):\n" \
    "WPA3 (Pflicht: PMF)\n" \
    "802.11w aktiviert\n" \
    "Alle mod. Smartphones\n" \
    "Fuer echte WiFi-Stoerung"

#define INFO_WIFI3 \
    "Echtes WiFi-Jammen:\n" \
    "-> NRF24 ALL-Modus!\n" \
    "-> Stoert 2.4GHz RF\n" \
    "   physikalisch\n" \
    "-> Kein Geraet immun\n" \
    "-> PA+LNA fuer Reichweite\n" \
    "---\n" \
    "5GHz WiFi: NICHT moegl.\n" \
    "mit aktuellem Hardware"

#define INFO_IR \
    "MODUL: Intern ODER\n" \
    "Rabbit Labs IR Blaster\n" \
    "Ext: GPIO PA6\n" \
    "---\n" \
    "JAMMT: IR-Fernbedienungen\n" \
    "TV, Beamer, Verstaerker\n" \
    "---\n" \
    "Intern: ~2-5m\n" \
    "Ext Blaster: ~10-20m"

#define INFO_RFID \
    "MODUL: Eingebaut\n" \
    "125 kHz LF-RFID\n" \
    "---\n" \
    "JAMMT: LF-Kartenleser\n" \
    "EM4100, HID, Indala\n" \
    "Zutrittskontrollsysteme\n" \
    "---\n" \
    "Spoofer: Tag emulieren\n" \
    "Fuzzer: Zufalls-IDs"

#define INFO_NFC \
    "MODUL: Eingebaut\n" \
    "13.56 MHz HF-NFC\n" \
    "---\n" \
    "JAMMT: NFC-Kartenleser\n" \
    "MIFARE Classic/Ultralight\n" \
    "ISO14443A Geraete\n" \
    "---\n" \
    "Spoofer: UID emulieren\n" \
    "Fuzzer: Zufalls-UIDs"

#define INFO_USB_FUZZ \
    "MODUL: Eingebaut USB\n" \
    "Kein externes Modul!\n" \
    "---\n" \
    "TESTET: USB HID Hosts\n" \
    "HID Flood: Tastatur-\n" \
    "flood auf verbundenem PC\n" \
    "Malformed: inv. Reports\n" \
    "Reconnect: USB-Schleife\n" \
    "Nur auf eigenem Geraet!"

#define INFO_DRONE \
    "DROHNEN 2.4GHz:\n" \
    "MODUL: NRF24L01+\n" \
    "PA+LNA NOETIG!\n" \
    "---\n" \
    "DSM2/DSMX (Spektrum)\n" \
    "FlySky AFHDS/AFHDS2A\n" \
    "FrSky D/X/XD\n" \
    "DEVO/Walkera\n" \
    "-> NRF24 ALL-Modus"

#define INFO_PERIPHERALS \
    "MAUS/TASTATUR 2.4GHz:\n" \
    "MODUL: NRF24L01+\n" \
    "---\n" \
    "Logitech Unifying:\n" \
    "-> NRF24 Logitech-Modus\n" \
    "Microsoft/generisch:\n" \
    "-> NRF24 ALL-Modus\n" \
    "---\n" \
    "Reichweite: 1-5m (std)"
