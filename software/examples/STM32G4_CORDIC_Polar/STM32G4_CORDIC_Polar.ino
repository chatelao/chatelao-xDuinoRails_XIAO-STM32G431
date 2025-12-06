/*
 * STM32G4 CORDIC Polar (Rectangular to Polar) Example
 *
 * This example demonstrates using the CORDIC hardware accelerator on the STM32G431
 * to convert Rectangular coordinates (X, Y) to Polar coordinates (Modulus, Phase).
 *
 * Hardware:
 * - STM32G431 CORDIC Co-processor
 *
 * Function:
 * - Calculates Modulus = sqrt(X^2 + Y^2) and Phase = atan2(Y, X).
 *
 * Notes:
 * - CORDIC works with fixed-point q1.31 format.
 * - The Modulus operation has an intrinsic gain of ~1.647.
 * - Input vectors (X, Y) must be scaled down (e.g. Magnitude < 0.607) to prevent
 *   overflow of the output Modulus in the q1.31 format.
 *
 * Input:
 * - X = 0.5, Y = 0.5 (Magnitude = ~0.707).
 *   NOTE: 0.707 * 1.647 > 1.0, which would OVERFLOW.
 *   So we will use smaller inputs: X=0.3, Y=0.3. Magnitude ~0.424.
 *   0.424 * 1.647 = ~0.698 (Safe).
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

CORDIC_HandleTypeDef hcordic;

void Error_Handler() {
  Serial.println("Error encountered!");
  while(1) delay(100);
}

// Float to q1.31 conversion
int32_t FloatToQ31(float x) {
  // Clamp to range [-1, 1)
  if (x >= 1.0f) return 0x7FFFFFFF;
  if (x <= -1.0f) return 0x80000000;
  return (int32_t)(x * 2147483648.0f);
}

// q1.31 to Float conversion
float Q31ToFloat(int32_t x) {
  return (float)x / 2147483648.0f;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("STM32G4 CORDIC Polar Example");

  // --- 1. Enable CORDIC Clock ---
  __HAL_RCC_CORDIC_CLK_ENABLE();

  // --- 2. Initialize CORDIC ---
  hcordic.Instance = CORDIC;
  if (HAL_CORDIC_Init(&hcordic) != HAL_OK) Error_Handler();

  // --- 3. Configure CORDIC for Modulus (Polar) ---
  CORDIC_ConfigTypeDef sConfig = {0};
  sConfig.Function = CORDIC_FUNCTION_MODULUS; // Converts (X, Y) -> (Mod, Phase)
  sConfig.Precision = CORDIC_PRECISION_6CYCLES;
  sConfig.Scale = CORDIC_SCALE_0; // No extra shifting
  sConfig.NbWrite = CORDIC_NBWRITE_2; // Two 32-bit inputs (X, Y)
  sConfig.NbRead = CORDIC_NBREAD_2;   // Two 32-bit outputs (Modulus, Phase)
  sConfig.InSize = CORDIC_INSIZE_32BITS;
  sConfig.OutSize = CORDIC_OUTSIZE_32BITS;

  if (HAL_CORDIC_Configure(&hcordic, &sConfig) != HAL_OK) Error_Handler();

  Serial.println("CORDIC Configured for Modulus/Phase.");

  // --- 4. Prepare Input ---
  // We use X=0.3, Y=0.4. Expected Modulus = 0.5. Phase = atan2(0.4, 0.3) approx 53.13 deg.
  // IMPORTANT:
  // CORDIC Modulus Gain K approx 1.647.
  // The hardware output for Modulus will be (TrueModulus * K).
  // 0.5 * 1.647 = 0.8235. This fits in q1.31 range [-1, 1].

  float input_x_f = 0.3f;
  float input_y_f = 0.4f;

  // Note: For CORDIC_FUNCTION_MODULUS, data is packed.
  // But HAL_CORDIC_Calculate handles arrays.
  // Input Array: [X, Y] (Order depends on packing, but typically X then Y for Modulus)
  // Actually, Reference Manual says for Modulus:
  // ARG1 = X, ARG2 = Y.

  int32_t input_q31[2];
  input_q31[0] = FloatToQ31(input_x_f);
  input_q31[1] = FloatToQ31(input_y_f);

  int32_t output_q31[2]; // [Modulus, Phase]

  Serial.print("Input: X="); Serial.print(input_x_f);
  Serial.print(", Y="); Serial.println(input_y_f);

  // --- 5. Perform Calculation ---
  uint32_t start = micros();
  // 1 calculation set
  if (HAL_CORDIC_Calculate(&hcordic, input_q31, output_q31, 1, 100) != HAL_OK) {
     Serial.println("Error: CORDIC Calculation Failed");
  }
  uint32_t end = micros();

  // --- 6. Process Results ---
  // Output 0: Modulus * K
  // Output 1: Phase (Normalized / PI)

  float mod_raw = Q31ToFloat(output_q31[0]);
  float phase_raw = Q31ToFloat(output_q31[1]);

  // Apply inverse gain to get true modulus
  // K = 1.646760258121
  float K = 1.646760258121f;
  float mod_true = mod_raw / K;

  // Phase is normalized [-1, 1] mapped to [-PI, PI]
  float phase_rad = phase_raw * PI;
  float phase_deg = phase_rad * 180.0f / PI;

  Serial.println("\n--- Results ---");
  Serial.print("Raw Modulus (Scaled): "); Serial.println(mod_raw, 6);
  Serial.print("True Modulus (Corrected): "); Serial.println(mod_true, 6);
  Serial.print("Expected Modulus: 0.500000"); Serial.println();

  Serial.print("Phase (Rad): "); Serial.println(phase_rad, 6);
  Serial.print("Phase (Deg): "); Serial.println(phase_deg, 6);
  Serial.print("Expected Phase (Deg): 53.13"); Serial.println();

  Serial.print("Time: "); Serial.print(end - start); Serial.println(" us");
}

void loop() {
  delay(1000);
}
