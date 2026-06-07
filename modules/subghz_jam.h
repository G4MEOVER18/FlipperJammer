#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    SubGhzJamCarrier = 0,
    SubGhzJamNoise,
    SubGhzJamSweep,
} SubGhzJamMode;

/**
 * Start SubGHz jamming.
 * @param freq_mhz  Center frequency in MHz (e.g. 433.92)
 * @param mode      Carrier / Noise / Sweep
 * @param external  true = use external path (REK5Lab)
 */
void subghz_jam_start(float freq_mhz, SubGhzJamMode mode, bool external);

/** Stop SubGHz jamming and release HAL. */
void subghz_jam_stop(void);

/** Returns true while jamming is active. */
bool subghz_jam_is_running(void);

/** Im Sweep-Modus: aktuell ausgegebene Frequenz in MHz. Sonst Basis-Frequenz. */
float subghz_jam_current_freq_mhz(void);
