#include "raytracing.h"

#define RAN_F (float)(RAND_MAX/2 - rand())/(float)RAND_MAX

void ray_AB(Ray* prdest, Vector* A, Vector* B)
{
    prdest->start_position = *A;
    vector_sub(&(prdest->direction), B, A);
    vector_unit(&(prdest->direction), &(prdest->direction));
}

void ray_get_point(Vector* pvdest, Ray* prsrc, float t)
{
    vector_mult_scalar(pvdest, &(prsrc->direction), t);
    vector_add(pvdest, pvdest, &(prsrc->start_position));
}

float intersection(Vector* intersection_point, Ray* pr, Sphere* ps)
{
    Vector vCA;
    float b;
    float c;
    float delta;
    float t, t1, t2;

    vector_sub(&vCA, &(pr->start_position), ps->center);

    b = 2*vector_dot_product(&vCA, &(pr->direction));
    vector_norme(&vCA); // compute and cache the norme
    c = vCA.square_norme - ps->radius * ps->radius;

    delta = b*b -4*c;

    if(delta < 0)
    {
        return -1.0;
    }

    delta = sqrt(delta);

    t1 = (-b - delta) / 2;
    t2 = (-b + delta) / 2;

    if(t1 <= 0 || t2 <= 0)
    {
        return -1.0;
    }
    else
    {
        t = t1;
    }

    ray_get_point(intersection_point, pr, t);

    return t;
}

int interception(Vector* pintersection, Scene* pscene, Ray* pr)
{
    float min_t = -1.0;
    int sphere_j = -1;
    for(int j = 0; j < pscene->objects_number; j++)
    {
        Vector P;
        float t = intersection(&P, pr, pscene->objects[j]);
        if(t >= 0 && (min_t < 0 || t < min_t))
        {
            min_t = t;
            sphere_j = j;
            *pintersection = P;
        }
    }
    return sphere_j;
}

char over_horizon(Sphere* ps, Vector* pcontact, Vector* plightsrc)
{
    Vector vnormal;
    Vector vC_lightsrc;

    vector_sub(&vnormal, pcontact, ps->center);
    vector_unit(&vnormal, &vnormal);

    vector_sub(&vC_lightsrc, plightsrc, ps->center);

    if(vector_dot_product(&vnormal, &vC_lightsrc) < 0)
    {
        return 0;
    }
    return 1;
}

char light_is_visible(Scene* pscene, int object_num, Vector* pcontact, Vector* plightsrc)
{
    if(!over_horizon(pscene->objects[object_num], pcontact, plightsrc))
    {
        return 0;
    }

    Ray r;
    Vector v;
    float n;

    ray_AB(&r, plightsrc, pcontact);
    vector_sub(&v, plightsrc, pcontact);
    n = vector_norme(&v);
    for(int i = 0; i < pscene->objects_number; i++)
    {
        if(i != object_num)
        {
            float t = intersection(&v, &r, pscene->objects[i]);
            if(t >= 0 && t < n)
            {
                return 0;
            }
        }
    }
    return 1;
}

void scattered_color(FloatColor* pcolor, Ray* pr, FloatColor* plight_color, Vector* pvnormal, FloatColor* psphere_color)
{
    floatcolor_mult(pcolor, psphere_color, plight_color);
    floatcolor_mult_scalar(pcolor, pcolor, vector_dot_product(&(pr->direction), pvnormal));
}

