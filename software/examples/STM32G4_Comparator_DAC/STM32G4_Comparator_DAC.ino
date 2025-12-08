/*
 * STM32G4 Comparator with Internal DAC Threshold Example
 *
 * This example demonstrates the use of the internal Analog Comparator (COMP1)
 * and Digital-to-Analog Converter (DAC1) on the STM32G431.
 *
 * Function:
 * - The DAC generates a fixed reference voltage (Threshold).
 * - The Comparator compares an external analog input against this threshold.
 * - The result is output directly to a GPIO pin via hardware (low latency).
 *
 * Pinout:
 * - Input (COMP1_INP): D2 (PA1)
 * - Output (COMP1_OUT): D9 (PA6)
 * - Threshold Source: Internal DAC1 Channel 1 (Configured to ~1.65V)
 *
 * Instructions:
 * 1. Connect a variable voltage (0V - 3.3V) to D2.
 * 2. Monitor D9 (PA6).
 *    - If D2 > 1.65V, D9 should be HIGH (3.3V).
 *    - If D2 < 1.65V, D9 should be LOW (0V).
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

// Handles
DAC_HandleTypeDef hdac1;
COMP_HandleTypeDef hcomp1;

void Example_Error_Handler() {
  Serial.println("Error encountered!");
  while(1) {
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("STM32G4 Comparator + DAC Example Starting...");

  // --- 1. Configure DAC1 (Reference Voltage) ---

  // Enable DAC Clock
  __HAL_RCC_DAC1_CLK_ENABLE();

  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK) Example_Error_Handler();

  DAC_ChannelConfTypeDef sConfig = {0};
  sConfig.DAC_HighFrequency = DAC_HIGH_FREQUENCY_INTERFACE_MODE_AUTOMATIC;
  sConfig.DAC_DMADoubleDataMode = DISABLE;
  sConfig.DAC_SignedFormat = DISABLE;
  sConfig.DAC_SampleAndHold = DAC_SAMPLEANDHOLD_DISABLE;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;

  // Important: We want to connect DAC to on-chip peripheral (COMP),
  // but usually DAC output buffer is disabled for internal connections to save power/interference,
  // unless we also want to measure it on PA4.
  // For this example, we keep the buffer off and rely on internal routing.
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  sConfig.DAC_ConnectOnChipPeripheral = DAC_CHIPCONNECT_ENABLE;
  sConfig.DAC_UserTrimming = DAC_TRIMMING_FACTORY;

  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK) Example_Error_Handler();

  // Start DAC
  if (HAL_DAC_Start(&hdac1, DAC_CHANNEL_1) != HAL_OK) Example_Error_Handler();

  // Set Value: 2048 (Mid-scale). With 3.3V VREF, this is ~1.65V.
  // 12-bit resolution: 0 to 4095.
  if (HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048) != HAL_OK) Example_Error_Handler();

  Serial.println("DAC Configured: ~1.65V Internal Reference");

  // --- 2. Configure COMP1 ---

  // Enable Clocks
  __HAL_RCC_COMP_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  // Configure GPIO for Input (PA1 / D2)
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure GPIO for Output (PA6 / D9)
  // PA6 AF8 is COMP1_OUT
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF8_COMP1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Initialize COMP1
  hcomp1.Instance = COMP1;
  // PA1 is IO1 for Input Plus.
  // (Check stm32g4xx_ll_comp.h or datasheet for specific mapping if not standard,
  // but usually IO1=PA1 for COMP1)
  hcomp1.Init.InputPlus = COMP_INPUT_PLUS_IO1;
  hcomp1.Init.InputMinus = COMP_INPUT_MINUS_DAC1_CH1; // Compare against DAC
  hcomp1.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED; // High when In+ > In-
  hcomp1.Init.Hysteresis = COMP_HYSTERESIS_NONE;
  hcomp1.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
  hcomp1.Init.TriggerMode = COMP_TRIGGERMODE_NONE; // No interrupt, just hardware output

  if (HAL_COMP_Init(&hcomp1) != HAL_OK) Example_Error_Handler();

  // Start Comparator
  if (HAL_COMP_Start(&hcomp1) != HAL_OK) Example_Error_Handler();

  Serial.println("Comparator Started.");
  Serial.println("  In+ : D2 (PA1)");
  Serial.println("  In- : Internal DAC (~1.65V)");
  Serial.println("  Out : D9 (PA6)");
}

void loop() {
  // Comparator works in background.
  // We can print something or just wait.
  delay(2000);
  Serial.println("System Running...");
}
