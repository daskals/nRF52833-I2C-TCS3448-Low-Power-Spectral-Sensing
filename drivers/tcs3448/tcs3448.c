// tcs3448.c
#include "tcs3448.h"
#include "i2c_interface.h"
#include "nrf_delay.h"
#include "nrf_drv_twi.h"
#include "nrf_log.h"

#include "nrf_log_ctrl.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_errors.h"

//Static variables
static tcs3448_gain_t s_tcs3448_gain = TCS3448_GAIN_256X;
static uint8_t s_atime = 0;
static uint16_t s_astep = 0;

//Test connection
bool test_tcs3448_connection(void) {
    uint8_t chip_id = 0;
    tcs3448_write_reg(TCS3448_CFG0, 0x10);  // REG_BANK=1
    if (!tcs3448_read_reg(TCS3448_ID_REG, &chip_id)) {
        tcs3448_write_reg(TCS3448_CFG0, 0x00);
        return false;
    }
    tcs3448_write_reg(TCS3448_CFG0, 0x00);  //Recovery
    NRF_LOG_INFO("TCS3448 ID_REG: 0x%02x", chip_id);
    return (chip_id == 0x81);
}

// Enable or disable sensor power
void tcs3448_power_enable(bool enable) {
    uint8_t enable_val = 0;
    tcs3448_read_reg(TCS3448_ENABLE, &enable_val);
    enable_val = (enable_val & ~0x01) | (enable ? 0x01 : 0x00);
    tcs3448_write_reg(TCS3448_ENABLE, enable_val);
}

/**
 * @brief Initialize the TCS3448 sensor with default configuration and remember gain for later use.
 */
 ret_code_t tcs3448_init_sensor(void) 
 {
    NRF_LOG_INFO("tcs3448 Init");
    s_tcs3448_gain = TCS3448_GAIN_4X;// Store gain for later normalization
    uint8_t atime = 35;
    uint16_t astep = 999;
    // Total time = (ATIME+1)*(ASTEP+1)*2.78us
    return tcs3448_configure(s_tcs3448_gain, atime, astep);
}

/**
 * @brief Configures the TCS3448 for spectral measurement.
 */
 ret_code_t tcs3448_configure(tcs3448_gain_t gain, uint8_t atime, uint16_t astep) {
    NRF_LOG_DEBUG("tcs3448_configure() called: gain=%d, atime=%u, astep=%u", gain, atime, astep);
    bool ok = true;

    ok &= tcs3448_write_reg(TCS3448_ATIME, atime);
    if (!ok) NRF_LOG_ERROR("Failed to write ATIME");
    // ASTEP LSB and MSB must be written in one burst so the sensor latches both atomically
    ok &= tcs3448_write_reg16(TCS3448_ASTEP_L, astep);
    if (!ok) NRF_LOG_ERROR("Failed to write ASTEP");
    ok &= tcs3448_write_reg(TCS3448_CFG0, 0x00);
    if (!ok) NRF_LOG_ERROR("Failed to write CFG0");
    ok &= tcs3448_write_reg(TCS3448_CFG1, (uint8_t)gain);
    if (!ok) NRF_LOG_ERROR("Failed to write CFG1");
    // CFG6 SMUX_CMD[4:3]=0b01 → execute SMUX; bit 5 is reserved and must be 0
    ok &= tcs3448_write_reg(TCS3448_CFG6, 0x10);
    if (!ok) NRF_LOG_ERROR("Failed to write CFG6");
    // CFG20 auto_smux=3 (bits[6:5]=0b11) → 18-channel automatic measurement
    ok &= tcs3448_write_reg(TCS3448_CFG20, 0x60);
    if (!ok) NRF_LOG_ERROR("Failed to write CFG20 (auto_smux)");

    s_tcs3448_gain = gain;
    s_atime = atime;
    s_astep = astep;

    return ok ? NRF_SUCCESS : NRF_ERROR_INTERNAL;
}

