#include "nrf24_spi.h"
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_gpio.h>

/* ------------------------------------------------------------------ */
/*  Bit-bang SPI helpers                                               */
/* ------------------------------------------------------------------ */

static inline void csn_low(void) {
    furi_hal_gpio_write(NRF24_CSN_PIN, false);
}
static inline void csn_high(void) {
    furi_hal_gpio_write(NRF24_CSN_PIN, true);
}
static inline void ce_low(void) {
    furi_hal_gpio_write(NRF24_CE_PIN, false);
}
static inline void ce_high(void) {
    furi_hal_gpio_write(NRF24_CE_PIN, true);
}
static inline void sck_low(void) {
    furi_hal_gpio_write(NRF24_SCK_PIN, false);
}
static inline void sck_high(void) {
    furi_hal_gpio_write(NRF24_SCK_PIN, true);
}
static inline void mosi_write(bool bit) {
    furi_hal_gpio_write(NRF24_MOSI_PIN, bit);
}
static inline bool miso_read(void) {
    return furi_hal_gpio_read(NRF24_MISO_PIN);
}

/* Transfer one byte MSB-first, SPI Mode 0 */
static uint8_t spi_xfer(uint8_t out) {
    uint8_t in = 0;
    for(int i = 7; i >= 0; i--) {
        mosi_write((out >> i) & 1);
        sck_high();
        in = (uint8_t)((in << 1) | miso_read());
        sck_low();
    }
    return in;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                          */
/* ------------------------------------------------------------------ */

void nrf24_spi_init(void) {
    furi_hal_gpio_init(NRF24_CE_PIN,   GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(NRF24_CSN_PIN,  GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(NRF24_SCK_PIN,  GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(NRF24_MOSI_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);
    furi_hal_gpio_init(NRF24_MISO_PIN, GpioModeInput,          GpioPullUp, GpioSpeedVeryHigh);

    ce_low();
    csn_high();
    sck_low();

    furi_delay_ms(5); /* Power-on reset */

    /* Power up, PTX mode, 2-byte CRC */
    nrf24_write_reg(NRF24_REG_CONFIG,
        NRF24_CONFIG_PWR_UP | NRF24_CONFIG_PRIM_TX | (1 << 3) | (1 << 2));
    furi_delay_ms(2); /* Tpd2stby */

    /* Disable auto-ACK */
    nrf24_write_reg(NRF24_REG_EN_AA, 0x00);

    /* Max PA, 2 Mbps */
    nrf24_write_reg(NRF24_REG_RF_SETUP, NRF24_RF_SETUP_2MBPS | NRF24_RF_SETUP_PWR_MAX);

    /* Disable retransmit */
    nrf24_write_reg(NRF24_REG_SETUP_RETR, 0x00);

    /* Broadcast TX address: 0xAAAAAAAAAA */
    const uint8_t bcast[5] = {0xAA, 0xAA, 0xAA, 0xAA, 0xAA};
    nrf24_write_buf(NRF24_REG_TX_ADDR, bcast, 5);
}

void nrf24_spi_deinit(void) {
    ce_low();
    /* Power down */
    nrf24_write_reg(NRF24_REG_CONFIG, 0x08); /* reset value, PWR_UP = 0 */

    furi_hal_gpio_init(NRF24_CE_PIN,   GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(NRF24_CSN_PIN,  GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(NRF24_SCK_PIN,  GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(NRF24_MOSI_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(NRF24_MISO_PIN, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

uint8_t nrf24_write_reg(uint8_t reg, uint8_t val) {
    csn_low();
    uint8_t status = spi_xfer(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    spi_xfer(val);
    csn_high();
    return status;
}

uint8_t nrf24_read_reg(uint8_t reg) {
    csn_low();
    spi_xfer(reg & 0x1F); /* read command = reg address */
    uint8_t val = spi_xfer(NRF24_CMD_NOP);
    csn_high();
    return val;
}

void nrf24_write_buf(uint8_t reg, const uint8_t* buf, uint8_t len) {
    csn_low();
    spi_xfer(NRF24_CMD_W_REGISTER | (reg & 0x1F));
    for(uint8_t i = 0; i < len; i++) {
        spi_xfer(buf[i]);
    }
    csn_high();
}

uint8_t nrf24_cmd(uint8_t cmd) {
    csn_low();
    uint8_t status = spi_xfer(cmd);
    csn_high();
    return status;
}

void nrf24_write_tx_payload(const uint8_t* data, uint8_t len) {
    csn_low();
    spi_xfer(NRF24_CMD_W_TX_PAYLOAD);
    for(uint8_t i = 0; i < len; i++) {
        spi_xfer(data[i]);
    }
    csn_high();
}

void nrf24_pulse_ce(void) {
    ce_high();
    /* Tce >= 10 us */
    furi_delay_us(15);
    ce_low();
}
