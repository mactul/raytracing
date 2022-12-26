#include <math.h>

typedef struct vector {
    float x;
    float y;
    float z;
    float norme;
    float square_norme;
    char _norme_computed;
} Vector;

void vector_init(Vector* pv, float x, float y, float z);
float vector_dot_product(Vector* pv1, Vector* pv2);
void vector_mult_scalar(Vector* pvdest, Vector* pvsrc, float scalar);
void vector_sub(Vector* pvdest, Vector* pv1, Vector* pv2);
void vector_add(Vector* pvdest, Vector* pv1, Vector* pv2);
float vector_norme(Vector* pv);
void vector_unit(Vector* pvdest, Vector* pvsrc);
char vector_isequal(Vector* pv1, Vector* pv2);