from copy import deepcopy
import numpy as np

class Vector():
    def __init__(self, v):
        if type(v) is tuple:
            self.x, self.y = v
        elif type(v) is Vector:
            (self.x, self.y) = v.data()
        else:
            raise TypeError(f"Unknow type: {type(v)}")

    def data(self):
        return (self.x, self.y)

    def copy(self):
        return deepcopy(self)

    def __add__(self, v):
        return Vector((self.x+v.x, self.y+v.y))

    def __sub__(self, v):
        return Vector((self.x-v.x, self.y-v.y))

    def __mul__(self, c):
        return Vector((self.x*c, self.y*c))

    def __floordiv__(self, c):
        return Vector((self.x//c, self.y//c))

    def __truediv__(self, c):
        return Vector((self.x/c, self.y/c))

    def __repr__(self):
        return f"Vector({self.x},{self.y})"

    def dot(self, v):
        return self.x*v.x + self.y*v.y

    def perpendicular(self):
        return Vector((self.y, -self.x))

    def round(self):
        return Vector((round(self.x), round(self.y)))

class Line():
    def __init__(self, f, t, thinkness=1):
        self.f = Vector(f)
        self.t = Vector(t)

    def __repr__(self):
        return f"Line({self.f},{self.t})"
