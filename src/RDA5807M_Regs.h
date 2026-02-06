//================================================= (c) Kolesov Andrey & Qwen ==
// RDA5807M Register Bitfield Definitions
// Based on official datasheet v1.1 and v1.8
//------------------------------------------------------------------------------
#include <stdint.h>

// ========================================================
// Register 0x00: CHIP ID (Read-only) CHIPID = 0x58
// ========================================================
struct __attribute__((packed)) RDA5807M_REG00 {
  uint16_t UNUSED : 8; // [7:0]   Reserved / unused
  uint16_t CHIPID : 8; // [15:8]  Chip ID, default: 0x58
};
static_assert(sizeof(RDA5807M_REG00) == 2, "RDA5807M_REG00 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG00 bits;
} RDA5807M_REG00_U;

// ========================================================
// Register 0x01: Unused.
// ========================================================
struct __attribute__((packed)) RDA5807M_REG01 {
  uint16_t UNUSED : 16; // [15:0]   Reserved / unused
};
static_assert(sizeof(RDA5807M_REG01) == 2, "RDA5807M_REG01 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG01 bits;
} RDA5807M_REG01_U;

// ========================================================
// Register 0x02: Power, Seek & General Control
// ========================================================
struct __attribute__((packed)) RDA5807M_REG02 {
  uint16_t ENABLE : 1;             // [0]   Power Up Enable: 0=Disabled, 1=Enabled
  uint16_t SOFT_RESET : 1;         // [1]   Soft Reset: 0=Normal, 1=Reset (auto-clear)
  uint16_t NEW_METHOD : 1;         // [2]   New Demodulate Method: improves sensitivity ~1dB
  uint16_t RDS_EN : 1;             // [3]   RDS/RBDS Enable: 1=Enabled
  uint16_t CLK_MODE : 3;           // [6:4] Clock Mode:
                                   //       000=32.768kHz, 001=12MHz, 010=13MHz, 011=19.2MHz
                                   //       101=24MHz, 110=26MHz, 111=38.4MHz
  uint16_t SKMODE : 1;             // [7]   Seek Mode: 0=Wrap at band limits, 1=Stop at limits
  uint16_t SEEK : 1;               // [8]   Seek Enable: 0=Stop, 1=Start seek in direction of SEEKUP
  uint16_t SEEKUP : 1;             // [9]   Seek Direction: 0=Down, 1=Up
  uint16_t RCLK_DIRECT : 1;        // [10]  RCLK Direct Input Mode: 1=RCLK clock use the directly input mode
  uint16_t RCLK_NON_CALIBRATE : 1; // [11]  RCLK not always supply (limits temp stability)
  uint16_t BASS : 1;               // [12]  Bass Boost: 0=Off, 1=On
  uint16_t MONO : 1;               // [13]  Force Mono: 0=Stereo, 1=Mono
  uint16_t DMUTE : 1;              // [14]  Mute Disable: 0=Muted, 1=Normal operation
  uint16_t DHIZ : 1;               // [15]  Audio High-Z: 0=High-Z, 1=Normal operation
};
static_assert(sizeof(RDA5807M_REG02) == 2, "RDA5807M_REG02 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG02 bits;
} RDA5807M_REG02_U;

// ========================================================
// Register 0x03: Channel & Band Settings
// ========================================================
struct __attribute__((packed)) RDA5807M_REG03 {
  uint16_t SPACE : 2;       // [1:0] Channel Spacing:
                            //       00=100kHz, Европа, СНГ, Азия, Африка, Австралия, большая часть мира
                            //       01=200kHz, США, Канада, Мексика
                            //       10=50kHz,  Япония
                            //       11=25kHz   Экспериментальные/любительские диапазоны, военные системы
  uint16_t BAND : 2;        // [3:2] Band Select:
                            //       00=87–108 MHz (US/Europe)
                            //       01=76–91 MHz (Japan)
                            //       10=76–108 MHz (Worldwide)
                            //       11=65–76 MHz (East Europe) or 50–65 MHz
  uint16_t TUNE : 1;        // [4]   Tune: 0=Disable, 1=Enable (auto-clears on completion)
  uint16_t DIRECT_MODE : 1; // [5]   Direct Mode (test only)
  uint16_t CHAN : 10;       // [15:6] Channel Select:
                            //        Frequency = Base + CHAN * Spacing
                            //        Base: 87.0 (BAND=0), 76.0 (BAND=1,2), 65.0 (BAND=3)
};
static_assert(sizeof(RDA5807M_REG03) == 2, "RDA5807M_REG03 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG03 bits;
} RDA5807M_REG03_U;

