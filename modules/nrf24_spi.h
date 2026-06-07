#pragma once
#include <stdint.h>
#include <stdbool.h>

/* ------------------------------------------------------------------ */
/*  NRF24L01+ GPIO pin mapping (REK5Lab connector)                     */
/* ------------------------------------------------------------------ */
/* Adjust these to match your actual wiring */
#define NRF24_CE_PIN    &gpio_ext_pa7
#define NRF24_CSN_PIN   &gpio_ext_pa4
#define NRF24_SCK_PIN   &gpio_ext_pb3
#define NRF24_MOSI_PIN  &gpio_ext_pb2
#define NRF24_MISO_PIN  &gpio_swclk   /* PB4 = SWCLK auf Flipper Zero GPIO-Header */

/* ------------------------------------------------------------------ */
/*  NRF24L01+ Register Map                                             */
/* ------------------------------------------------------------------ */
#define NRF24_REG_CONFIG    0x00
#define NRF24_REG_EN_AA     0x01
#define NRF24_REG_EN_RXADDR 0x02
#define NRF24_REG_SETUP_AW  0x03
#define NRF24_REG_SETUP_RETR 0x04
#define NRF24_REG_RF_CH     0x05
#define NRF24_REG_RF_SETUP  0x06
#define NRF24_REG_STATUS    0x07
#define NRF24_REG_TX_ADDR   0x10
#define NRF24_REG_RX_PW_P0  0x11

/* SPI Commands */
#define NRF24_CMD_W_REGISTER    0x20
#define NRF24_CMD_W_TX_PAYLOAD  0xA0
#define NRF24_CMD_FLUSH_TX      0xE1
#define NRF24_CMD_NOP           0xFF

/* CONFIG bits */
#define NRF24_CONFIG_PWR_UP     (1 << 1)
#define NRF24_CONFIG_PRIM_TX    (0 << 0)

/* RF_SETUP values */
#define NRF24_RF_SETUP_250KBPS  0x20
#define NRF24_RF_SETUP_1MBPS    0x00
#define NRF24_RF_SETUP_2MBPS    0x08
#define NRF24_RF_SETUP_PWR_MAX  0x06

/* ------------------------------------------------------------------ */
/*  API                                                                 */
/* ------------------------------------------------------------------ */

/** Initialise GPIO pins and power up NRF24. */
void nrf24_spi_init(void);

/** Release GPIO pins, power down NRF24. */
void nrf24_spi_deinit(void);

/**
 * Write a single register byte.
 * Returns the STATUS byte received during the command.
 */
uint8_t nrf24_write_reg(uint8_t reg, uint8_t val);

/** Read a single register byte. */
uint8_t nrf24_read_reg(uint8_t reg);

/** Write multiple bytes to a register (e.g. TX_ADDR). */
void nrf24_write_buf(uint8_t reg, const uint8_t* buf, uint8_t len);

/** Send a raw SPI command (e.g. FLUSH_TX, NOP). Returns STATUS. */
uint8_t nrf24_cmd(uint8_t cmd);

/** Write TX payload (W_TX_PAYLOAD). */
void nrf24_write_tx_payload(const uint8_t* data, uint8_t len);

/** Pulse CE pin to trigger a TX burst. */
void nrf24_pulse_ce(void);

/** Prüft ob ein NRF24-Modul physisch angeschlossen ist (GPIO-SPI Write/Read-Check).
 *  Gibt true zurück wenn das Modul antwortet, false wenn nicht verbunden. */
bool nrf24_check_connected(void);
