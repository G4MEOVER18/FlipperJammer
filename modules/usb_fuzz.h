#pragma once
#include <stdint.h>

// UsbFuzzMode ist in jammer_app.h definiert — kein Redeclaration-Fehler
// Wenn jammer_app.h nicht bereits inkludiert, Forward-Typedef nutzen
#ifndef JAMMER_APP_H_INCLUDED
typedef enum {
    UsbFuzzHidFlood    = 0,
    UsbFuzzMalformed,
    UsbFuzzDescriptor,
    UsbFuzzReconnect,
} UsbFuzzMode;
#endif

/**
 * Start USB fuzzing.
 * @param mode  Attack mode
 */
void usb_fuzz_start(UsbFuzzMode mode);

/** Stop USB fuzzing and restore default USB config. */
void usb_fuzz_stop(void);
