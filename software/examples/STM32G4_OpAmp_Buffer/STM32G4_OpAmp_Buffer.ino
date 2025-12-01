/*
 * STM32G4 OpAmp Buffer Example for XIAO STM32G431
 *
 * This example configures the internal OPAMP1 in Follower (Unity Gain Buffer) mode.
 *
 * Hardware:
 * - XIAO STM32G431 Board
 * - STM32G431CBU6 MCU
 *
 * Pinout:
 * - Input (VINP): D2 (PA1)
 * - Output (VOUT): D3 (PA2)
 *
 * Function:
 * The signal applied to D2 is buffered and output to D3.
 * This is useful for high-impedance analog sensors connecting to the ADC
 * (though D3 is also an ADC pin, buffering helps drive the ADC sampling cap or other loads).
 *
 * Instructions:
 * 1. Connect a signal source (e.g., potentiometer or function generator) to D2.
 * 2. Monitor the output on D3 with an oscilloscope or multimeter.
 * 3. The voltage on D3 should mirror D2.
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

// Define OpAmp handle
OPAMP_HandleTypeDef hopamp1;

void setup() {
  // Initialize Serial for debug
  Serial.begin(115200);
  // Wait for serial port to connect (optional, for native USB)
  // while (!Serial) delay(10);
  delay(1000);
  Serial.println("STM32G4 OpAmp Buffer Example Starting...");

  // 1. Enable Clocks
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_OPAMP_CLK_ENABLE();

  // 2. Configure GPIOs
  // PA1 (D2) as Analog Input (OPAMP1_VINP)
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // PA2 (D3) as Analog Output (OPAMP1_VOUT)
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // 3. Configure OPAMP1
  hopamp1.Instance = OPAMP1;
  hopamp1.Init.PowerMode = OPAMP_POWERMODE_NORMAL;
  hopamp1.Init.Mode = OPAMP_FOLLOWER_MODE;                // Buffer mode
  hopamp1.Init.NonInvertingInput = OPAMP_NONINVERTINGINPUT_IO0; // PA1 is VINP0 for OPAMP1

  // Default settings for other parameters
  hopamp1.Init.InternalOutput = DISABLE;
  hopamp1.Init.TimerControlledMuxmode = OPAMP_TIMERCONTROLLEDMUXMODE_DISABLE;
  hopamp1.Init.UserTrimming = OPAMP_TRIMMING_FACTORY;

  if (HAL_OPAMP_Init(&hopamp1) != HAL_OK) {
    Serial.println("Error: OPAMP Init Failed");
    while (1);
  }

  // 4. Start OPAMP
  if (HAL_OPAMP_Start(&hopamp1) != HAL_OK) {
    Serial.println("Error: OPAMP Start Failed");
    while (1);
  }

  Serial.println("OPAMP1 Initialized successfully.");
  Serial.println("Configuration:");
  Serial.println("  Input: D2 (PA1)");
  Serial.println("  Output: D3 (PA2)");
  Serial.println("  Mode: Follower (Buffer)");
}

void loop() {
  // OpAmp runs in hardware. We can blink an LED to show the system is alive.
  static uint32_t last_time = 0;
  if (millis() - last_time > 1000) {
    last_time = millis();
    // Assuming LED_BUILTIN is defined for the board variant
    #ifdef LED_BUILTIN
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    #endif
    Serial.println("OpAmp active...");
  }
}
