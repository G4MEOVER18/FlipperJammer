#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    IrJamNoiseBurst = 0,
    IrJamTvCodes,
    IrJamRandomRaw,
    IrJamNecFlood,
    IrJamSamsung,
    IrJamSony,
} IrJamMode;

/**
 * Start IR jamming.
 * @param mode      Jamming mode
 * @param external  true = use external IR Blaster on GPIO PA6
 */
void ir_jam_start(IrJamMode mode, bool external);

/** Stop IR jamming. */
void ir_jam_stop(void);