// ========================================================
// Register 0x04: Audio & AFC Settings
// ========================================================
struct __attribute__((packed)) RDA5807M_REG04 {
  uint16_t GPIO1 : 2;        // [1:0] General Purpose I/O 1. when gpio_sel=01
                             //       00 = High impedance
                             //       01 = Reserved
                             //       10 = Low
                             //       11 = High
  uint16_t GPIO2 : 2;        // [3:2] General Purpose I/O 2. when gpio_sel=01
  uint16_t GPIO3 : 2;        // [5:4] General Purpose I/O 3. when gpio_sel=01
  uint16_t I2S_ENABLE : 1;   // [6] I2S enable: 0 = disabled, 1 = enabled.
  uint16_t UNUSED_7 : 1;     // [7] Reserved
  uint16_t AFCD : 1;         // [8]  AFC Disable: 0=AFC enabled, 1=AFC disabled
  uint16_t SOFTMUTE_EN : 1;  // [9]  Soft Mute Enable: 1=Enabled
  uint16_t RDS_FIFO_CLR : 1; // [10] RDS fifo clear: 1=Clear
  uint16_t DE : 1;           // [11] De-emphasis: 0=75µs (Europe), 1=50µs (US)
  uint16_t RDS_FIFO_ENU : 1; // [12] RDS fifo mode enabled: 1=Enabled
  uint16_t RBDS : 1;         // [13] RBDS mode Enable: 1=Enabled, 0=RDS mode only
  uint16_t STCIEN : 1;       // [14] Seek/Tune Complete Interrupt Enable: 1=Enable,
                             //      will generate a low pulse on GPIO2 when the interrupt occurs
  uint16_t UNUSED_ : 1;      // [15] Reserved
};
// Note: DE bit position may vary; in some docs it's bit 11, not 11 — check your chip version
static_assert(sizeof(RDA5807M_REG04) == 2, "RDA5807M_REG04 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG04 bits;
} RDA5807M_REG04_U;

// ========================================================
// Register 0x05: Volume, Seek Threshold
// ========================================================
struct __attribute__((packed)) RDA5807M_REG05 {
  uint16_t VOLUME : 4;     // [3:0]  Volume (DAC Gain): 0000=min (mute), 1111=max
  uint16_t UNUSED_4_5 : 2; // [5:4]  Reserved
  uint16_t UNUSED_6_7 : 2; // [7:6]  Reserved
  uint16_t SEEKTH : 4;     // [11:8] Seek SNR Threshold (default 1000b ≈ 32dB)
  uint16_t UNUSED_12 : 1;  // [12] Reserved
  uint16_t SEEK_MODE : 2;  // [14:13] Default value is 00; When = 10, will add the RSSI seek mode.
  uint16_t INT_MODE : 1;   // [15]   Interrupt Mode: 0=5ms pulse, 1=latched until reg 0x0C read
};
static_assert(sizeof(RDA5807M_REG05) == 2, "RDA5807M_REG05 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG05 bits;
} RDA5807M_REG05_U;

// ========================================================
// Register 0x06: Reserved (Open Mode)
// ========================================================
struct __attribute__((packed)) RDA5807M_REG06 {
  uint16_t UNUSED_0_12 : 13; // [12:0] Reserved
  uint16_t OPEN_MODE : 2;    // [14:13] Open reserved register mode: 11=enable write, others=read-only
  uint16_t UNUSED_15 : 1;    // [15] Reserved
};
static_assert(sizeof(RDA5807M_REG06) == 2, "RDA5807M_REG06 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG06 bits;
} RDA5807M_REG06_U;

// ========================================================
// Register 0x07: Seek & Blend Settings
// ========================================================
struct __attribute__((packed)) RDA5807M_REG07 {
  uint16_t FREQ_MODE : 1;    // [0]   Frequency Direct Mode: 1 = use FREQ_DIRECT (reg 0x08) for freq setting
  uint16_t SOFTBLEND_EN : 1; // [1]   Soft Blend Enable
  uint16_t SEEK_TH_OLD : 6;  // [7:2] Seek threshold (old mode), valid if Seek_Mode=001
  uint16_t UNUSED_8 : 1;     // [8]   Reserved
  uint16_t MODE_65_50M : 1;  // [9]   Band mode when BAND=3: 1=65–76 MHz, 0=50–76 MHz
  uint16_t TH_SOFTBLEND : 5; // [14:10] Threshold for noise soft blend (unit: 2dB), default 10000b
  uint16_t UNUSED_15 : 1;    // [15] Reserved
};
static_assert(sizeof(RDA5807M_REG07) == 2, "RDA5807M_REG07 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG07 bits;
} RDA5807M_REG07_U;

// ========================================================
// Register 0x08: Frequency direct mode
// ========================================================
struct __attribute__((packed)) RDA5807M_REG08 {
  uint16_t FREQ_DIRECT : 16; // [15:0] Frequency Direct in kHz, valid if FREQ_MODE=1
};
static_assert(sizeof(RDA5807M_REG08) == 2, "RDA5807M_REG08 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG08 bits;
} RDA5807M_REG08_U;

// ========================================================
// Register 0x09: Unused.
// ========================================================
struct __attribute__((packed)) RDA5807M_REG09 {
  uint16_t UNUSED : 16; // [15:0]   Reserved / unused
};
static_assert(sizeof(RDA5807M_REG09) == 2, "RDA5807M_REG09 must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG09 bits;
} RDA5807M_REG09_U;

