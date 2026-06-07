#pragma once
#include <stdint.h>

/**
 * RFID Fuzzer — emits random EM4100 IDs at ~50 ms intervals.
 * NFC Fuzzer  — emits random ISO 14443-A UIDs at ~100 ms intervals.
 *
 * Both share the same iteration counter.
 */

void rfid_fuzzer_start(void);
void rfid_fuzzer_stop(void);

void nfc_fuzzer_start(void);
void nfc_fuzzer_stop(void);

/** Returns total number of IDs/UIDs emitted since last start. */
uint32_t fuzzer_iteration_count(void);
