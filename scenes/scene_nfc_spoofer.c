#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/spoofer.h"
#include <stdio.h>

static const char* nfc_type_labels[] = {
    "MIFARE Classic",
    "MIFARE UL",
    "ISO14443A Raw",
};

/* Preset UIDs (4-byte) */
static const uint8_t preset_uids[][4] = {
    {0xDE, 0xAD, 0xBE, 0xEF},
    {0xCA, 0xFE, 0xBA, 0xBE},
    {0x13, 0x37, 0x00, 0x01},
    {0xFF, 0xFF, 0xFF, 0xFF},
    {0x00, 0x00, 0x00, 0x01},
};
static const char* preset_uid_labels[] = {
    "DEADBEEF",
    "CAFEBABE",
    "13370001",
    "FFFFFFFF",
    "00000001",
};

typedef struct {
    uint8_t type_index;
    uint8_t uid_index;
    bool    running;
} NfcSpooferState;

static NfcSpooferState s_nfc = {0};

static void nfc_type_change(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, nfc_type_labels[index]);
    s_nfc.type_index = index;
}

static void nfc_uid_change(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, preset_uid_labels[index]);
    s_nfc.uid_index = index;
}

typedef enum {
    NfcSpfType = 0,
    NfcSpfUid,
    NfcSpfStart,
    NfcSpfStop,
} NfcSpfIndex;

static void nfc_spoofer_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == NfcSpfStart) {
        nfc_spoofer_start(
            (NfcSpooferType)s_nfc.type_index,
            preset_uids[s_nfc.uid_index],
            4);
        s_nfc.running = true;
        jammer_notify_start(app);
    } else if(index == NfcSpfStop) {
        nfc_spoofer_stop();
        s_nfc.running = false;
        jammer_notify_stop(app);
    }
}

void jammer_scene_NfcSpooferConfig_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "NFC Spoofer");

    item = variable_item_list_add(app->var_list, "Typ", 3, nfc_type_change, app);
    variable_item_set_current_value_index(item, s_nfc.type_index);
    variable_item_set_current_value_text(item, nfc_type_labels[s_nfc.type_index]);

    item = variable_item_list_add(app->var_list, "UID Preset", 5, nfc_uid_change, app);
    variable_item_set_current_value_index(item, s_nfc.uid_index);
    variable_item_set_current_value_text(item, preset_uid_labels[s_nfc.uid_index]);

    variable_item_list_add(app->var_list, ">> START Emulate <<", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> STOPP <<",         1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, nfc_spoofer_enter, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_NfcSpooferConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_NfcSpooferConfig_on_exit(void* context) {
    JammerApp* app = context;
    if(s_nfc.running) {
        nfc_spoofer_stop();
        s_nfc.running = false;
    }
    variable_item_list_reset(app->var_list);
}
