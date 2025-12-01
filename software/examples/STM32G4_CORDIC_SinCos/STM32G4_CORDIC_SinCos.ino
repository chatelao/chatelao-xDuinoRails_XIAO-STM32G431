/*
 * STM32G4 CORDIC Sine/Cosine Example
 *
 * This example demonstrates using the CORDIC hardware accelerator on the STM32G431
 * to calculate trigonometric functions (Sine and Cosine) significantly faster
 * and more efficiently than software implementations.
 *
 * Hardware:
 * - STM32G431 CORDIC Co-processor
 *
 * Function:
 * - Calculates Sin(Angle) and Cos(Angle) for a fixed angle (PI/4).
 * - Measures and compares execution time against the standard math.h `sin()` and `cos()`.
 *
 * Notes:
 * - CORDIC works with fixed-point q1.31 format.
 * - Input angle is normalized: 1.0 represents PI, -1.0 represents -PI.
 */

#include <Arduino.h>

#if !defined(STM32G4)
  #error "This example is designed for STM32G4 series"
#endif

CORDIC_HandleTypeDef hcordic;

void setup() {
  Serial.begin(115200);
  // Wait for serial
  delay(1000);
  Serial.println("STM32G4 CORDIC Sine/Cosine Example");

  // --- 1. Enable CORDIC Clock ---
  __HAL_RCC_CORDIC_CLK_ENABLE();

  // --- 2. Initialize CORDIC ---
  hcordic.Instance = CORDIC;
  if (HAL_CORDIC_Init(&hcordic) != HAL_OK) {
     Serial.println("Error: CORDIC Init Failed");
     while(1);
  }

  // --- 3. Configure CORDIC for Sine/Cosine ---
  CORDIC_ConfigTypeDef sConfig = {0};
  sConfig.Function = CORDIC_FUNCTION_SINE; // This function generates BOTH Sin and Cos
  sConfig.Precision = CORDIC_PRECISION_6CYCLES; // 6 cycles per iteration (High precision)
  sConfig.Scale = CORDIC_SCALE_0; // Result scaling
  sConfig.NbWrite = CORDIC_NBWRITE_1; // One 32-bit input (Angle)
  sConfig.NbRead = CORDIC_NBREAD_2;   // Two 32-bit outputs (Sin, Cos)
  sConfig.InSize = CORDIC_INSIZE_32BITS;
  sConfig.OutSize = CORDIC_OUTSIZE_32BITS;

  if (HAL_CORDIC_Configure(&hcordic, &sConfig) != HAL_OK) {
    Serial.println("Error: CORDIC Config Failed");
    while(1);
  }

  Serial.println("CORDIC Configured.");
  Serial.println("Calculating Sin/Cos for 45 degrees (PI/4)...");

  // --- 4. Perform Calculation ---

  // Angle conversion:
  // CORDIC input range [-1, 1] corresponds to [-PI, PI].
  // 45 degrees = PI/4.
  // Normalized input = (PI/4) / PI = 0.25.
  // In q1.31 format: 0.25 * 2^31 = 0x20000000.

  int32_t input_angle_q31 = 0x20000000;
  int32_t output_results[2]; // Index 0: Sin, Index 1: Cos

  // Benchmark CORDIC
  uint32_t start_hw = micros();
  // Using HAL_CORDIC_Calculate (Polling mode)
  // Arguments: handle, input buffer, output buffer, number of calculations, timeout
  if (HAL_CORDIC_Calculate(&hcordic, &input_angle_q31, output_results, 1, 100) != HAL_OK) {
     Serial.println("Error: CORDIC Calculation Failed");
  }
  uint32_t end_hw = micros();

  // Convert results back to float for display
  // Divide by 2^31 (2147483648.0)
  float sin_cordic = (float)output_results[0] / 2147483648.0f;
  float cos_cordic = (float)output_results[1] / 2147483648.0f;

  // --- 5. Benchmark Software (math.h) ---
  float angle_rad = PI / 4.0;
  volatile float sin_sw, cos_sw; // volatile to prevent optimization

  uint32_t start_sw = micros();
  sin_sw = sin(angle_rad);
  cos_sw = cos(angle_rad);
  uint32_t end_sw = micros();

  // --- 6. Display Results ---
  Serial.println("\n--- Results ---");

  Serial.print("CORDIC Sin: "); Serial.println(sin_cordic, 6);
  Serial.print("CORDIC Cos: "); Serial.println(cos_cordic, 6);
  Serial.print("CORDIC Time (approx): "); Serial.print(end_hw - start_hw); Serial.println(" us (includes HAL overhead)");

  Serial.println("");

  Serial.print("Math.h Sin: "); Serial.println(sin_sw, 6);
  Serial.print("Math.h Cos: "); Serial.println(cos_sw, 6);
  Serial.print("Math.h Time: "); Serial.print(end_sw - start_sw); Serial.println(" us");

  Serial.println("\nNote: CORDIC hardware is much faster than indicated by micros().");
  Serial.println("The overhead of the HAL function call dominates this simple test.");
  Serial.println("For true high-performance, use LL drivers or DMA.");
}

void loop() {
  delay(1000);
}
