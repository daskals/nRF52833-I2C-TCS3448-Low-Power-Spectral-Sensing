/*
 *  @file tcs3448_defines.h
 *  @brief Register definitions and enums for the TCS3448 14-Channel Spectral Sensor
 *
 *  Adapted for nRF52 SDK integration from the original Adafruit Arduino library.
 *  Remove all Arduino dependencies. For use with nRF52833 and compatible MCUs.
 *
 *  Original Author: Bryan Siepert for Adafruit Industries
 *  Adaptation: [ Jingjie Nan/Heriot Watt University]
 *  License: BSD (see license.txt)
 */


#ifndef _TCS3448_DEFINES_H
#define _TCS3448_DEFINES_H


// I2C Address / Identification
#define TCS3448_I2CADDR_DEFAULT 0x59 ///< TCS3448 default 7-bit I2C addr
#define TCS3448_CHIP_ID         0x81 ///< Expected value read from ID reg (0x5A)
#define TCS3448_AUXID    0x58 ///< AUXID register (aux identification) 
#define TCS3448_REVID    0x59 ///< REVID register (revision ID)    
#define TCS3448_ID_REG   0x5A ///< ID register (returns 0x81)

// Core configuration / timing / threshold
#define TCS3448_ENABLE      0x80 ///< Main enable: PON, ALS_EN, FDEN, WEN, etc.
#define TCS3448_ATIME       0x81 ///< ALS integration time cycles (ATIME)      
#define TCS3448_WTIME       0x83 ///< Wait time between measurement cycles 

#define TCS3448_ALS_TH_L_L  0x84 ///< ALS low-threshold LSB for interrupt 
#define TCS3448_ALS_TH_L_H  0x85 ///< ALS low-threshold MSB
#define TCS3448_ALS_TH_H_L  0x86 ///< ALS high-threshold LSB 
#define TCS3448_ALS_TH_H_H  0x87 ///< ALS high-threshold MSB

#define TCS3448_CFG10       0x65 ///< Flicker detect persistence FD_PERS[2:0]
#define TCS3448_CFG12       0x66 ///< ALS_TH_CH[2:0] => which channel drives thresholds/persistence

//Integration step size ASTEP works together with ATIME.
//tint(ms) = (ATIME+1) * (ASTEP+1) * 2.78µs

#define TCS3448_ASTEP_L     0xD4 ///< Integration step size LSB (ASTEP[7:0])   
#define TCS3448_ASTEP_H     0xD5 ///< Integration step size MSB (ASTEP[15:8])

// Auto-zero / gain ceiling / auto-SMUX / flicker timing
#define TCS3448_AZ_CONFIG   0xDE ///< Auto-zero frequency for ALS engines     
#define TCS3448_AGC_GAIN_MAX 0xD7 ///< Max AGC gain for flicker detect, etc.    
#define TCS3448_CFG8        0xC9 ///< FIFO_TH, etc. (FIFO threshold config)    
#define TCS3448_CFG20       0xD6 ///< auto_smux mode / FD_FIFO_8b          
#define TCS3448_FD_TIME1    0xE0 ///< Flicker detection integration timing LSB
#define TCS3448_FD_TIME2    0xE2 ///< Flicker detection timing MSB + gain
#define TCS3448_FD_CFG0     0xDF ///< FIFO write control for flicker data

// Status / interrupt / ID
#define TCS3448_STATUS2     0x90 ///< AVALID, ASAT_DIGITAL/ANALOG, FDSAT_*
#define TCS3448_STATUS3     0x91 ///< INT_ALS_H / INT_ALS_L
#define TCS3448_STATUS      0x93 ///< Primary status: ASAT, AINT, FINT, SINT
#define TCS3448_ASTATUS     0x94 ///< "ALS status" snapshot; also can be FIFO'ed
                                 ///< (ASTATUS is written to FIFO if enabled)
#define TCS3448_STATUS5     0xBB ///< SINT_FD / SINT_SMUX (system int source)
#define TCS3448_STATUS4     0xBC ///< FIFO_OV, OVTEMP, FD_TRIG, ALS_TRIG,
                                 ///< SAI_ACTIVE, INT_BUSY  

#define TCS3448_ALS_INT_HIGH_MSK 0b00100000 ///< INT_ALS_H (high threshold crossed)
#define TCS3448_ALS_INT_LOW_MSK  0b00010000 ///< INT_ALS_L (low threshold crossed)

