import numpy as np
import cv2
import utils
from geometry import Vector, Line

class Image:
    def __init__(self, img_file, ctrl_line_file):
        self.img_file = img_file
        self.ctrl_line_file = ctrl_line_file
        self.raw_data = cv2.imread(img_file)
        self.ctrl_lines = []
        self.ctrl_lines_edit = []

        with open(ctrl_line_file) as f:
            for line in f:
                line = line.strip()
                s_x, s_y, d_x, d_y = tuple(map(int, line.split(' ')))
                l = Line((s_x, s_y), (d_x, d_y))
                self.ctrl_lines.append(l)

    def data(self):
        return self.raw_data.copy()

    def data_(self):
        return self.raw_data

    def size(self):
        return (self.raw_data.shape[0], self.raw_data.shape[1])

    def save(self):
        with open(self.ctrl_line_file, 'w') as f:
            for line in self.ctrl_lines:
                s_x, s_y = line.f.data()
                d_x, d_y = line.t.data()
                f.write(f'{s_x} {s_y} {d_x} {d_y}\n')
