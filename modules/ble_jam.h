#pragma once

/**
 * BLE Jammer — floods BLE advertising channels 37/38/39
 * using the NRF24L01+ at 2 Mbps.
 *
 * BLE advertising channels in NRF24 terms (2400 + offset MHz):
 *   Ch 37 → 2402 MHz → NRF24 channel  2
 *   Ch 38 → 2426 MHz → NRF24 channel 26
 *   Ch 39 → 2480 MHz → NRF24 channel 80
 */

void ble_jam_start(void);
void ble_jam_stop(void);
