/*
 * STM32G4 CORDIC Polar Conversion (Modulus/Phase) Example
 *
 * This example demonstrates using the CORDIC hardware accelerator to convert
 * Cartesian coordinates (x, y) to Polar coordinates (r, theta).
 * This operation is equivalent to calculating `sqrt(x^2 + y^2)` and `atan2(y, x)`.
 *
 * Hardware:
 * - STM32G431 CORDIC Co-processor
 *
 * Use Case:
 * - Field Oriented Control (FOC) for motors (Clarke/Park transforms).
 * - Signal Magnitude calculations.
 *
 * Function:
 * - Generates a rotating vector (X, Y) representing a phasor.
 * - Uses CORDIC to compute Magnitude (Modulus) and Angle (Phase).
 * - Verifies the Magnitude remains constant (Radius) and Angle ramps linearly.
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

CORDIC_HandleTypeDef hcordic;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("STM32G4 CORDIC Polar Example");

  // --- 1. Enable CORDIC Clock ---
  __HAL_RCC_CORDIC_CLK_ENABLE();

  // --- 2. Initialize CORDIC ---
  hcordic.Instance = CORDIC;
  if (HAL_CORDIC_Init(&hcordic) != HAL_OK) {
     Serial.println("Error: CORDIC Init Failed");
     while(1);
  }

  // --- 3. Configure CORDIC for Modulus (Phase/Mag) ---
  CORDIC_ConfigTypeDef sConfig = {0};
  sConfig.Function = CORDIC_FUNCTION_MODULUS; // Calculates Modulus (Mag) and Phase (Arg)
  sConfig.Precision = CORDIC_PRECISION_6CYCLES;
  sConfig.Scale = CORDIC_SCALE_0;
  sConfig.NbWrite = CORDIC_NBWRITE_2; // Two 32-bit inputs (X, Y)
  sConfig.NbRead = CORDIC_NBREAD_2;   // Two 32-bit outputs (Modulus, Phase)
  sConfig.InSize = CORDIC_INSIZE_32BITS;
  sConfig.OutSize = CORDIC_OUTSIZE_32BITS;

  if (HAL_CORDIC_Configure(&hcordic, &sConfig) != HAL_OK) {
    Serial.println("Error: CORDIC Config Failed");
    while(1);
  }

  Serial.println("CORDIC Configured for Modulus.");
  Serial.println("Generating rotating vector (Radius=0.8)...");
  Serial.println("Format: Input_X, Input_Y, Calc_Modulus, Calc_Phase");
}

void loop() {
  static float angle = 0;

  // 1. Generate Input Vector (Cartesian)
  // Radius = 0.5 (Must be kept low because CORDIC Modulus adds gain ~1.647)
  // With Radius 0.8: 0.8 * 1.647 = ~1.31 which overflows q1.31 range [-1, 1)
  // With Radius 0.5: 0.5 * 1.647 = ~0.82 which is safe.
  float radius = 0.5;
  float x_float = radius * cos(angle);
  float y_float = radius * sin(angle);

  // Convert to q1.31
  // 2^31 = 2147483648.0
  int32_t x_q31 = (int32_t)(x_float * 2147483648.0f);
  int32_t y_q31 = (int32_t)(y_float * 2147483648.0f);

  int32_t input_buffer[2];
  input_buffer[0] = x_q31; // Primary Argument (X)
  input_buffer[1] = y_q31; // Secondary Argument (Y)

  int32_t output_buffer[2]; // [0]=Modulus, [1]=Phase

  // 2. Perform Calculation
  // We send X and Y.
  if (HAL_CORDIC_Calculate(&hcordic, input_buffer, output_buffer, 1, 10) == HAL_OK) {

     // 3. Convert Results back to Float
     float mod_float = (float)output_buffer[0] / 2147483648.0f;
     float phase_float = (float)output_buffer[1] / 2147483648.0f;

     // Phase output is normalized [-1, 1] mapped to [-PI, PI]
     float phase_rad = phase_float * PI;

     // 4. Print
     // For CSV plotting:
     Serial.print(x_float, 4); Serial.print(",");
     Serial.print(y_float, 4); Serial.print(",");
     Serial.print(mod_float, 4); Serial.print(",");
     Serial.println(phase_rad, 4);
  }

  // Update Angle
  angle += 0.1;
  if (angle > 2*PI) angle -= 2*PI;

  delay(50);
}
