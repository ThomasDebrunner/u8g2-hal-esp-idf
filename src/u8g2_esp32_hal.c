#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "u8g2_esp32_hal.h"

static const char* TAG = "u8g2_hal";

#define HOST SPI2_HOST

/*
 * HAL callback function as prescribed by the U8G2 library.  This callback is
 * invoked to handle callbacks for GPIO and delay functions.
 */
uint8_t u8g2_esp32_gpio_and_delay_cb(u8x8_t* u8x8,
                                     uint8_t msg,
                                     uint8_t arg_int,
                                     void* arg_ptr) {

  switch (msg) {
      // Initialize the GPIO and DELAY HAL functions.
    case U8X8_MSG_GPIO_AND_DELAY_INIT: {
      uint64_t bitmask = 0;

      for (uint8_t i = 0; i < U8X8_PIN_CNT; i++) {
        if (u8x8->pins[i] != U8X8_PIN_NONE) {
          bitmask = bitmask | (1ull << u8x8->pins[i]);
        }
      }

      if (bitmask == 0) {
        break;
      }
      gpio_config_t gpioConfig;
      gpioConfig.pin_bit_mask = bitmask;
      gpioConfig.mode = GPIO_MODE_OUTPUT;
      gpioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
      gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
      gpioConfig.intr_type = GPIO_INTR_DISABLE;
      gpio_config(&gpioConfig);
      break;
    }

    // Delay for the number of milliseconds passed in through arg_int.
    case U8X8_MSG_DELAY_MILLI: {
      vTaskDelay(arg_int / portTICK_PERIOD_MS);
      break;
    }

    default: {
      if (msg >= U8X8_MSG_GPIO(0)) {
        uint8_t i = u8x8_GetPinValue(u8x8, msg);
        if (i != U8X8_PIN_NONE) {
          if (u8x8_GetPinIndex(u8x8, msg) < U8X8_PIN_OUTPUT_CNT) {
            gpio_set_level(i, arg_int);
          }
        }
      }
    }
  }
  return 0;
}  // u8g2_esp32_gpio_and_delay_cb
