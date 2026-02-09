#include "LedHelper.h"


CRGB LEDHelper::rgb565ToCRGB(uint16_t color)
{
    uint8_t r, g, b;
    r = (color >> 11) & 0x1F;
    g = (color >> 5) & 0x3F;
    b = color & 0x1F;

    CRGB col = CRGB(r << 3, g << 2, b << 3); // Convert to 8-bit RGB
    return col;
}

uint16_t LEDHelper::CRGBtorgb565(CRGB col)
{
    return ((col.r & 0xF8) << 8) | ((col.g & 0xFC) << 3) | (col.b >> 3);
}

CRGB LEDHelper::DPT_Colour_RGB_to_CRGB(uint32_t ko)
{
    CRGB col = CRGB(0, 0, 0); //Set the color to black

    uint8_t* p = (uint8_t*)(void*)&ko; // KNX DPT_Colour_RGB arrives in BGR byte order on little-endian RP2040
    col.r = p[2]; //Get the red value from the ko
    col.g = p[1]; //Get the green value from the ko
    col.b = p[0]; //Get the blue value from the ko
    return col; //Return the color

}