// ADC result registers
#define TCS3448_CH0_DATA_L  0x95 ///< ADC CH0 data low byte
#define TCS3448_CH0_DATA_H  0x96 ///< ADC CH0 data high byte
#define TCS3448_CH1_DATA_L  0x97 ///< ADC CH1 data low byte
#define TCS3448_CH1_DATA_H  0x98 ///< ADC CH1 data high byte
#define TCS3448_CH2_DATA_L  0x99 ///< ADC CH2 data low byte
#define TCS3448_CH2_DATA_H  0x9A ///< ADC CH2 data high byte
#define TCS3448_CH3_DATA_L  0x9B ///< ADC CH3 data low byte
#define TCS3448_CH3_DATA_H  0x9C ///< ADC CH3 data high byte
#define TCS3448_CH4_DATA_L  0x9D ///< ADC CH4 data low byte
#define TCS3448_CH4_DATA_H  0x9E ///< ADC CH4 data high byte
#define TCS3448_CH5_DATA_L  0x9F ///< ADC CH5 data low byte
#define TCS3448_CH5_DATA_H  0xA0 ///< ADC CH5 data high byte
#define TCS3448_CH6_DATA_L  0xA1 ///< ADC CH6 data low byte
#define TCS3448_CH6_DATA_H  0xA2 ///< ADC CH6 data high byte
#define TCS3448_CH7_DATA_L  0xA3 ///< ADC CH7 data low byte
#define TCS3448_CH7_DATA_H  0xA4 ///< ADC CH7 data high byte

// Misc config blocks (banked / misc control)
#define TCS3448_CFG0        0xBF ///< LOW_POWER, REG_BANK, WLONG, etc.
#define TCS3448_CFG1        0xC6 ///< AGAIN[4:0] (ALS gain control 0.5x..2048x)
#define TCS3448_CFG3        0xC7 ///< SAI (sleep-after-interrupt)
#define TCS3448_CFG6        0xF5 ///< SMUX_CMD[4:3] (SMUX command control)
#define TCS3448_CFG9        0xCA ///< SIEN_FD / SIEN_SMUX system int enables

#define TCS3448_PERS        0xCF ///< APERS[3:0]: ALS interrupt persistence

#define TCS3448_GPIO        0x6B ///< GPIO polarity/dir/in/out      
#define TCS3448_LED         0xCD ///< LED_ACT / LED_DRIVE[6:0] current limit

#define TCS3448_FD_STATUS   0xE3 ///< Flicker Detect status bits (FD_VALID...) 

#define TCS3448_INTENAB     0xF9 ///< Interrupt enable: ASIEN / ALS_IEN / FIEN / SIEN
#define TCS3448_CONTROL     0xFA ///< SW_RESET, ALS_MAN_AZ, FIFO_CLR, CLEAR_SAI_ACT

#define TCS3448_FIFO_MAP    0xFC ///< Which CHx / ASTATUS / FD data go into FIFO
#define TCS3448_FIFO_LVL    0xFD ///< FIFO fill level (# of 2-byte entries) 
#define TCS3448_FDATA_L     0xFE ///< FIFO data LSB (auto-increment readout) 
#define TCS3448_FDATA_H     0xFF ///< FIFO data MSB

// Gain multipliers
// CFG1.AGAIN[4:0] supports programmable ALS gain steps:
// 0:0.5x, 1:1x, 2:2x, 3:4x, 4:8x, 5:16x, 6:32x, 7:64x,
// 8:128x, 9:256x, 10:512x, 11:1024x, 12:2048x.
typedef enum {
  TCS3448_GAIN_0_5X = 0,
  TCS3448_GAIN_1X,
  TCS3448_GAIN_2X,
  TCS3448_GAIN_4X,
  TCS3448_GAIN_8X,
  TCS3448_GAIN_16X,
  TCS3448_GAIN_32X,
  TCS3448_GAIN_64X,
  TCS3448_GAIN_128X,
  TCS3448_GAIN_256X,
  TCS3448_GAIN_512X,
  TCS3448_GAIN_1024X,
  TCS3448_GAIN_2048X,
} tcs3448_gain_t;

