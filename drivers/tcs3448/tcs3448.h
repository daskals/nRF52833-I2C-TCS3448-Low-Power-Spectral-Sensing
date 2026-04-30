/*
 *  @file tcs3448.h
 *  @brief TCS3448 14-Channel Spectral Sensor driver header for nRF52 SDK
 *
 *  Adapted for nRF52 SDK from Adafruit_TCS3448 Arduino library.
 *  All Arduino/C++ dependencies removed. For use with nRF52833 and compatible MCUs.
 *
 *  Original Author: Bryan Siepert for Adafruit Industries
 *  Adaptation: [Jingjie Nan/Heriot Watt Universuty]
 *  License: BSD (see license.txt)
 */

#ifndef TCS3448_H__
#define TCS3448_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_errors.h"
#include "tcs3448_defines.h"

// Default I2C address for TCS3448
#define TCS3448_I2C_ADDR 0x59

// Register address for channel data low byte (adjust if needed)
#define TCS3448_CH0_DATA_L 0x95

/**
 * @brief Test connection to TCS3448 sensor.
 * @return true if successful, false otherwise.
 */
bool test_tcs3448_connection(void);

/**
 * @brief Configure the TCS3448 sensor (gain, atime, astep).
 * @param gain Gain setting.
 * @param atime Integration time.
 * @param astep Step count.
 * @return true if successful, false otherwise.
 */
ret_code_t tcs3448_configure(tcs3448_gain_t gain, uint8_t atime, uint16_t astep);

/**
 * @brief Read mode 1 channels from TCS3448.
 * @param buffer Buffer to store channel readings.
 * @param len Number of channels to read.
 * @return true if successful, false otherwise.
 */
bool tcs3448_read_mode1_channels(uint16_t *buffer, uint8_t len);

/**
 * @brief Read all channels (F1-F8, Clear, NIR) from the TCS3448.
 * @param readings_buffer Buffer to store 12 uint16_t readings (6 low + 6 high channels).
 * @return true if successful, false otherwise.
 */
bool tcs3448_read_all_channels(uint16_t *readings_buffer);

/**
 * @brief Enable or disable sensor power.
 * @param enable true to enable, false to disable.
 */
void tcs3448_power_enable(bool enable);

/**
 * @brief Enable or disable spectral measurement.
 * @param enable true to enable, false to disable.
 * @return true if successful, false otherwise.
 */
bool tcs3448_enable_spectral_measurement(bool enable);

/**
 * @brief Delay for data readiness/acquisition.
 * @param wait_time Time to wait in ms, or 0 to wait until data ready.
 */
void tcs3448_delay_for_data(int wait_time_ms);

/**
 * @brief Configure SMUX for low or high channels.
 * @param f1_f4 true for low channels (F1-F4, Clear, NIR), false for high channels (F5-F8, Clear, NIR).
 */
void tcs3448_set_smux_low_channels(bool f1_f4);

/**
 * @brief Check if data is ready.
 * @return true if data is ready, false otherwise.
 */
bool tcs3448_get_is_data_ready(void);

/**
 * @brief Enable SMUX.
 * @return true if successful, false otherwise.
 */
bool tcs3448_enable_smux(void);

/**
 * @brief SMUX configurations for F1-F4, Clear, NIR.
 */
void tcs3448_setup_f1f4_clear_nir(void);

/**
 * @brief SMUX configurations for F5-F8, Clear, NIR.
 */
void tcs3448_setup_f5f8_clear_nir(void);

/**
 * @brief Set the SMUX command register.
 * @param command SMUX command to set.
 * @return true if register write succeeds, false otherwise.
 */
bool tcs3448_set_smux_command(tcs3448_smux_cmd_t command);

/**
 * @brief Sample and print sensor data.
 */
void tcs3448_sample_and_print(void);

/**
 * @brief Calculate Photosynthetically Active Radiation (PAR) value from channels.
 * @return Calculated PAR value in micromoles per square meter per second (��mol/m^2/s).
 */
int32_t tcs3448_calculate_par_from_channels(const uint16_t *channel_readings);

/**
 * @brief Calculate Lux value from channels.
 * @param channel_readings Pointer to an array of channel readings.
 * @return Calculated Lux value.
 */
int32_t tcs3448_calculate_lux_from_channels(const uint16_t *channel_readings);

/**
 * @brief Initialize the TCS3448 sensor (wrapper for tcs3448_configure).
 */
ret_code_t tcs3448_init_sensor(void);

/**
 * @brief Get the current gain setting of the TCS3448 sensor.
 * @return Current gain setting.
 */
tcs3448_gain_t tcs3448_get_current_gain(void);

#endif // TCS3448_H__
