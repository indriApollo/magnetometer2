
#include "graphics.h"
#include "font09.h"

void rotateImage(float angle, uint16_t* buf1, uint16_t* buf2, uint16_t width, uint16_t height) {
	float midX, midY;
	float deltaX, deltaY;
	int16_t rotX, rotY;
	uint16_t x, y;

	// Optimization: compute the sin and cos only once
	float _sin = sin(angle);
	float _cos = cos(angle);

	midX = height / 2.0f; // 'f': single precision float
	midY = height / 2.0f;

	for(y = 0; y < width; y++) {
		for(x = 0; x < height; x++) {
			deltaX = y - midX;
			deltaY = x - midY;
			rotX = (int16_t)(midX + deltaX * _sin + deltaY * _cos);
			// optimization: leave the loop early when we already know we'll be out of bounds
			if(rotX < 0 || rotX >= width) continue;
			rotY = (int16_t)(midY + deltaX * _cos - deltaY * _sin);
			if(rotY >= 0 && rotY < height) {
				*(buf2+(x * width + y)) = *(buf1+(rotX * width + rotY));
			}
		}
	}
}

void angleText(int16_t angle, uint16_t* fontBuffer)
{
	uint8_t strbuf[4];
	snprintf((char*)strbuf, sizeof(strbuf), "%d", angle);

	uint16_t *fontStart;
	uint8_t strbufLen = strlen((char*)strbuf);
	uint16_t bgFillLen = (N_CHARS-strbufLen)*FONT_WIDTH/2; // compute the needed borders to center the chars
	uint16_t* fontBufferPtr = fontBuffer;

	for(uint16_t y = 0; y<FONT_HEIGHT ; y++) {
		for(uint16_t x = 0; x<FONTBUFFER_WIDTH ; x++) {
			fontBufferPtr[(FONTBUFFER_WIDTH*y)+x] = 0x0000;
			if(x==bgFillLen) x = ((FONTBUFFER_WIDTH)-bgFillLen-1); // jump to the right border
		}
	}

	for(uint8_t i = 0; i<strbufLen; i++) {

		uint8_t index = strbuf[i] - '0';
		if(index > 9) index = 0; // should never happen

		fontStart = (uint16_t*)(font09 + index*FONT_SIZE);

		// skip border from center mode and start right of the previous character
		fontBufferPtr = fontBuffer+(bgFillLen+FONT_WIDTH*i);
		for(uint16_t y = 0; y<FONT_HEIGHT ; y++) {
			for(uint16_t x = 0; x<FONT_WIDTH ; x++) {
				*fontBufferPtr++ = *fontStart++;
			}
			fontBufferPtr+=(FONT_WIDTH*(N_CHARS-1)); // move buffer pointer to next row
		}
	}
}

void cpyRotated(uint16_t* src, uint16_t* dest, uint16_t srcWidth, uint16_t srcHeight)
{
	for(uint16_t y = 0; y<srcHeight; y++) {
		for(uint16_t x = 0; x<srcWidth; x++) {
			// 90 deg to right rotation
			dest[(srcHeight*(x+1))-y-1] = src[(srcWidth*y)+x];
		}
	}
}

void clearBuffer(uint16_t* buf, uint32_t bufSize, uint16_t clearVal)
{
	while(bufSize-- > 0) {
		*buf++ = clearVal;
	}
}

void cpyToFb(const uint16_t* buf, uint16_t bufWidth, uint16_t bufHeight, uint16_t* fb, uint16_t fbWidth, uint16_t posx, uint16_t posy)
{
	fb += (fbWidth*posy)+posx;
	uint16_t offset = fbWidth-bufWidth; // bufWidth better be < than fbWidth

	for(uint16_t y = 0; y<bufHeight; y++) {
		for(uint16_t x = 0; x<bufWidth; x++) {
			*fb++ = *buf++;
		}
		fb+=offset;
	}
}