// SMUX command set
typedef enum {
  TCS3448_SMUX_CMD_ROM_RESET = 0, ///< ROM code init of SMUX
  TCS3448_SMUX_CMD_READ      = 1, ///< Read SMUX chain into RAM
  TCS3448_SMUX_CMD_WRITE     = 2, ///< Write RAM config back into SMUX chain
} tcs3448_smux_cmd_t;

// ADC channel specifiers (the physical ADC engines 0..5)
typedef enum {
  TCS3448_ADC_CHANNEL_0 = 0,
  TCS3448_ADC_CHANNEL_1,
  TCS3448_ADC_CHANNEL_2,
  TCS3448_ADC_CHANNEL_3,
  TCS3448_ADC_CHANNEL_4,
  TCS3448_ADC_CHANNEL_5,
} tcs3448_adc_channel_t;

// Spectral / ALS channel specifiers
// (FZ, FY, FXL), near-IR (NIR), "2xVIS"/CLEAR style broadband, and FD
typedef enum {
  TCS3448_CHANNEL_F1,
  TCS3448_CHANNEL_F2,
  TCS3448_CHANNEL_F3,
  TCS3448_CHANNEL_F4,
  TCS3448_CHANNEL_F5,
  TCS3448_CHANNEL_F6,
  TCS3448_CHANNEL_F7,
  TCS3448_CHANNEL_F8,
  TCS3448_CHANNEL_FZ,     ///< extra visible/narrowband slot used in auto_smux
  TCS3448_CHANNEL_FY,     ///< extra visible/narrowband slot
  TCS3448_CHANNEL_FXL,    ///< broadband / extended-long filter path
  TCS3448_CHANNEL_VIS,    ///< wide visible / clear / 2xVIS aggregation
  TCS3448_CHANNEL_NIR,    ///< near infrared channel
  TCS3448_CHANNEL_FD,     ///< flicker detect pseudo-channel (not standard ADC)
} tcs3448_color_channel_t;

// Interrupt persistence
// PERS (0xCF).APERS[3:0] defines how many consecutive ALS samples must stay
// out of threshold before ALS_IEN will flag an interrupt in STATUS / STATUS3. 
// 0 = every time, 1=1, 2=2, 3=3, 4=5, 5=10, ... up to
typedef enum {
  TCS3448_INT_COUNT_ALL = 0, ///< interrupt every cycle
  TCS3448_INT_COUNT_1,       ///< 1
  TCS3448_INT_COUNT_2,       ///< 2
  TCS3448_INT_COUNT_3,       ///< 3
  TCS3448_INT_COUNT_5,       ///< 4
  TCS3448_INT_COUNT_10,      ///< 5
  TCS3448_INT_COUNT_15,      ///< 6
  TCS3448_INT_COUNT_20,      ///< 7
  TCS3448_INT_COUNT_25,      ///< 8
  TCS3448_INT_COUNT_30,      ///< 9
  TCS3448_INT_COUNT_35,      ///< 10
  TCS3448_INT_COUNT_40,      ///< 11
  TCS3448_INT_COUNT_45,      ///< 12
  TCS3448_INT_COUNT_50,      ///< 13
  TCS3448_INT_COUNT_55,      ///< 14
  TCS3448_INT_COUNT_60,      ///< 15
} tcs3448_int_cycle_count_t;

// GPIO direction / usage
// TCS3448 GPIO register (0x6B) exposes GPIO_INV, GPIO_IN_EN, GPIO_OUT, GPIO_IN. 
// - If GPIO_IN_EN = 1, pin behaves as input (hi-Z input buffer enabled).
// - Otherwise you drive it via GPIO_OUT (open drain style for INT/LED/etc.). 
typedef enum {
  TCS3448_GPIO_OUTPUT = 0, ///< Use as (open-drain style) output via GPIO_OUT
  TCS3448_GPIO_INPUT,      ///< Use as high-impedance input (GPIO_IN_EN=1)
} tcs3448_gpio_dir_t;

// Async read wait state helper (kept for driver flow parity)
// for INT_ALS_* or AVALID -> done".
typedef enum {
  TCS3448_WAITING_START = 0,
  TCS3448_WAITING_LOW,
  TCS3448_WAITING_HIGH,
  TCS3448_WAITING_DONE,
} tcs3448_waiting_t;

#endif // _TCS3448_DEFINES_H
