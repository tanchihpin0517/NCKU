from PyQt5 import QtWidgets
from PyQt5.QtWidgets import *
from PyQt5.QtCore import *
from PyQt5.QtGui import *
from main import DEFAULT_LINE_THINKNESS, DEFAULT_LINE_COLOR
import cv2
import math

BORDER_TYPE = cv2.BORDER_REPLICATE
#BORDER_TYPE = cv2.BORDER_REFLECT_101

def cvImgToQtImg(cvImg):
    height, width, channel = cvImg.shape
    bytesPerLine = 3 * width
    qImg = QImage(cvImg.data, width, height, bytesPerLine, QImage.Format_RGB888).rgbSwapped()
    return qImg

def imgWithCtrlLine(image, lines, index=None):
    temp = image.copy()
    line_thickness = DEFAULT_LINE_THINKNESS
    color = DEFAULT_LINE_COLOR

    if index is None:
        for line in lines:
            cv2.line(temp, line.f.data(), line.t.data(), color, line_thickness)
    else:
        for i in index:
            line = lines[i]
            cv2.line(temp, line.f.data(), line.t.data(), color, line_thickness)

    return temp

def sample_from_raw_image(x, y, raw_image):
    height = raw_image.shape[0]
    width = raw_image.shape[1]

    x0 = cv2.borderInterpolate(int(x), width, BORDER_TYPE)
    x1 = cv2.borderInterpolate(int(x+1), width, BORDER_TYPE)
    y0 = cv2.borderInterpolate(int(y), height, BORDER_TYPE)
    y1 = cv2.borderInterpolate(int(y+1), height, BORDER_TYPE)

    t_x = x - int(x)
    t_y = y - int(y)

    pixel = ((raw_image[y0][x0] * (1 - t_x) + raw_image[y0][x1] * t_x) * (1 - t_y) +
             (raw_image[y1][x0] * (1 - t_x) + raw_image[y1][x1] * t_x) * t_y).round().astype(raw_image.dtype)

    return pixel
