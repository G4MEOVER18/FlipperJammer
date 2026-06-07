#include "nrf24_spi.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>
#include <furi_hal_spi.h>
#include <string.h>

/* Hardware-SPI via furi_hal_spi_bus_handle_external (PA4=CS, PB3=SCK, PA7=MOSI, PA6=MISO).
   Zusaetzlich:
   - PC3 (NFC-CS) wird HIGH gezogen — verhindert dass NFC den SPI-Bus klaut
   - CE = PB2 (extra GPIO fuer NRF24)
   Diese Konfiguration ist identisch mit Momentum nrf24mousejacker und kompatibel
   mit REK5Lab v2 und HW-237 Multiboards. */

#define NRF24_HANDLE       (&furi_hal_spi_bus_handle_external)
#define NRF24_NFC_CS_PIN   (&gpio_ext_pc3)
#define NRF24_TIMEOUT      500
#define NRF24_REGISTER_MASK 0x1F

static bool s_spi_initialized = false;

static inline void ce_low(void)  { furi_hal_gpio_write(NRF24_CE_PIN, false); }
static inline void ce_high(void) { furi_hal_gpio_write(NRF24_CE_PIN, true);  }

static void nrf24_spi_trx(uint8_t* tx, uint8_t* rx, uint8_t size) {
    furi_hal_gpio_write(NRF24_HANDLE->cs, false);
    furi_hal_spi_bus_trx(NRF24_HANDLE, tx, rx, size, NRF24_TIMEOUT);
    furi_hal_gpio_write(NRF24_HANDLE->cs, true);
}

void nrf24_spi_init(void) {
    if(s_spi_initialized) return;
    // NFC-CS (PC3) HIGH halten — verhindert NFC-SPI-Konflikt
    furi_hal_gpio_init_simple(NRF24_NFC_CS_PIN, GpioModeOutputPushPull);
    furi_hal_gpio_write(NRF24_NFC_CS_PIN, true);

    // Hardware-SPI Bus reservieren
    furi_hal_spi_bus_handle_init(NRF24_HANDLE);
    furi_hal_spi_acquire(NRF24_HANDLE);

    // CE als Output
    furi_hal_gpio_init(NRF24_CE_PIN, GpioModeOutputPushPull, GpioPullUp, GpioSpeedVeryHigh);
    ce_low();
    furi_delay_ms(5);

    // CONFIG: PWR_UP | PRIM_TX (0) | EN_CRC | CRCO -> LED leuchtet jetzt
    nrf24_write_reg(NRF24_REG_CONFIG,
        NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_TX | (1 << 3) | (1 << 2));
    furi_delay_ms(2); // Tpd2stby

    // Disable Auto-ACK
    nrf24_write_reg(NRF24_REG_EN_AA, 0x00);

    // RF_SETUP: 2Mbps, max power (wird in jam_start ueberschrieben fuer CONT_WAVE)
    nrf24_write_reg(NRF24_REG_RF_SETUP, NRF24_RF_SETUP_2MBPS | NRF24_RF_SETUP_PWR_MAX);

    // No retransmits
    nrf24_write_reg(NRF24_REG_SETUP_RETR, 0x00);

    // Broadcast TX address
    const uint8_t bcast[5] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    nrf24_write_buf(NRF24_REG_TX_ADDR, bcast, 5);

    s_spi_initialized = true;
}

void nrf24_spi_deinit(void) {
    if(!s_spi_initialized) return;
    ce_low();
    // Power down
    nrf24_write_reg(NRF24_REG_CONFIG, 0x08);

    furi_hal_spi_release(NRF24_HANDLE);
    furi_hal_spi_bus_handle_deinit(NRF24_HANDLE);

    furi_hal_gpio_init(NRF24_CE_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init_simple(NRF24_NFC_CS_PIN, GpioModeAnalog);
    s_spi_initialized = false;
}

uint8_t nrf24_write_reg(uint8_t reg, uint8_t val) {
    uint8_t tx[2] = {0x20 | (reg & NRF24_REGISTER_MASK), val};
    uint8_t rx[2] = {0};
    nrf24_spi_trx(tx, rx, 2);
    return rx[0];
}

uint8_t nrf24_read_reg(uint8_t reg) {
    uint8_t tx[2] = {reg & NRF24_REGISTER_MASK, 0xFF};
    uint8_t rx[2] = {0};
    nrf24_spi_trx(tx, rx, 2);
    return rx[1];
}

void nrf24_write_buf(uint8_t reg, const uint8_t* buf, uint8_t len) {
    uint8_t tx[1 + 32];
    uint8_t rx[1 + 32];
    if(len > 32) len = 32;
    tx[0] = 0x20 | (reg & NRF24_REGISTER_MASK);
    memcpy(&tx[1], buf, len);
    nrf24_spi_trx(tx, rx, len + 1);
}

uint8_t nrf24_cmd(uint8_t cmd) {
    uint8_t tx = cmd;
    uint8_t rx = 0;
    nrf24_spi_trx(&tx, &rx, 1);
    return rx;
}

void nrf24_write_tx_payload(const uint8_t* data, uint8_t len) {
    uint8_t tx[1 + 32];
    uint8_t rx[1 + 32];
    if(len > 32) len = 32;
    tx[0] = NRF24_CMD_W_TX_PAYLOAD;
    memcpy(&tx[1], data, len);
    nrf24_spi_trx(tx, rx, len + 1);
}

void nrf24_pulse_ce(void) {
    ce_high();
    furi_delay_us(15);
    ce_low();
}

void nrf24_ce_set(bool high) {
    if(high) ce_high(); else ce_low();
}

bool nrf24_check_connected(void) {
    nrf24_spi_init();

    // Test: SETUP_AW Reset-Wert ist 0x03
    uint8_t aw = nrf24_read_reg(NRF24_REG_SETUP_AW);
    bool ok = (aw == 0x03 || aw == 0x02 || aw == 0x01);

    // Test 2: Write/Read-Back auf RF_CH mit 2 Patterns
    if(ok) {
        nrf24_write_reg(NRF24_REG_RF_CH, 0x2A);
        uint8_t rb1 = nrf24_read_reg(NRF24_REG_RF_CH);
        nrf24_write_reg(NRF24_REG_RF_CH, 0x55);
        uint8_t rb2 = nrf24_read_reg(NRF24_REG_RF_CH);
        ok = (rb1 == 0x2A) && (rb2 == 0x55);
    }

    nrf24_spi_deinit();
    return ok;
}
