# Partlist for XIAO STM32G431

This document lists the components used in the XIAO STM32G431 hardware design, based on the netlist and standard XIAO features.

| Designator | Value | Description | Footprint | Qty | Suggested LCSC Part | Notes |
| --- | --- | --- | --- | --- | --- | --- |
| U1 | STM32G431KBU6 | ARM Cortex-M4 MCU, 128KB Flash, 32KB RAM | Package_DFN_QFN:UFQFPN-32_5x5mm_P0.5mm | 1 | STM32G431KBU6 | Internal 128KB Flash (No ext flash needed for basic use) |
| U2 | XC6206P332MR | 3.3V LDO Regulator | Package_TO_SOT_SMD:SOT-23 | 1 | XC6206P332MR | Or similar 3.3V LDO (e.g. ME6211) |
| J1 | USB_C_Receptacle | USB Type-C Receptacle (16-pin) | Connector_USB:USB_C_Receptacle_HRO_TYPE-C-31-M-12 | 1 | C165948 | |
| SW1, SW2 | RESET, BOOT0 | Tactile Switch SPST | Button_Switch_SMD:SW_SPST_TL3342 | 2 | C318884 | |
| D1 | LED_PWR | LED, Red/Green (Power) | LED_SMD:LED_0603_1608Metric | 1 | C72041 | |
| D2 | LED_USR | LED, User (Pin 13/PC13) | LED_SMD:LED_0603_1608Metric | 1 | C72043 | Optional if RGB is primary user LED |
| D3 | RGB_LED | LED, RGB, Common Anode | LED_SMD:LED_0606 | 1 | C96555 | Connected to PB0, PB1, PB2 (Missing in netlist, to be added) |
| C1, C2 | 10uF | Capacitor, MLCC, 0603 | Capacitor_SMD:C_0603_1608Metric | 2 | C19702 | |
| C3 | 4.7uF | Capacitor, MLCC, 0603 | Capacitor_SMD:C_0603_1608Metric | 1 | C19666 | |
| C4-C8 | 100nF | Capacitor, MLCC, 0402 | Capacitor_SMD:C_0402_1005Metric | 5 | C1525 | |
| R1, R2 | 5.1k | Resistor, 0402 | Resistor_SMD:R_0402_1005Metric | 2 | C25900 | USB CC pull-downs |
| R3, R4 | 10k | Resistor, 0402 | Resistor_SMD:R_0402_1005Metric | 2 | C25744 | Pull-ups/downs |
| R5, R6 | 1k | Resistor, 0402 | Resistor_SMD:R_0402_1005Metric | 2 | C11702 | LED limiting |
| R7, R8 | 22 | Resistor, 0402 | Resistor_SMD:R_0402_1005Metric | 2 | C25092 | USB Series |
| J2, J3 | CONN_01X07 | Pin Header, 1x7, 2.54mm | PinHeader_2.54mm:PinHeader_1x07_P2.54mm_Vertical | 2 | - | |

## Design Choices & Comparison

This design aims to be compatible with the **Seeed Studio XIAO** form factor and feature set, specifically aligning with the modern XIAO RP2040 where possible.

### Key Features Comparison

| Feature | XIAO SAMD21 | XIAO RP2040 | XIAO STM32G431 (This Design) | Notes |
| --- | --- | --- | --- | --- |
| **MCU** | ATSAMD21G18 (48MHz, M0+) | RP2040 (133MHz, Dual M0+) | STM32G431KBU6 (170MHz, M4F) | Higher performance, FPU |
| **Flash** | 256KB Internal | 2MB External QSPI | 128KB Internal | STM32G4 internal flash is faster but smaller. |
| **USB** | USB 2.0 (Crystal often used) | USB 1.1 (Host/Device) | USB 2.0 FS (Device) | STM32G4 supports Crystal-less USB w/ CRS |
| **LEDs** | PWR, L (User), TX, RX | PWR, RGB (User) | PWR, User (PC13), RGB (PB0-2) | Added RGB for compatibility |
| **Buttons** | Reset Pads | Reset & Boot Buttons | Reset & Boot Buttons | Matching RP2040 convenience |
| **Crystal** | 32.768kHz | 12MHz | None (Internal HSI48) | Saves space/cost. HSI48+CRS is sufficient for USB. |

### Component Notes
- **RGB LED**: The XIAO RP2040 popularised the onboard RGB LED. We include one on pins PB0, PB1, PB2.
- **Crystal**: Omitted. The STM32G431 has an accurate internal HSI48 clock with Clock Recovery System (CRS) that synchronizes to the USB SOF frame, eliminating the need for an external crystal for USB operation.
- **Flash**: We rely on the 128KB internal flash. While smaller than RP2040's 2MB, it is sufficient for many embedded C/C++ applications. External QSPI flash could be added if space permits, but is not in the base partlist.
