#include "../jammer_app.h"
#include "scenes.h"
#include "../modules/spoofer.h"
#include <stdio.h>

static const char* rfid_type_labels[]  = {"EM4100", "HID Prox", "Indala", "Custom"};
static const char* facility_labels[]   = {"0", "10", "50", "100", "200", "255"};
static const uint32_t facility_vals[]  = {0, 10, 50, 100, 200, 255};
static const char* card_id_labels[]    = {"1337", "4242", "9999", "12345", "65535"};
static const uint32_t card_id_vals[]   = {1337, 4242, 9999, 12345, 65535};

typedef struct {
    uint8_t  type_index;
    uint8_t  facility_index;
    uint8_t  card_id_index;
    bool     running;
} RfidSpooferState;

static RfidSpooferState s_rfid = {0};

static void rfid_type_change(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, rfid_type_labels[index]);
    s_rfid.type_index = index;
}

static void rfid_facility_change(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, facility_labels[index]);
    s_rfid.facility_index = index;
}

static void rfid_card_id_change(VariableItem* item) {
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, card_id_labels[index]);
    s_rfid.card_id_index = index;
}

typedef enum {
    RfidSpfType = 0,
    RfidSpfFacility,
    RfidSpfCardId,
    RfidSpfStart,
    RfidSpfStop,
} RfidSpfIndex;

static void rfid_spoofer_enter(void* context, uint32_t index) {
    JammerApp* app = context;
    if(index == RfidSpfStart) {
        rfid_spoofer_start(
            (RfidSpooferType)s_rfid.type_index,
            facility_vals[s_rfid.facility_index],
            card_id_vals[s_rfid.card_id_index]);
        s_rfid.running = true;
        jammer_notify_start(app);
    } else if(index == RfidSpfStop) {
        rfid_spoofer_stop();
        s_rfid.running = false;
        jammer_notify_stop(app);
    }
}

void jammer_scene_RfidSpooferConfig_on_enter(void* context) {
    JammerApp*    app = context;
    VariableItem* item;

    variable_item_list_reset(app->var_list);
    variable_item_list_set_header(app->var_list, "RFID Spoofer");

    item = variable_item_list_add(app->var_list, "Typ", 4, rfid_type_change, app);
    variable_item_set_current_value_index(item, s_rfid.type_index);
    variable_item_set_current_value_text(item, rfid_type_labels[s_rfid.type_index]);

    item = variable_item_list_add(app->var_list, "Facility", 6, rfid_facility_change, app);
    variable_item_set_current_value_index(item, s_rfid.facility_index);
    variable_item_set_current_value_text(item, facility_labels[s_rfid.facility_index]);

    item = variable_item_list_add(app->var_list, "Card-ID", 5, rfid_card_id_change, app);
    variable_item_set_current_value_index(item, s_rfid.card_id_index);
    variable_item_set_current_value_text(item, card_id_labels[s_rfid.card_id_index]);

    variable_item_list_add(app->var_list, ">> START Emulate <<", 1, NULL, app);
    variable_item_list_add(app->var_list, ">> STOPP <<",         1, NULL, app);

    variable_item_list_set_enter_callback(app->var_list, rfid_spoofer_enter, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ViewVarList);
}

bool jammer_scene_RfidSpooferConfig_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void jammer_scene_RfidSpooferConfig_on_exit(void* context) {
    JammerApp* app = context;
    if(s_rfid.running) {
        rfid_spoofer_stop();
        s_rfid.running = false;
    }
    variable_item_list_reset(app->var_list);
}
