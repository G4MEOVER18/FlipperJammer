#pragma once
#include <stdint.h>

/* ------------------------------------------------------------------ */
/*  RFID Spoofer                                                        */
/* ------------------------------------------------------------------ */

typedef enum {
    SpooferEm4100   = 0,
    SpooferHidProx,
    SpooferIndala,
    SpooferCustom,
} RfidSpooferType;

/**
 * Start continuous RFID emulation.
 * @param type       Protocol type
 * @param facility   Facility / site code (26-bit HID) or full upper word (EM4100)
 * @param card_id    Card number / lower word
 */
void rfid_spoofer_start(RfidSpooferType type, uint32_t facility, uint32_t card_id);

/** Stop RFID emulation. */
void rfid_spoofer_stop(void);

/* ------------------------------------------------------------------ */
/*  NFC Spoofer                                                         */
/* ------------------------------------------------------------------ */

typedef enum {
    SpooferMifareClassic = 0,
    SpooferMifareUl,
    SpooferIso14443aRaw,
} NfcSpooferType;

/**
 * Start NFC tag emulation.
 * @param type    Tag type
 * @param uid     UID bytes
 * @param uid_len Number of UID bytes (4 or 7)
 */
void nfc_spoofer_start(NfcSpooferType type, const uint8_t* uid, uint8_t uid_len);

/** Stop NFC emulation. */
void nfc_spoofer_stop(void);
