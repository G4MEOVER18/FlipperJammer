#pragma once

/**
 * NFC Jammer — energises the 13.56 MHz field to prevent
 * ISO 14443-A/B readers from communicating with cards.
 */

/** Enable 13.56 MHz field. */
void nfc_jam_start(void);

/** Disable field and release NFC hardware. */
void nfc_jam_stop(void);