// Enable spectral measurement
bool tcs3448_enable_spectral_measurement(bool enable) {
    uint8_t enable_val = 0;
    tcs3448_read_reg(TCS3448_ENABLE, &enable_val);
    enable_val = (enable_val & ~0x02) | (enable ? 0x02 : 0x00);
    return tcs3448_write_reg(TCS3448_ENABLE, enable_val);
}

// Read all channels
bool tcs3448_read_all_channels(uint16_t *readings_buffer) {
    if (readings_buffer == NULL) return false;

    tcs3448_power_enable(true);
    tcs3448_enable_spectral_measurement(true);
    tcs3448_delay_for_data(0);

    // Read ASTATUS (0x94) + 36 channel bytes (0x95–0xB8) in one burst.
    // Reading ASTATUS first latches all 18 channels coherently before parsing.
    uint8_t buf[37] = {0};
    if (!i2c_read_bytes(TCS3448_I2CADDR_DEFAULT, TCS3448_ASTATUS, buf, 37)) {
        NRF_LOG_ERROR("Failed to read channels");
        tcs3448_enable_spectral_measurement(false);
        tcs3448_power_enable(false);
        return false;
    }

    // buf[0] = ASTATUS snapshot; channel data starts at buf[1]
    for (int i = 0; i < 18; i++) {
        readings_buffer[i] = (uint16_t)(buf[1 + i * 2] | (buf[1 + i * 2 + 1] << 8));
    }

    tcs3448_enable_spectral_measurement(false);
    tcs3448_power_enable(false);
    return true;
}

// Delay for data readiness (nRF52 style, no POSIX timespec)
void tcs3448_delay_for_data(int wait_time_ms) {
    if (wait_time_ms == 0) {
        // Wait until data ready
        while (!tcs3448_get_is_data_ready()) {
            //nrf_delay_ms(1);
            nrf_delay_ms(1);
        }
    } else if (wait_time_ms > 0) {
        uint32_t elapsed = 0;
        while (!tcs3448_get_is_data_ready() && elapsed < (uint32_t)wait_time_ms) {
            nrf_delay_ms(1);
            elapsed++;
        }
    }
}

//Check if data ready
bool tcs3448_get_is_data_ready(void) {
    uint8_t status = 0;
    if (!tcs3448_read_reg(TCS3448_STATUS2, &status)) {
        return false;
    }
    return (status & 0x40) != 0;// AVALID bit
}

/**
 * @brief Print the TCS3448 channel readings.
 */
 void tcs3448_sample_and_print(void) {
    uint16_t channel_readings[18] = {0};
    if (tcs3448_read_all_channels(channel_readings)) {
        NRF_LOG_INFO("------------------------------");
        NRF_LOG_INFO("TCS3448 Spectral Data:");
        // auto_smux=3 cycle order per CFG20 table (datasheet Table 46)
        // Cycle 1
        NRF_LOG_INFO("  FZ   (Cycle 1): %u", channel_readings[0]);
        NRF_LOG_INFO("  FY   (Cycle 1): %u", channel_readings[1]);
        NRF_LOG_INFO("  FXL  (Cycle 1): %u", channel_readings[2]);
        NRF_LOG_INFO("  NIR  (Cycle 1): %u", channel_readings[3]);
        NRF_LOG_INFO("  VIS  (Cycle 1): %u", channel_readings[4]);
        NRF_LOG_INFO("  FD   (Cycle 1): %u", channel_readings[5]);
        // Cycle 2
        NRF_LOG_INFO("  F2   (445 nm) : %u", channel_readings[6]);
        NRF_LOG_INFO("  F3   (480 nm) : %u", channel_readings[7]);
        NRF_LOG_INFO("  F4   (515 nm) : %u", channel_readings[8]);
        NRF_LOG_INFO("  F6   (590 nm) : %u", channel_readings[9]);
        NRF_LOG_INFO("  VIS  (Cycle 2): %u", channel_readings[10]);
        NRF_LOG_INFO("  FD   (Cycle 2): %u", channel_readings[11]);
        // Cycle 3
        NRF_LOG_INFO("  F1   (415 nm) : %u", channel_readings[12]);
        NRF_LOG_INFO("  F7   (630 nm) : %u", channel_readings[13]);
        NRF_LOG_INFO("  F8   (680 nm) : %u", channel_readings[14]);
        NRF_LOG_INFO("  F5   (555 nm) : %u", channel_readings[15]);
        NRF_LOG_INFO("  VIS  (Cycle 3): %u", channel_readings[16]);
        NRF_LOG_INFO("  FD   (Cycle 3): %u", channel_readings[17]);
        NRF_LOG_INFO("------------------------------");
        int32_t par_value = tcs3448_calculate_par_from_channels(channel_readings);
        NRF_LOG_INFO("  PAR: %d umol/m2/s", par_value);
        NRF_LOG_INFO("------------------------------");
    } else {
        NRF_LOG_WARNING("TCS3448 channel read failed.");
    }
}

