from functools import cached_property
import numpy
import math

DELTA = 100  # size of the scene
N = 500      # resolution of the image


class Vector:
    def __init__(self, x: float, y: float, z: float):
        self.x = x
        self.y = y
        self.z = z
    def dot_product(self, v: 'Vector') -> float:
        return self.x * v.x + self.y * v.y + self.z * v.z
    
    @cached_property
    def square_norme(self) -> float:
        return self.x**2 + self.y**2 + self.z**2

    @cached_property
    def norme(self) -> float:
        return math.sqrt(self.square_norme)
    
    @cached_property
    def unit(self) -> 'Vector':
        return self / self.norme
    
    def __add__(self, v: 'Vector') -> 'Vector':
        return Vector(self.x + v.x, self.y + v.y, self.z + v.z)
    
    def __sub__(self, v: 'Vector') -> 'Vector':
        return Vector(self.x - v.x, self.y - v.y, self.z - v.z)
    
    def __iadd__(self, v: 'Vector') -> 'Vector':
        self.x += v.x
        self.y += v.y
        self.z += v.z
        return self
    
    def __isub__(self, v: 'Vector') -> 'Vector':
        self.x -= v.x
        self.y -= v.y
        self.z -= v.z
        return self

    def __neg__(self) -> 'Vector':
        return Vector(-self.x, -self.y, -self.z)

    def __truediv__(self, a: float) -> 'Vector':
        return Vector(self.x/a, self.y/a, self.z/a)
    
    def __mul__(self, a: float) -> 'Vector':
        return Vector(self.x*a, self.y*a, self.z*a)
    
    def __rmul__(self, a: float) -> 'Vector':
        return self * a
    
    def __len__(self) -> int:
        return 3
    
    def __eq__(self, v: 'Vector') -> bool:
        if not isinstance(v, Vector):
            return False
        return self.x == v.x and self.y == v.y and self.z == v.z
    
    def __ne__(self, v: 'Vector') -> bool:
        return not (self == v)

class FloatColor:
    def __init__(self, r: float, g: float, b: float):
        self.r = r
        self.g = g
        self.b = b
    
    def __add__(self, c: 'FloatColor') -> 'FloatColor':
        return FloatColor(self.r + c.r, self.g + c.g, self.b + c.b)
    
    def __iadd__(self, c: 'FloatColor') -> 'FloatColor':
        self.r += c.r
        self.g += c.g
        self.b += c.b
        return self
    
    def __mul__(self, c: 'FloatColor') -> 'FloatColor':
        if isinstance(c, FloatColor):
            return FloatColor(self.r * c.r, self.g * c.g, self.b * c.b)
        else:
            return FloatColor(self.r * c, self.g * c, self.b * c)
    
    def __rmul__(self, a: float) -> 'FloatColor':
        return self * a
    
    def to_tuple(self) -> tuple[float, float, float]:
        return (self.r, self.g, self.b)

class Physical_Object:
    def __init__(self, kd: FloatColor, kr: float):
        self.kd = kd
        self.kr = kr

class Sphere(Physical_Object):
    def __init__(self, center: Vector, radius: float, kd: FloatColor, kr: float):
        super().__init__(kd, kr)
        self.center = center
        self.radius = radius


class Light:
    def __init__(self, position: Vector, color: FloatColor):
        self.position = position
        self.color = color


class Ray:
    def __init__(self, start_position: Vector, direction: Vector):
        self.start_position = start_position
        self.direction = direction
    
    def get_point(self, t: float):
        assert t >= 0
        return self.start_position + t * self.direction


class Scene:
    def __init__(self, objects: list[Sphere], lights: list[Light]):
        self.objects = objects
        self.lights = lights


def vec(A: Vector, B: Vector) -> Vector:
    return B - A


def ra(A: Vector, B: Vector) -> Ray:
    return Ray(A, vec(A, B).unit)



def intersection(r: Ray, s: Sphere) -> tuple[Vector, float]:
    vCA = vec(s.center, r.start_position)
    b = 2*vCA.dot_product(r.direction)
    c = vCA.square_norme - s.radius**2

    delta = b**2 - 4*c

    if delta < 0:
        return None

    sqrt_delta = math.sqrt(delta)
    t1 = (-b - sqrt_delta) / 2
    t2 = (-b + sqrt_delta) / 2

    if t1 <= 0 and t2 <= 0:
        return None
    
    if t1 <= 0:
        t = t2
    elif t2 <= 0:
        t = t1
    else:
        t = min(t1, t2)
    
    return (r.get_point(t), t)