void computed_color(FloatColor* pcolor, Scene* pscene, Vector* pcontact, int object_num)
{
    floatcolor_init(pcolor, 0.0, 0.0, 0.0);
    for(int i = 0; i < pscene->lights_number; i++)
    {
        if(!vector_isequal(pscene->objects[object_num]->center, pcontact) && light_is_visible(pscene, object_num, pcontact, pscene->lights[i]->position))
        {
            FloatColor fc;
            Ray r;
            Vector vnormal;

            // scattered_color renvoie une valeur négative ce qui est impossible
            // Parfois ça ne déclenche jamais ici, comme il n'y a pas d'aléatoire, c'est forcément qu'une variable est mal initialisée.
            // Reste à trouver laquelle

            ray_AB(&r, pcontact, pscene->lights[i]->position);
            vector_sub(&vnormal, pcontact, pscene->objects[object_num]->center);
            vector_unit(&vnormal, &vnormal);
            // WTF vnormal est nul parfois !!!
            // Il semble que c'est dans ce cas que scattered_color renvoie du négatif
            // en fait non...


            // J'ai trouvé la douille !
            // vector_isequal ne fonctionne pas parce qu'on est avec des flottants...
            // Il faut le changer par un calcul de norme comparé à un epsilon
            // bof... pas mieux...
            // je crois que le problème est sur la fonction light_is_visible

            scattered_color(&fc, &r, pscene->lights[i]->color, &vnormal, pscene->objects[object_num]->color);
            
            floatcolor_iadd(pcolor, &fc);
        }
    }
}

void reflected_ray(Ray* prdest, Sphere* psphere, Vector* pcontact, Vector* plightsrc)
{
    Vector vnormal;
    Vector u;

    vector_sub(&vnormal, pcontact, psphere->center);
    vector_unit(&vnormal, &vnormal);

    vector_sub(&u, pcontact, plightsrc);
    vector_unit(&u, &u);

    vector_mult_scalar(&(prdest->direction), &vnormal, 2*vector_dot_product(&u, &vnormal));
    vector_sub(&(prdest->direction), &u, &(prdest->direction));

    prdest->start_position = *pcontact;
}

int reflexions(ObjectPoint* pts_array, Scene* pscene, Ray* pr, int rmax, int rnum)
{
    int n = 0;
    int sphere_j = interception(&(pts_array[rnum].point), pscene, pr);
    pts_array[rnum].object_num = sphere_j;
    if(sphere_j >= 0)
    {
        n = 1;
        if(rnum < rmax && !vector_isequal(pscene->objects[sphere_j]->center, &(pts_array[rnum].point)) && !vector_isequal(&(pr->start_position), &(pts_array[rnum].point)))
        {
            reflected_ray(pr, pscene->objects[sphere_j], &(pts_array[rnum].point), &(pr->start_position));
            n += reflexions(pts_array, pscene, pr, rmax, rnum+1);
        }
    }

    return n;
}

void final_color(FloatColor* pcolor, Scene* pscene, Ray* pr, int rmax, FloatColor* background)
{
    ObjectPoint pts_array[rmax+1];

    int l = reflexions(pts_array, pscene, pr, rmax, 0);

    if(l == 0)
    {
        *pcolor = *background;
        return;
    }
    computed_color(pcolor, pscene, &(pts_array[l-1].point), pts_array[l-1].object_num);
    //printf("calcul\n");
    for(int k = l-2; k >= 0; k--)
    {
        FloatColor tempcolor;
        computed_color(&tempcolor, pscene, &(pts_array[k].point), pts_array[k].object_num);
        floatcolor_mult_scalar(pcolor, pcolor, pscene->objects[pts_array[k].object_num]->reflexion_rate);
        floatcolor_iadd(pcolor, &tempcolor);
    }
    
}

void grid(Vector* pvdest, int row, int col)
{
    vector_init(pvdest, col-N/2, -row+N/2, 0);
    vector_mult_scalar(pvdest, pvdest, (float)DELTA/(float)N);
}

void screen_ray(Ray* predest, Vector* camera, int row, int col)
{
    Vector v;

    grid(&v, row, col);
    ray_AB(predest, camera, &v);
}

void get_pixel(FloatColor* result, int row, int col, Scene* pscene, Vector* camera, FloatColor* background, int rmax)
{
    Ray r;
    screen_ray(&r, camera, row, col);
    final_color(result, pscene, &r, rmax, background);
}