#include "vector.h"
#include "floatcolor.h"
#define MAX_NUMBER_OBJECTS 20
#define MAX_NUMBER_LIGHTS 10

#define DELTA 100
#define N 1000


typedef struct sphere {
    Vector* center;
    float radius;
    FloatColor* color;
    float reflexion_rate;
    float roughness;
} Sphere;

typedef struct light {
    Vector* position;
    FloatColor* color;
} Light;

typedef struct ray {
    Vector start_position;
    Vector direction;
} Ray;

typedef struct object_point {
    Vector point;
    int object_num;
} ObjectPoint;

typedef struct scene {
    Sphere* objects[MAX_NUMBER_OBJECTS];
    Light* lights[MAX_NUMBER_LIGHTS];
    int objects_number;
    int lights_number;
} Scene;

void get_pixel(FloatColor* result, int row, int col, Scene* pscene, Vector* camera, FloatColor* background, int rmax);