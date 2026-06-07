#pragma once

/* ------------------------------------------------------------------ */
/*  Info texts — displayed in the Info scene for each module            */
/* ------------------------------------------------------------------ */

#define INFO_MAIN \
    "Jammer Suite v1.0\n" \
    "Momentum Firmware FAP\n" \
    "---------------------\n" \
    "Alle Module nutzen die\n" \
    "eingebaute Hardware.\n" \
    "NRF24+BLE: REK5Lab\n" \
    "WiFi: ESP32 via UART\n" \
    "IR Blaster: Rabbit Labs\n" \
    "Nur fuer eigene Netze!"

#define INFO_SUBGHZ \
    "SubGHz Jammer\n" \
    "Stoert 433/868/315/915\n" \
    "MHz Funksignale.\n" \
    "Carrier: Dauersignal\n" \
    "Noise: Rauschburst\n" \
    "Sweep: Freq. Sweep\n" \
    "REK5Lab: ext. Antenne\n" \
    "Nur auf eigenen Geraeten!"

#define INFO_NRF24 \
    "NRF24 2.4GHz Jammer\n" \
    "Stoert 2.4 GHz Bänder.\n" \
    "Modus waehlen:\n" \
    "ALL: komplettes 2.4GHz\n" \
    "BT: Bluetooth Classic\n" \
    "Zigbee: 802.15.4\n" \
    "WiFi 1/6/11: WLAN Ch.\n" \
    "Logitech: Unifying"

#define INFO_BLE \
    "BLE Jammer\n" \
    "Stoert Bluetooth Low\n" \
    "Energy Werbung.\n" \
    "Kanaele: 37/38/39\n" \
    "(2402/2426/2480 MHz)\n" \
    "NRF24L01+ via REK5Lab\n" \
    "2 Mbps, max. Leistung\n" \
    "1 ms/Kanal Rotation"

#define INFO_WIFI \
    "WiFi Deauth Jammer\n" \
    "Sendet 802.11 Deauth\n" \
    "Frames via ESP32.\n" \
    "Verbindung: UART\n" \
    "115200 Baud (PA6/PA7)\n" \
    "Kanal 1-13 oder ALL\n" \
    "Broadcast oder Ziel-AP\n" \
    "ESP32 Sketch noetig!"

#define INFO_IR \
    "IR Jammer\n" \
    "Stoert IR-Fernbed.\n" \
    "Noise: Rauschpulse\n" \
    "TV Codes: Power-Cmds\n" \
    "NEC/Samsung/Sony Flood\n" \
    "Random RAW Sequenzen\n" \
    "Intern: eingebaute LED\n" \
    "Extern: Rabbit Blaster"

#define INFO_RFID \
    "RFID Jammer / Spoofer\n" \
    "125 kHz (EM4100/HID)\n" \
    "Jammer: Carrier aktiv\n" \
    "Spoofer: Tag emulieren\n" \
    "Fuzzer: Zufalls-IDs\n" \
    "Facility + Card-ID\n" \
    "Typen: EM4100, HID,\n" \
    "Indala, Custom"

#define INFO_NFC \
    "NFC Jammer / Spoofer\n" \
    "13.56 MHz ISO14443\n" \
    "Jammer: Feld aktiv\n" \
    "Spoofer: UID emulieren\n" \
    "Fuzzer: Zufalls-UIDs\n" \
    "Typen: MIFARE Cl./UL\n" \
    "ISO14443A Raw\n" \
    "Preset UIDs waehlbar"

#define INFO_USB_FUZZ \
    "USB Fuzzer\n" \
    "Testet USB HID Host.\n" \
    "HID Flood: Tastatur\n" \
    "Malformed: inv. Reports\n" \
    "Descriptor: Neuenum.\n" \
    "Reconnect: USB Schleife\n" \
    "Standard CDC danach\n" \
    "wiederhergestellt"
