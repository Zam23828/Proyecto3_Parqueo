#ifndef INC__NEOPIXEL_H
#define INC__NEOPIXEL_H

#include "main.h"
#include "math.h"

#define numPixels 8

#define CCR_0 34
#define CCR_1 67

extern TIM_HandleTypeDef htim1;

#define neoPixel_timer htim1
#define neoPixel_canal TIM_CHANNEL_1

#define GAMMA_CORRECTION 2.2f
#define GAMMA 2.2f
#define MAX_BRIGHTNESS 255

void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);
void setBrightness(uint8_t b);
void pixelShow(void);
void pixelClear(void);

uint8_t Gamma_correccion(uint8_t color, float brillo_);

#endif /* __MAIN_H */
