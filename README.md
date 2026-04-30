# nRF52833-I2C-TCS3448-Low-Power-Spectral-Sensing

Low-power firmware for Nordic Semiconductor's nRF52833 DK to interface with the TCS3448 multi-channel spectral sensor over I2C. Designed for outdoor applications such as PAR (Photosynthetically Active Radiation) monitoring and spectral analysis.

## Hardware

- **MCU**: nRF52833 DK (PCA10100)
- **Sensor**: TCS3448 14-channel spectral sensor (AMS)
- **Interface**: I2C / TWI at 400 kHz (3.3 V logic)
- **Use Case**: Outdoor spectral measurement and PAR sensing

### I2C Pin Assignment

| Signal | nRF52833 Pin |
|--------|-------------|
| SDA    | **P0.19**   |
| SCL    | **P0.17**   |

> **External pull-up resistors are required on both SDA and SCL lines.**
> The firmware explicitly disables the nRF52833 internal pull-ups after TWI init so they do not conflict with the external resistors. Typical values: 4.7 kΩ to 3.3 V for 400 kHz operation.

### I2C Address

| Parameter | Value |
|-----------|-------|
| Default 7-bit address | `0x59` |
| Chip ID (register 0x5A) | `0x81` |

## Sensor Configuration

| Parameter | Value |
|-----------|-------|
| Gain      | `TCS3448_GAIN_4X` |
| ATIME     | 35 |
| ASTEP     | 999 |
| Integration time | ~100 ms |
| Channels  | F1–F8 (415–680 nm), FZ, FY, FXL, VIS, NIR × 2 passes (18 total) |

### Integration Time Formula

```
t_int = (ATIME + 1) × (ASTEP + 1) × 2.78 µs
      = 36 × 1000 × 2.78 µs ≈ 100 ms
```

### Measurement Sequence

The TCS3448 uses `auto_smux` mode (CFG20 = 0x60), which lets the hardware automatically route spectral channels to the 6 ADC engines across two internal passes. `tcs3448_read_all_channels()` reads 18 × 16-bit values starting at register `0x95` (CH0_DATA_L):

| Index | Channel |
|-------|---------|
| 0–5   | Pass 1 ADC outputs |
| 6–11  | Pass 2 ADC outputs |
| 12–17 | Extended channels (FZ, FY, FXL, VIS, NIR, FD) |

### Gain Settings

| Enum | Multiplier |
|------|-----------|
| `TCS3448_GAIN_0_5X` | 0.5× |
| `TCS3448_GAIN_1X`   | 1× |
| `TCS3448_GAIN_2X`   | 2× |
| `TCS3448_GAIN_4X`   | 4× (default) |
| `TCS3448_GAIN_8X`   | 8× |
| `TCS3448_GAIN_16X`  | 16× |
| `TCS3448_GAIN_32X`  | 32× |
| `TCS3448_GAIN_64X`  | 64× |
| `TCS3448_GAIN_128X` | 128× |
| `TCS3448_GAIN_256X` | 256× |
| `TCS3448_GAIN_512X` | 512× |
| `TCS3448_GAIN_1024X`| 1024× |
| `TCS3448_GAIN_2048X`| 2048× |

### Computed Outputs

| Output | Description |
|--------|-------------|
| PAR    | Photosynthetically Active Radiation (µmol/m²/s), weighted non-negative least-squares regression across F1–F8 spectral bands |

## Low Power Strategy

1. DCDC converter enabled (`NRF_POWER->DCDCEN = 1`)
2. RTC2 used for 1-second periodic wake-ups (32 Hz LFCLK, lower power than TIMER)
3. `nrf_pwr_mgmt_run()` in main loop → WFE between events
4. Sensor power is enabled only during acquisition and disabled immediately after

## Software Setup

| Item | Details |
|------|---------|
| SDK  | Nordic nRF5 SDK 17.1.0 |
| Toolchain | SEGGER Embedded Studio for ARM (v5.42a or later) |
| SoftDevice | Not required |
| Logging | SEGGER RTT (J-Link on-board) |

### Getting Started

1. Copy the project folder into:
   ```
   nRF5_SDK_17.1.0_ddde560/examples/peripheral/
   ```
2. Open the `.emProject` file in:
   ```
   pca10100/blank/ses/tcs3448_pca10100.emProject
   ```
3. **Build and Debug** (F5 in SES) — do **not** just flash and run. RTT output is only visible when the debugger is attached.
4. Open the **Debug Terminal** tab (or use SEGGER J-Link RTT Viewer) to see log output.

> RTT messages will not appear if you flash without the debugger attached. Always use **Build → Debug** to start a debug session and view the output.

## Directory Structure

```
nRF52833-I2C-TCS3448-Low-Power-Spectral-Sensing/
├── main.c                              # App entry point: RTC, sensor init, main loop
├── drivers/
│   ├── tcs3448/
│   │   ├── tcs3448.c                   # Sensor driver implementation
│   │   ├── tcs3448.h                   # Driver API
│   │   └── tcs3448_defines.h           # Register map, enums, bit masks
│   └── i2c/
│       ├── i2c_interface.c             # TWI abstraction (nRF SDK nrf_drv_twi)
│       └── i2c_interface.h             # I2C API
├── pca10100/blank/ses/
│   └── tcs3448_pca10100.emProject      # SEGGER Embedded Studio project file
├── pca10100/blank/config/
│   └── sdk_config.h                    # nRF5 SDK peripheral configuration
└── TCS3448/                            # TCS3448 datasheet (PDF)
```

## Troubleshooting

| Symptom | Likely Cause | Fix |
|---------|-------------|-----|
| `ANACK` error on I2C | Sensor not powered or not ready | Check sensor VDD; verify 3.3 V is present before the MCU starts I2C |
| `ANACK` on every transaction | Missing pull-up resistors | Add external 4.7 kΩ pull-ups on SDA/SCL to 3.3 V |
| ID register returns wrong value | Wrong I2C address or REG_BANK not set | `test_tcs3448_connection()` temporarily sets REG_BANK=1 via CFG0 to access register 0x5A; verify wiring |
| No RTT output | Debug session not started | Use Build → Debug, not just flash/run |
| All channel readings are 0 | Integration time too short or gain too low | Increase ATIME or GAIN in `tcs3448_init_sensor()` |
| Saturated readings (65535) | Gain too high | Reduce gain in `tcs3448_init_sensor()` |

## References

- [TCS3448 Datasheet – AMS](https://ams.com/tcs3448)
- [nRF52833 DK – Nordic Semiconductor](https://www.nordicsemi.com/Products/nRF52833)
- [nRF5 SDK 17.1.0 Documentation](https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/index.html)
- Adapted from: [`nRF52833-I2C-AS7341-Low-Power-Spectral-Sensing`](https://github.com/daskals/nRF52833-I2C-AS7341-Low-Power-Spectral-Sensing)

## License

MIT License
