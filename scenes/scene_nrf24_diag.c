#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/nrf24_spi.h"
#include <furi_hal_power.h>
#include <stdio.h>

/* NRF24 Diagnose-Scene — VariableItemList als Step-Menue.
   User klickt durch die Schritte und sieht ob LED reagiert. */

typedef enum {
    DiagStepOff = 0,
    DiagStepOtgOn,
    DiagStepSpiInit,
    DiagStepCeHigh,
    DiagStepContWave,
    DiagStepCount,
} DiagStep;

static const char* step_labels[DiagStepCount] = {
    "0: Alles AUS",
    "1: OTG 5V AN",
    "2: PWR_UP=1",
    "3: CE HIGH",
    "4: CONT_WAVE (Carrier)",
};

static DiagStep s_step = DiagStepOff;
static bool     s_otg_by_diag = false;

static void diag_apply_step(DiagStep step) {
    // Erst alles abbauen ab Schritt step+1 abwaerts
    nrf24_ce_set(false);
    if(step < DiagStepSpiInit) {
        nrf24_spi_deinit();
    }
    if(step < DiagStepOtgOn) {
        if(s_otg_by_diag && furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
            s_otg_by_diag = false;
        }
    }

    // Aufbauen
    if(step >= DiagStepOtgOn) {
        if(!furi_hal_power_is_otg_enabled()) {
            furi_hal_power_enable_otg();
            furi_delay_ms(20);
            s_otg_by_diag = true;
        }
    }
    if(step >= DiagStepSpiInit) {
        nrf24_spi_init();
    }
    if(step >= DiagStepCeHigh) {
        nrf24_ce_set(true);
    }
    if(step >= DiagStepContWave) {
        nrf24_write_reg(NRF24_REG_RF_SETUP,
            NRF24_RF_SETUP_CONT_WAVE | NRF24_RF_SETUP_PLL_LOCK |
            NRF24_RF_SETUP_2MBPS | NRF24_RF_SETUP_PWR_MAX);
        nrf24_write_reg(NRF24_REG_RF_CH, 40);
        furi_delay_us(140);
    }
    s_step = step;
}

static void diag_step_change(VariableItem* item) {
    JammerApp* app = variable_item_get_context(item);
    uint8_t idx = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, step_labels[idx]);
    diag_apply_step((DiagStep)idx);
    UNUSED(app);
}

void jammer_scene_Nrf24Diag_on_enter(void* context) {
    JammerApp* app = context;

    s_step = DiagStepOff;
    s_otg_by_diag = false;
    diag_apply_step(s_step);

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "NRF24 Diagnose");

    VariableItem* item = variable_item_list_add(
        app->var_list, "Schritt", DiagStepCount, diag_step_change, app);
    variable_item_set_current_value_index(item, (uint8_t)s_step);
    variable_item_set_current_value_text(item, step_labels[s_step]);

    variable_item_list_add(app->var_list, "<- aendern mit Rechts/Links", 1, NULL, app);
    variable_item_list_add(app->var_list, "LED beobachten!", 1, NULL, app);
    variable_item_list_add(app->var_list, "Back=Stop+Verlassen", 1, NULL, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_Nrf24Diag_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_Nrf24Diag_on_exit(void* context) {
    JammerApp* app = context;
    variable_item_list_reset(app->var_list);
    nrf24_ce_set(false);
    nrf24_spi_deinit();
    if(s_otg_by_diag && furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
        s_otg_by_diag = false;
    }
    s_step = DiagStepOff;
}
