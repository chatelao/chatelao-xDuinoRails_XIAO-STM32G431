# XIAO STM32G431 Pinout Proposal

This proposal aims to maximize compatibility with the Seeed XIAO ecosystem (SAMD21/RP2040) while leveraging the advanced analog features of the STM32G431KBU6 (UFQFPN32).

## Standard XIAO Pins (D0-D10)

| Pin | XIAO Standard Function | STM32G431 Pin | Pin Name | Alternate Functions (Selected) | Notes |
|:---:|:---:|:---:|:---:|:---:|:---|
| **D0** | A0, **DAC** | 10 | **PA4** | **DAC1_OUT1**, ADC2_IN17, COMP1_INM | Changed from PA0 to support DAC. |
| **D1** | A1 | 6 | **PA0** | ADC12_IN1, COMP1_INM, TIM2_CH1 | OPAMP1_VINP. |
| **D2** | A2 | 7 | **PA1** | ADC12_IN2, OPAMP1_VINP, OPAMP3_VINP, TIM2_CH2 | |
| **D3** | A3 | 8 | **PA2** | ADC12_IN3, **OPAMP1_VOUT**, COMP2_INM, TIM2_CH3 | |
| **D4** | A4, SDA | 30 | **PB7** | **I2C1_SDA**, USART1_RX, COMP3_INP | Hardware I2C1 SDA. |
| **D5** | A5, SCL | 29 | **PB6** | **I2C1_SCL**, USART1_TX, COMP4_OUT | Hardware I2C1 SCL. |
| **D6** | TX | 19 | **PA9** | **USART1_TX**, I2C2_SCL, DAC1_EXTI9 | Hardware USART1 TX. |
| **D7** | RX | 20 | **PA10** | **USART1_RX**, I2C2_SDA | Hardware USART1 RX. |
| **D8** | SCK | 11 | **PA5** | **SPI1_SCK**, DAC1_OUT2, ADC2_IN13, **OPAMP2_VINM** | Hardware SPI1 SCK. |
| **D9** | MISO | 12 | **PA6** | **SPI1_MISO**, ADC2_IN3, **OPAMP2_VOUT**, COMP1_OUT | Hardware SPI1 MISO. |
| **D10**| MOSI | 13 | **PA7** | **SPI1_MOSI**, ADC2_IN4, **OPAMP2_VINP**, COMP2_INP | Hardware SPI1 MOSI. |

## On-board NeoPixel

To save pins and match modern XIAO boards (RP2040, ESP32C3), a single NeoPixel is used instead of discrete RGB LEDs.

| Component | STM32 Pin | Function | Notes |
|:---:|:---:|:---:|:---|
| **NeoPixel Data** | **PB0** (Pin 14) | **TIM3_CH3** | PWM/DMA capable for WS2812B control. Also OPAMP2_VINP. |
| **NeoPixel Power**| 3V3 | Power | Driven directly from 3.3V rail. |

*Note: This frees up PB1 for user applications.*

## Xiao Plus Extra Pads (Bottom)

These pads expose additional GPIOs and advanced features.

| Pad | STM32 Pin | Functions | Advanced Analog / Timer |
|:---:|:---:|:---:|:---|
| **P0** | **PA3** | **LPUART1_RX**, ADC1_IN4 | **OPAMP1_VINM**, COMP2_INP |
| **P1** | **PB3** | **SPI3_SCK**, USART2_TX, TIM2_CH2 | |
| **P2** | **PB4** | **SPI3_MISO**, USART2_RX, TIM3_CH1 | |
| **P3** | **PB5** | **SPI3_MOSI**, I2C3_SDA, TIM3_CH2 | |
| **P4** | **PA15** | **SPI1_NSS**, I2C1_SCL, TIM2_CH1 | |
| **P5** | **PA8** | I2C3_SCL, TIM1_CH1 | High current timer output. |
| **P6** | **PB1** | **LPUART1_DE**, TIM3_CH4 | **OPAMP3_VOUT**, COMP4_OUT, ADC1_IN12. |
| **P7** | **PB8** | **BOOT0**, I2C1_SCL, TIM16_CH1 | Shared with BOOT0 button. |

## Advanced Analog Functions (COMP / OPAMP)

The STM32G431 is designed for mixed-signal applications. This pinout exposes the following internal analog peripherals:

### Operational Amplifiers (OPAMP)
*   **OPAMP1**:
    *   **VINP (+) :** Exposed on **D1 (PA0)**, **D2 (PA1)**.
    *   **VINM (-) :** Exposed on **P0 (PA3)**.
    *   **VOUT     :** Exposed on **D3 (PA2)**.
    *   *Full standalone OPAMP available on D1/D2/D3/P0.*
*   **OPAMP2**:
    *   **VINP (+) :** Exposed on **D10 (PA7)**, **NeoPixel (PB0)**.
    *   **VINM (-) :** Exposed on **D8 (PA5)**.
    *   **VOUT     :** Exposed on **D9 (PA6)**.
    *   *Full standalone OPAMP available on D8/D9/D10.*
*   **OPAMP3**:
    *   **VINP (+) :** Exposed on **D2 (PA1)**, **NeoPixel (PB0)**.
    *   **VINM (-) :** Not exposed (PB2 unavailable). *Restricted to Follower/PGA modes.*
    *   **VOUT     :** Exposed on **P6 (PB1)**.

### Comparators (COMP)
*   **COMP1**:
    *   **INP (+) :** **D2 (PA1)**, **P6 (PB1)**.
    *   **INM (-) :** **D0 (PA4)**, **D1 (PA0)**.
    *   **OUT     :** **D1 (PA0)**, **D9 (PA6)**.
*   **COMP2**:
    *   **INP (+) :** **D10 (PA7)**, **P0 (PA3)**.
    *   **INM (-) :** **D3 (PA2)**, **D0 (PA4)**.
    *   **OUT     :** **D3 (PA2)**, **D10 (PA7)**.
*   **COMP3**:
    *   **INP (+) :** **D4 (PB7)**.
*   **COMP4**:
    *   **INP (+) :** **NeoPixel (PB0)**, **P6 (PB1)**.
    *   **INM (-) :** **P1 (PB3)**.
    *   **OUT     :** **D5 (PB6)**, **P6 (PB1)**.

*Note: Availability depends on the specific alternate function mapping and configuration.*

## Summary of Changes vs Original Netlist

1.  **D0**: Changed to **PA4** (was PA0) for **DAC** output.
2.  **D1**: Changed to **PA0** (was PA1).
3.  **D2**: Changed to **PA1** (was PA2).
4.  **D3**: Changed to **PA2** (was PA3).
5.  **NeoPixel**: Added on **PB0** (TIM3_CH3) replacing discrete RGB LEDs.
6.  **P6**: Assigned to **PB1** (was PF1) to expose **OPAMP3_VOUT**.
7.  **P7**: Confirmed as **PB8** (BOOT0).
8.  **USB/SWD**: Standard assignments preserved.