// ========================================================
// Register 0x0A: Status & RDS Ready
// ========================================================
struct __attribute__((packed)) RDA5807M_REG0A {
  uint16_t READCHAN : 10; // [9:0]  Read Channel (current tuned channel)
  uint16_t ST : 1;        // [10]   Stereo Indicator: 0=Mono, 1=Stereo
  uint16_t BLK_E : 1;     // [11]   Block E found (RDS)
  uint16_t RDSS : 1;      // [12]   RDS Synchronized: 0=No, 1=Yes
  uint16_t SF : 1;        // [13]   Seek Fail: 0=Success, 1=No station found
  uint16_t STC : 1;       // [14]   Seek/Tune Complete: 1=Done
  uint16_t RDSR : 1;      // [15]   RDS Ready: 1=New RDS group ready
};
static_assert(sizeof(RDA5807M_REG0A) == 2, "RDA5807M_REG0A must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG0A bits;
} RDA5807M_REG0A_U;

// ========================================================
// Register 0x0B: RSSI, FM Status, RDS Errors
// ========================================================
struct __attribute__((packed)) RDA5807M_REG0B {
  uint16_t BLERB : 2;    // [1:0]  RDS Block B Error Level:
                         //        00=0 errors, 01=1-2, 10=3-5, 11=6+ or uncorrectable
  uint16_t BLERA : 2;    // [3:2]  RDS Block A Error Level (same encoding)
  uint16_t ABCD_E : 1;   // [4]    1=Blocks C/D are E (RBDS), 0=are A/B/C/D
  uint16_t FM_READY : 1; // [7]    FM Ready: 1=Ready
  uint16_t FM_TRUE : 1;  // [8]    FM True: 1=Valid station detected
  uint16_t RSSI : 7;     // [15:9] RSSI (0=min, 127=max), logarithmic scale
};
static_assert(sizeof(RDA5807M_REG0B) == 2, "RDA5807M_REG0B must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG0B bits;
} RDA5807M_REG0B_U;

// ========================================================
// Register 0x0C: RDS Block A or E
// ========================================================
struct __attribute__((packed)) RDA5807M_REG0C {
  uint16_t RDSA : 16; // [15:0] RDS Block A (or E in RBDS mode if ABCD_E=1)
};
static_assert(sizeof(RDA5807M_REG0C) == 2, "RDA5807M_REG0C must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG0C bits;
} RDA5807M_REG0C_U;

// ========================================================
// Register 0x0D: RDS Block B or E
// ========================================================
struct __attribute__((packed)) RDA5807M_REG0D {
  uint16_t RDSB : 16; // [15:0] RDS Block B (or E)
};
static_assert(sizeof(RDA5807M_REG0D) == 2, "RDA5807M_REG0D must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG0D bits;
} RDA5807M_REG0D_U;

// ========================================================
// Register 0x0E: RDS Block C or E
// ========================================================
struct __attribute__((packed)) RDA5807M_REG0E {
  uint16_t RDSC : 16; // [15:0] RDS Block C (or E)
};
static_assert(sizeof(RDA5807M_REG0E) == 2, "RDA5807M_REG0E must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG0E bits;
} RDA5807M_REG0E_U;

// ========================================================
// Register 0x0F: RDS Block D or E
// ========================================================
struct __attribute__((packed)) RDA5807M_REG0F {
  uint16_t RDSD : 16; // [15:0] RDS Block D (or E)
};
static_assert(sizeof(RDA5807M_REG0F) == 2, "RDA5807M_REG0F must be 16 bits");
typedef union __attribute__((packed)) {
  uint16_t value;
  RDA5807M_REG0F bits;
} RDA5807M_REG0F_U;

//==============================================================================
// Структура с описанием всех регистров RDA5807M
//------------------------------------------------------------------------------
typedef struct __attribute__((packed)) {
  RDA5807M_REG00_U reg00; // CHIP ID (Read-only) CHIPID = 0x58
  RDA5807M_REG01_U reg01; // Unused
  RDA5807M_REG02_U reg02; // Power, Seek & General Control
  RDA5807M_REG03_U reg03; // Channel & Band Settings
  RDA5807M_REG04_U reg04; // Audio & AFC Settings
  RDA5807M_REG05_U reg05; // Volume, Seek Threshold
  RDA5807M_REG06_U reg06; // Reserved (Open Mode)
  RDA5807M_REG07_U reg07; // Seek & Blend Settings
  RDA5807M_REG08_U reg08; // Frequency direct mode
  RDA5807M_REG09_U reg09; // Unused
  RDA5807M_REG0A_U reg0A; // Status & RDS Ready
  RDA5807M_REG0B_U reg0B; // RSSI, FM Status, RDS Errors
  RDA5807M_REG0C_U reg0C; // RDS Block A or E
  RDA5807M_REG0D_U reg0D; // RDS Block B or E
  RDA5807M_REG0E_U reg0E; // RDS Block C or E
  RDA5807M_REG0F_U reg0F; // RDS Block D or E
} RDA_REG_NAMED;

//==============================================================================
// Union для доступа к регистрам по номеру или по именам.
//------------------------------------------------------------------------------
typedef union __attribute__((packed)) {
  uint16_t regs[16];
  RDA_REG_NAMED named;
} RDA_REGS_U;
