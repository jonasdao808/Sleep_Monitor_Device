#include <Arduino.h>
#include <driver/i2s.h>
#include "mic.h"
#include "system_state.h"

#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000
#define DECIMATE 16
#define WINDOW_SAMPLES 128
#define SNORE_THRESHOLD 130

#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

static int32_t raw_samples[SAMPLE_BUFFER_SIZE];

void MicTask(void *pvParameters) {
  i2s_config_t config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false
  };

  i2s_pin_config_t pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA
  };

  i2s_driver_install(I2S_NUM_0, &config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pins);

  int32_t dc = 0;

  while (true) {
    size_t bytesRead;
    i2s_read(I2S_NUM_0, raw_samples, sizeof(raw_samples), &bytesRead, portMAX_DELAY);

    float sumSq = 0;
    int count = 0;

    for (int i = 0; i < SAMPLE_BUFFER_SIZE && count < WINDOW_SAMPLES; i += DECIMATE) {
      int32_t s = raw_samples[i] >> 16;
      dc = (dc * 63 + s) >> 6;
      s -= dc;
      sumSq += s * s;
      count++;
    }

    float rms = sqrt(sumSq / count);

    xSemaphoreTake(sysMutex, portMAX_DELAY);
    sys.rms = rms;
    sys.snoreDetected = rms > SNORE_THRESHOLD;
    xSemaphoreGive(sysMutex);

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}
