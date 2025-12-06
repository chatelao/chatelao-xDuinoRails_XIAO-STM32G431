/*
 * STM32G4 FMAC FIR Filter Example
 *
 * This example demonstrates using the FMAC (Filter Math Accelerator)
 * on the STM32G431 to perform a Finite Impulse Response (FIR) filter operation.
 *
 * Hardware:
 * - STM32G431 FMAC Co-processor
 *
 * Function:
 * - Configures FMAC for a 5-tap Moving Average FIR filter.
 * - Feeds input samples (Impulse).
 * - Reads filtered output.
 * - Uses Polling Mode (Blocking).
 *
 * Data Format:
 * - Fixed Point q1.15 (1 sign bit, 15 fractional bits).
 * - Range: [-1.0, 0.999969]
 * - 1.0 (float) is represented as 0x7FFF (max positive int16).
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

// Define FMAC handle
FMAC_HandleTypeDef hfmac;

// --- Filter Configuration ---
// Simple 5-tap Moving Average: Coefficients = [0.2, 0.2, 0.2, 0.2, 0.2]
// In q1.15: 0.2 * 32768 = 6553.6 -> 6554 (0x199A)
// Note: FMAC expects coefficients in int16_t array.
#define FILTER_TAP_NUM 5
const int16_t aCoeffB[FILTER_TAP_NUM] = {
  0x199A, 0x199A, 0x199A, 0x199A, 0x199A
};

// Buffers
// Input: Impulse (1, 0, 0, ...)
// Output: Response
#define DATA_SIZE 10
int16_t aInputData[DATA_SIZE];
int16_t aOutputData[DATA_SIZE];

void Error_Handler() {
  Serial.println("Error encountered!");
  while(1) delay(100);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for serial
  Serial.println("STM32G4 FMAC FIR Example");

  // --- 1. Enable FMAC Clock ---
  __HAL_RCC_FMAC_CLK_ENABLE();

  // --- 2. Initialize FMAC ---
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK) Error_Handler();

  // --- 3. Configure Filter ---
  FMAC_FilterConfigTypeDef sFmacConfig = {0};

  // Memory Map in FMAC RAM:
  // X1 (Coeffs): Start at 0, Size 5
  // X2 (Input): Start at 5, Size 10 (Circular buffer)
  // Y  (Output): Start at 15, Size 10

  sFmacConfig.CoeffBaseAddress = 0;
  sFmacConfig.CoeffBufferSize = FILTER_TAP_NUM;

  sFmacConfig.InputBaseAddress = FILTER_TAP_NUM;
  sFmacConfig.InputBufferSize = DATA_SIZE; // Circular buffer size
  sFmacConfig.InputThreshold = FMAC_THRESHOLD_1; // DMA/IRQ threshold (unused in polling but required)

  sFmacConfig.OutputBaseAddress = FILTER_TAP_NUM + DATA_SIZE;
  sFmacConfig.OutputBufferSize = DATA_SIZE;
  sFmacConfig.OutputThreshold = FMAC_THRESHOLD_1;

  sFmacConfig.pCoeffA = NULL; // No A coeffs for FIR
  sFmacConfig.CoeffASize = 0;

  sFmacConfig.pCoeffB = (int16_t*)aCoeffB;
  sFmacConfig.CoeffBSize = FILTER_TAP_NUM;

  sFmacConfig.Filter = FMAC_FUNC_CONVO_FIR; // Convolution (FIR)
  sFmacConfig.InputAccess = FMAC_BUFFER_ACCESS_NONE; // CPU writes
  sFmacConfig.OutputAccess = FMAC_BUFFER_ACCESS_NONE; // CPU reads

  // Scaling factors (P, Q, R) usually 0 for standard FIR unless needed
  sFmacConfig.Clip = FMAC_CLIP_DISABLED;
  sFmacConfig.P = 0;
  sFmacConfig.Q = 0;
  sFmacConfig.R = 0;

  if (HAL_FMAC_ConfigFilter(&hfmac, &sFmacConfig) != HAL_OK) Error_Handler();

  // --- 4. Start Filter ---
  if (HAL_FMAC_Start(&hfmac) != HAL_OK) Error_Handler();

  Serial.println("FMAC Configured and Started.");
  Serial.println("Processing Impulse Response...");

  // --- 5. Prepare Data ---
  // Impulse: 1.0 (0x7FFF) followed by zeros
  for (int i = 0; i < DATA_SIZE; i++) {
    aInputData[i] = (i == 0) ? 0x7FFF : 0;
    aOutputData[i] = 0;
  }

  // --- 6. Process Data (Polling) ---

  // Set the output buffer pointer for HAL
  if (HAL_FMAC_ConfigFilterOutputBuffer(&hfmac, aOutputData, DATA_SIZE) != HAL_OK) Error_Handler();

  // Push input data to FMAC
  uint32_t len = DATA_SIZE;
  // This copies data into FMAC internal memory (X2)
  if (HAL_FMAC_AppendFilterData(&hfmac, aInputData, &len) != HAL_OK) Error_Handler();

  // Poll until processing is complete
  // The HAL will read data from FMAC Y buffer into aOutputData
  if (HAL_FMAC_PollFilterData(&hfmac, 100) != HAL_OK) {
    Serial.println("Error: Polling Timeout");
  }

  // --- 7. Display Results ---
  Serial.println("Index\tInput\tOutput (Expected ~6554 for first 5 samples)");
  for(int i=0; i<DATA_SIZE; i++) {
    Serial.print(i);
    Serial.print("\t");
    Serial.print(aInputData[i]);
    Serial.print("\t");
    Serial.println(aOutputData[i]);
  }
}

void loop() {
  delay(1000);
}
