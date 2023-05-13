#include "vector.h"

#define EPSILON 1e-13

void vector_init(Vector* pv, float x, float y, float z)
{
    pv->x = x;
    pv->y = y;
    pv->z = z;
    pv->_norme_computed = 0;
}

float vector_dot_product(Vector* pv1, Vector* pv2)
{
    return pv1->x * pv2->x + pv1->y * pv2->y + pv1->z * pv2->z;
}

void vector_mult_scalar(Vector* pvdest, Vector* pvsrc, float scalar)
{
    pvdest->x = pvsrc->x * scalar;
    pvdest->y = pvsrc->y * scalar;
    pvdest->z = pvsrc->z * scalar;
    pvdest->_norme_computed = 0;
}

void vector_sub(Vector* pvdest, Vector* pv1, Vector* pv2)
{
    pvdest->x = pv1->x - pv2->x;
    pvdest->y = pv1->y - pv2->y;
    pvdest->z = pv1->z - pv2->z;
    pvdest->_norme_computed = 0;
}

void vector_add(Vector* pvdest, Vector* pv1, Vector* pv2)
{
    pvdest->x = pv1->x + pv2->x;
    pvdest->y = pv1->y + pv2->y;
    pvdest->z = pv1->z + pv2->z;
    pvdest->_norme_computed = 0;
}

float vector_norme(Vector* pv)
{
    if(!pv->_norme_computed)
    {
        pv->_norme_computed = 1;
        pv->square_norme = pv->x * pv->x + pv->y * pv->y + pv->z * pv->z;
        pv->norme = sqrt(pv->square_norme);
    }
    return pv->norme;
}

void vector_unit(Vector* pvdest, Vector* pvsrc)
{
    vector_mult_scalar(pvdest, pvsrc, 1.0 / vector_norme(pvsrc));
    pvdest->_norme_computed = 1;
    pvdest->square_norme = 1.0;
    pvdest->norme = 1.0;
}

char vector_isequal(Vector* pv1, Vector* pv2)
{
    Vector v;
    vector_sub(&v, pv1, pv2);
    return vector_dot_product(&v, &v) < EPSILON;
}