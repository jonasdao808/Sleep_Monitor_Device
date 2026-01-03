#include "mic.h"
#include <Arduino.h>
#include <driver/i2s.h>

#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000
#define DECIMATE 16
#define WINDOW_SAMPLES 128
#define PLOT_LIMIT 20000

#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

static int32_t raw_samples[SAMPLE_BUFFER_SIZE];

static i2s_config_t i2s_config = {
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

static i2s_pin_config_t i2s_pins = {
  .bck_io_num = I2S_MIC_SERIAL_CLOCK,
  .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_MIC_SERIAL_DATA
};

void Mic_Init() {
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &i2s_pins);
}

float Mic_ProcessAndGetRMS() {
  size_t bytes_read;
  i2s_read(I2S_NUM_0, raw_samples, sizeof(raw_samples), &bytes_read, portMAX_DELAY);

  static int32_t dc = 0;
  float sum_sq = 0;
  int plotted = 0;

  Serial.println(PLOT_LIMIT);
  Serial.println(-PLOT_LIMIT);

  for (int i = 0; i < SAMPLE_BUFFER_SIZE && plotted < WINDOW_SAMPLES; i += DECIMATE) {
    int32_t s = raw_samples[i] >> 16;
    dc = (dc * 63 + s) >> 6;
    s -= dc;
    s *= 50;

    if (s > PLOT_LIMIT) s = PLOT_LIMIT;
    if (s < -PLOT_LIMIT) s = -PLOT_LIMIT;

    Serial.println(s);
    sum_sq += (float)s * s;
    plotted++;
  }

  return sqrt(sum_sq / plotted);
}
