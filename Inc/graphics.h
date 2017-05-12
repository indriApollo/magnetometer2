
#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <math.h>
#include <stdint.h>

void rotateImage(float angle, uint16_t* buf1, uint16_t* buf2);

void angleText(int16_t angle, uint16_t* fontBuffer);

void cpyRotated(uint16_t* src, uint16_t* dest, uint16_t srcWidth, uint16_t srcHeight);

#endif /* GRAPHICS_H_ */
