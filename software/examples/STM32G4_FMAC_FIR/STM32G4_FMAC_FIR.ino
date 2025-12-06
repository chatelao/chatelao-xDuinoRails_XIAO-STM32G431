/*
 * STM32G4 FMAC FIR Filter Example
 *
 * This example demonstrates the use of the Filter Math Accelerator (FMAC)
 * on the STM32G431 to perform a Finite Impulse Response (FIR) filter operation.
 *
 * Hardware:
 * - STM32G431 FMAC Unit
 *
 * Function:
 * - Configures FMAC as a 7-tap Low Pass FIR filter.
 * - Generates a noisy input signal (Fundamental Sine + High Freq Noise).
 * - Feeds the signal to the FMAC hardware using Polling mode.
 * - Reads the filtered output.
 * - Prints Input vs Output for plotting.
 *
 * Data Format:
 * - Inputs (X) and Coefficients (B) are in q1.15 fixed-point format.
 * - Output (Y) is in q1.15 format.
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

// --- Configuration ---
// Filter: 7-tap Low Pass
// Coefficients in q1.15 format (Float * 32768)
const int16_t FIR_COEFFS[] = {
  1310,  // 0.04
  3932,  // 0.12
  6554,  // 0.20
  9175,  // 0.28
  6554,  // 0.20
  3932,  // 0.12
  1310   // 0.04
};

#define NUM_TAPS (sizeof(FIR_COEFFS) / sizeof(FIR_COEFFS[0]))

FMAC_HandleTypeDef hfmac;

void Error_Handler() {
  Serial.println("FMAC Error!");
  while(1) delay(100);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("STM32G4 FMAC FIR Example");

  // --- 1. Enable Clocks ---
  __HAL_RCC_FMAC_CLK_ENABLE();

  // --- 2. Initialize FMAC ---
  hfmac.Instance = FMAC;
  if (HAL_FMAC_Init(&hfmac) != HAL_OK) Error_Handler();

  // --- 3. Configure Filter ---
  FMAC_FilterConfigTypeDef sFilterConfig = {0};

  // Memory Partitioning (Total 256 words)
  sFilterConfig.CoeffBaseAddress = 0;
  sFilterConfig.CoeffBufferSize = NUM_TAPS;

  sFilterConfig.InputBaseAddress = NUM_TAPS;
  sFilterConfig.InputBufferSize = 20; // Size of circular input buffer (X1) inside FMAC
  sFilterConfig.InputThreshold = FMAC_THRESHOLD_1;

  sFilterConfig.OutputBaseAddress = NUM_TAPS + 20;
  sFilterConfig.OutputBufferSize = 20; // Size of circular output buffer (Y) inside FMAC
  sFilterConfig.OutputThreshold = FMAC_THRESHOLD_1;

  // Coefficients
  sFilterConfig.pCoeffB = (int16_t*)FIR_COEFFS;
  sFilterConfig.CoeffBSize = NUM_TAPS;
  sFilterConfig.CoeffAccess = FMAC_BUFFER_ACCESS_NONE; // Loaded during Config

  sFilterConfig.Filter = FMAC_FUNC_CONVO_FIR;

  // Access Mode: Polling
  sFilterConfig.InputAccess = FMAC_BUFFER_ACCESS_POLLING;
  sFilterConfig.OutputAccess = FMAC_BUFFER_ACCESS_POLLING;

  sFilterConfig.Clip = FMAC_CLIP_ENABLED;
  sFilterConfig.P = NUM_TAPS;
  sFilterConfig.Q = 0;
  sFilterConfig.R = 0;

  if (HAL_FMAC_FilterConfig(&hfmac, &sFilterConfig) != HAL_OK) Error_Handler();

  // --- 4. Start FMAC ---
  // We start with NULL output buffer, it will be provided in the loop
  if (HAL_FMAC_FilterStart(&hfmac, NULL, NULL) != HAL_OK) Error_Handler();

  Serial.println("FMAC Configured. Running Loop...");
  Serial.println("Format: Input_Signal, Filtered_Output");
}

void loop() {
  static float phase = 0;
  static float noise_phase = 0;

  // 1. Generate Signal
  float signal = 10000.0f * sin(phase);
  float noise = 4000.0f * sin(noise_phase);
  int16_t input_val = (int16_t)(signal + noise);

  phase += 0.05f;
  noise_phase += 1.3f;
  if (phase > 2*PI) phase -= 2*PI;
  if (noise_phase > 2*PI) noise_phase -= 2*PI;

  // 2. Feed FMAC (Append Data)
  uint16_t input_size = 1; // Number of samples to write
  if (HAL_FMAC_AppendFilterData(&hfmac, &input_val, &input_size) != HAL_OK) {
     // Buffer might be full, or previous operation not done
     // For this simple example, we ignore or retry
  }

  // 3. Prepare to Read Output
  int16_t output_val = 0;
  uint16_t output_size = 1; // Number of samples to read

  // Provide buffer for the result
  if (HAL_FMAC_ConfigFilterOutputBuffer(&hfmac, &output_val, &output_size) == HAL_OK) {
      // 4. Poll for completion
      // This will move data from FMAC Y buffer to output_val
      if (HAL_FMAC_PollFilterData(&hfmac, 10) == HAL_OK) {
          // Check if data was actually read (output_size might be updated? or OutputCurrentSize)
          // In polling mode, Poll returns when buffer is filled or input handled.
          // Since we requested 1 sample, success usually means we got it.

          Serial.print(input_val);
          Serial.print(",");
          Serial.println(output_val);
      }
  }

  delay(10);
}
