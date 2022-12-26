typedef struct floatcolor {
    float red;
    float green;
    float blue;
} FloatColor;

void floatcolor_init(FloatColor* pdest, float red, float green, float blue);
void floatcolor_mult(FloatColor* pdest, FloatColor* pfc1, FloatColor* pfc2);
void floatcolor_mult_scalar(FloatColor* pdest, FloatColor* pfcsrc, float scalar);
void floatcolor_iadd(FloatColor* pdest, FloatColor* pfc);
void floatcolor_add(FloatColor* pdest, FloatColor* pfc1, FloatColor* pfc2);