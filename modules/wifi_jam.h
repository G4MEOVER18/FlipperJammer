#pragma once
#include <stdint.h>
#include <stdbool.h>

/**
 * WiFi Jammer — communicates with an ESP32 coprocessor via UART.
 * The ESP32 runs the deauth.ino sketch and sends 802.11 deauth frames.
 *
 * UART: FuriHalUartIdLPUART1 on GPIO PA6 (TX) / PA7 (RX)
 * Baud: 115200
 * Protocol: single JSON line per command
 */

/** Initialise UART and wait for ESP32 ready. */
void wifi_jam_init(void);

/**
 * Start WiFi jamming.
 * @param channel    WiFi channel 1-13, or 0 for ALL
 * @param broadcast  true = deauth all APs, false = target specific BSSID
 * @param bssid      "XX:XX:XX:XX:XX:XX" — used when broadcast == false
 */
void wifi_jam_start(uint8_t channel, bool broadcast, const char* bssid);

/** Stop jamming (sends stop command to ESP32). */
void wifi_jam_stop(void);

/** Letztes vom ESP32 empfangenes Status-Byte (Anzahl seit Start) */
uint32_t wifi_jam_rx_bytes(void);

/** Wahr wenn ESP32 in den letzten 2 Sekunden geantwortet hat */
bool wifi_jam_esp_online(void);