def interception(scene: Scene, r: Ray) -> tuple[Vector, int]:
    min_t = math.inf
    nearest_P = None
    shere_j = 0
    for j in range(len(scene.objects)):
        inters = intersection(r, scene.objects[j])
        if inters is not None:
            P, t = inters
            if t < min_t:
                min_t = t
                nearest_P = P
                shere_j = j
    if nearest_P is None:
        return None
    return (nearest_P, shere_j)


def over_horizon(s: Sphere, P: Vector, src: Vector) -> bool:
    v_normal = vec(s.center, P).unit
    vCSRC = vec(s.center, src)

    if v_normal.dot_product(vCSRC) < 0:
        return False
    return True


def light_is_visible(obj: list[Sphere], j: int, P: Vector, src: Vector) -> bool:
    if not over_horizon(obj[j], P, src):
        return False
    
    r = ra(src, P)
    n = vec(P, src).norme
    for i in range(len(obj)):
        if i == j:
            continue
        H = intersection(r, obj[i])
        if H is not None:
            if H[1] < n:
                return False
    return True


def scattered_color(r: Ray, Cs: FloatColor, N: Vector, kd: FloatColor) -> FloatColor:
    cos_theta = r.direction.dot_product(N)
    return (kd * Cs)*cos_theta


def computed_color(scene: Scene, P: Vector, j: int) -> FloatColor:
    Cd = FloatColor(0.0, 0.0, 0.0)
    for i in range(len(scene.lights)):
        if light_is_visible(scene.objects, j, P, scene.lights[i].position) and scene.objects[j].center != P:
            Cd += scattered_color(ra(P, scene.lights[i].position), scene.lights[i].color, vec(scene.objects[j].center, P).unit, scene.objects[j].kd)
    return Cd


def reflected_ray(s: Sphere, P: Vector, src: Vector) -> Ray:
    v_normal = vec(s.center, P).unit
    u = vec(src, P).unit
    w = u - 2*u.dot_product(v_normal)*v_normal

    return Ray(P, w)


def reflexions(scene: Scene, r: Ray, rmax: int, L=None, rnum=0) -> list[tuple[Vector, int]]:
    if L is None:
        L = []
    interc = interception(scene, r)
    if interc is not None:
        P, sphere_j = interc
        L.append(interc)
        if rnum < rmax and scene.objects[sphere_j].center != P and r.start_position != P:
            r = reflected_ray(scene.objects[sphere_j], P, r.start_position)
            L += reflexions(scene, r, rmax, L, rnum+1)
    return L


def final_color(scene: Scene, r: Ray, rmax: int, fond: FloatColor) -> FloatColor:
    pts = reflexions(scene, r, rmax)
    if len(pts) == 0:
        return fond
    C = computed_color(scene, pts[-1][0], pts[-1][1])
    for k in range(len(pts)-2, -1, -1):
        C = computed_color(scene, pts[k][0], pts[k][1]) + scene.objects[pts[k][1]].kr * C
    return C


def grid(y: int, x: int) -> Vector:
    return Vector(x - N/2, -y + N/2, 0) * DELTA/N


def screen_ray(camera: Vector, i: int, j: int) -> Ray:
    return ra(camera, grid(i, j))


def compute_img(scene, camera: Vector, fond: FloatColor, rmax: int):
    img = numpy.empty((N, N, 3))
    for i in range(N):
        for j in range(N):
            r = screen_ray(camera, i, j)
            img[i, j] = final_color(scene, r, rmax, fond).to_tuple()
            p = 100*(N*i+j)/N**2
            if int(p) == p:
                print(int(p), "%")

    return img

camera = Vector(0, 0, 100)
light_pos = Vector(-1000, -500, -500)
A = Vector(-150, 0, -500)
B = Vector(150, 0, -500)

S1 = Sphere(A, 100, FloatColor(0.9, 0, 0), 0.2)
S2 = Sphere(B, 150, FloatColor(0.5, 0.9, 0.2), 0.9)

L1 = Light(light_pos, FloatColor(1, 1, 1))

scene = Scene([S1, S2], [L1])

img = compute_img(scene, camera, FloatColor(0.7, 0.7, 0.7), 0)

import matplotlib.pyplot as plt

plt.imshow(img)

plt.show()