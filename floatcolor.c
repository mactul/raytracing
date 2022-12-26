#include "floatcolor.h"

void floatcolor_init(FloatColor* pdest, float red, float green, float blue)
{
    pdest->red = red;
    pdest->green = green;
    pdest->blue = blue;
}

void floatcolor_mult(FloatColor* pdest, FloatColor* pfc1, FloatColor* pfc2)
{
    pdest->red = pfc1->red * pfc2->red;
    pdest->green = pfc1->green * pfc2->green;
    pdest->blue = pfc1->blue * pfc2->blue;
}

void floatcolor_mult_scalar(FloatColor* pdest, FloatColor* pfcsrc, float scalar)
{
    pdest->red = pfcsrc->red * scalar;
    pdest->green = pfcsrc->green * scalar;
    pdest->blue = pfcsrc->blue * scalar;
}

void floatcolor_iadd(FloatColor* pdest, FloatColor* pfc)
{
    pdest->red += pfc->red;
    pdest->green += pfc->green;
    pdest->blue += pfc->blue;
}

void floatcolor_add(FloatColor* pdest, FloatColor* pfc1, FloatColor* pfc2)
{
    pdest->red = pfc1->red + pfc2->red;
    pdest->green = pfc1->green + pfc2->green;
    pdest->blue = pfc1->blue + pfc2->blue;
}