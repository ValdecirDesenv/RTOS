#include <freertos/portmacro.h>
#include <pins_arduino.h>
#include <esp32-hal-gpio.h>
#include <HardwareSerial.h>

// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif
// String to print
const char   msg[] = "Vdc news FreeRTOS running";
// Pins
static const int led_pin = LED_BUILTIN;

//  Task handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

// Our task 0: blink an LED
void toggleLED(void *parameter) {
  while(1) {
    digitalWrite(led_pin, HIGH);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    digitalWrite(led_pin, LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// Our task 1: print msg to serial with low priority
void startTask1(void *parameter) {
  int msg_len = strlen(msg);
  while(1) {
    Serial.println();
    for (int i=0; i<=msg_len; i++){
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Our task 2: print msg to serial with higher priority
void startTask2(void *parameter) {
  int msg_len = strlen(msg);
  while(1) {
    Serial.print('*');
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {

  // Configure pin
  pinMode(led_pin, OUTPUT);
  Serial.begin(300);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println(' ---- FreeRTOS Task Demo ---');

  // Print self priority
  Serial.println("Setup and look task running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));


//   // Task to run forever
//   xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
//               toggleLED,    // Function to be called
//               "Toggle LED", // Name of task
//               1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
//               NULL,         // Parameter to pass to function
//               1,            // Task priority (0 to configMAX_PRIORITIES - 1)
//               NULL,         // Task handle
//               app_cpu);     // Run on one core for demo purposes (ESP32 only)

//   // If this was vanilla FreeRTOS, you'd want to call vTaskStartScheduler() in
//   // main after setting up your tasks.

  // Task to run forever
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              startTask1,    // Function to be called
              "Task 1", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,         // Parameter to pass to function
              1,            // Task priority (0 to configMAX_PRIORITIES - 1)
              &task_1,         // Task handle
              app_cpu);     // Run on one core for demo purposes (ESP32 only)

  // Task to run forever
  xTaskCreatePinnedToCore(  // Use xTaskCreate() in vanilla FreeRTOS
              startTask2,    // Function to be called
              "Task 1", // Name of task
              1024,         // Stack size (bytes in ESP32, words in FreeRTOS)
              NULL,         // Parameter to pass to function
              2,            // Task priority (0 to configMAX_PRIORITIES - 1)
              &task_2,         // Task handle
              app_cpu);     // Run on one core for demo purposes (ESP32 only)

}

void loop() {
  // Suspend the higher priority task for some intervals
  for (int i = 0; i < 3; i++){
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  // Delete the lower priority task
  if (task_1 != NULL){
    vTaskDelete(task_1);
    task_1 = NULL;
  }
}