static const float par_regression_coeffs[8]= {
/*
//brief Regression-calibrated coefficients for PAR
-5.40f,
-0.34f,
4.88f,
-2.00f,
-6.83f,
5.34f,
-1.62f,
-5.49f
*/

//weighted non-negative least squares coefficients
0.010000003f,
0.010000011f,
0.183731419f,
0.125531209f,
0.01f,
0.01f,
0.01f,
1.564951432f
};

//brief Gain factors for each supported TCS3448 gain setting.
static const float gain_factors[] = {
    0.5f,    //TCS3448_GAIN_0_5X
    1.0f,    //TCS3448_GAIN_1X
    2.0f,    //TCS3448_GAIN_2X
    4.0f,    //TCS3448_GAIN_4X
    8.0f,    //TCS3448_GAIN_8X
    16.0f,   //TCS3448_GAIN_16X
    32.0f,   //TCS3448_GAIN_32X
    64.0f,   //TCS3448_GAIN_64X
    128.0f,  //TCS3448_GAIN_128X
    256.0f,  //TCS3448_GAIN_256X
    512.0f,  //TCS3448_GAIN_512X
    1024.0f, //TCS3448_GAIN_1024X
    2048.0f, //TCS3448_GAIN_2048X
};

/**
 * @brief Calculate PAR index from channel readings using regression coefficients.
 *
 * @param[in] channel_readings Pointer to array of channel readings.
 * @return int32_t Calculated PAR index, or -1 on error.
 */
int32_t tcs3448_calculate_par_from_channels(const uint16_t *channel_readings) 
{
    // Validate input pointer
    if (channel_readings == NULL) {
        NRF_LOG_ERROR("tcs3448_calculate_par_from_channels: NULL channel_readings");
        return -1;
    }
    // Validate gain index
    if (s_tcs3448_gain >= (sizeof(gain_factors) / sizeof(gain_factors[0]))) {
        NRF_LOG_ERROR("tcs3448_calculate_par_from_channels: Invalid gain index");
        return -1;
    }

    float gain = gain_factors[s_tcs3448_gain];

    float int_time_ms = (s_atime + 1.0f) * (s_astep + 1.0f) * 0.00278f;
    int whole = (int)int_time_ms;
    int decimal = (int)((int_time_ms - whole) * 100);
    /*NRF_LOG_INFO("Integration time = %d.%02d ms", whole, decimal);*/

    float par = 0.0f;

    // Apply regression coefficients to F1-F8 channels'raw counts
    par += par_regression_coeffs[4] * channel_readings[12];// F1
    par += par_regression_coeffs[0] * channel_readings[6];// F2
    par += par_regression_coeffs[1] * channel_readings[7];// F3
    par += par_regression_coeffs[2] * channel_readings[8];// F4
    par += par_regression_coeffs[7] * channel_readings[15];// F5
    par += par_regression_coeffs[3] * channel_readings[9];// F6
    par += par_regression_coeffs[5] * channel_readings[13];// F7
    par += par_regression_coeffs[6] * channel_readings[14];// F8

    if (par < 0) par = 0;
    return (int32_t)(par);
}
