#pragma once

/**
 * RFID Jammer — emits a raw 125 kHz carrier to prevent readers
 * from communicating with legitimate RFID tags.
 */

/** Enable 125 kHz carrier via built-in RFID hardware. */
void rfid_jam_start_carrier(void);

/** Stop carrier and release RFID hardware. */
void rfid_jam_stop(void);
