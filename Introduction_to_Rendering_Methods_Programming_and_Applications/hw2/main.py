import sys
import numpy as np
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import *
from PyQt5 import QtCore
from PyQt5.QtCore import *
from PyQt5.QtGui import *

import ui
import signal
import utils
from image import Image
from geometry import Vector, Line
import cv2
import argparse
import os
import algo
import time

META_FILE = 'meta.txt'
MODE_NORMAL = 0
MODE_EDIT = 1
MODE_VIEW = 2
MODE = ['normal', 'edit', 'view']
DEFAULT_LINE_THINKNESS = 2
DEFAULT_LINE_COLOR = (0,0,255)

class Main(QMainWindow, ui.Ui_MainWindow):
    def __init__(self, args):
        super().__init__()
        self.setupUi(self)

        self.tags = []
        self.images = {}
        self.mode = 0
        self.morphing_t = 0.5 # variable T
        self.morphing_a = 1e-4
        self.morphing_b = 1.0
        self.morphing_p = 1.0
        self.morphing_interpolation = algo.INTERPOLATION_ENDPOINT
        self.image_views = [self.src_view, self.dst_view]
        self.view_edit_order = []
        self.anima_dir = args.anima_dir

        self.loadImages(args)

        for tag, image in self.images.items():
            height, width = image.size()
        #self.verticalLayoutWidget.resize((width+1)*(len(self.images)+1), 500)
        self.verticalLayoutWidget.resize(800, 500)
        self.loadViewImage(self.src_view, args.image_tags[0])
        self.loadViewImage(self.dst_view, args.image_tags[1])

        #self.mousePressEvent = mousePressEventTest
        self.src_view.mousePressEvent = self.mousePressEventSrcImage
        self.src_view.mouseReleaseEvent = self.mouseReleaseEventSrcImage
        self.src_view.mouseMoveEvent = self.mouseMoveEventSrcImage

        self.dst_view.mousePressEvent = self.mousePressEventDstImage
        self.dst_view.mouseReleaseEvent = self.mouseReleaseEventDstImage
        self.dst_view.mouseMoveEvent = self.mouseMoveEventDstImage

        self.mode_label.setText(MODE[self.mode])
        self.mode_button.clicked.connect(self.modeButtonClicked)

        self.clear_button.clicked.connect(self.clearButtonClicked)
        self.undo_button.clicked.connect(self.undoButtonClicked)
        self.save_button.clicked.connect(self.saveButtonClicked)

        self.show_all_button.clicked.connect(self.showAllButtonClicked)
        self.show_corresponding_button.clicked.connect(self.showCorrespondingButtonClicked)
        self.previous_button.clicked.connect(self.previousButtonClicked)
        self.next_button.clicked.connect(self.nextButtonClicked)

        self.morphing_button.clicked.connect(self.morphingButtonClicked)
        self.morphing_label_t.setText(f'T: {self.morphing_t}')
        self.morphing_slider_t.valueChanged.connect(self.morphingSliderTValueChanged)

        self.morphing_label_a.setText(f'a: %.4f' % self.morphing_a)
        self.morphing_slider_a.valueChanged.connect(self.morphingSliderAValueChanged)

        self.morphing_label_b.setText(f'b: {self.morphing_b}')
        self.morphing_slider_b.valueChanged.connect(self.morphingSliderBValueChanged)

        self.morphing_label_p.setText(f'p: {self.morphing_p}')
        self.morphing_slider_p.valueChanged.connect(self.morphingSliderPValueChanged)

    def test(self):
        l1 = Line((102, 124), (164,122))
        l2 = Line((102, 124), (164,122))
        v = Vector((97,171))
        s,_,_ = algo.transform_with_one_pair(v, l1, l2)
        print(v, s)

    def updateMorphingView(self):
        try:
            out, src, dst = algo.morphing(self.src_view.image, self.dst_view.image, self.morphing_t,
                            self.morphing_a, self.morphing_b, self.morphing_p, self.morphing_interpolation)
        except AssertionError as e:
            print(e)
            return

        self.setViewImage(self.morphing_view, out)
        self.setViewImage(self.src_view, src)
        self.setViewImage(self.dst_view, dst)
        print('morphing!')

    def morphingButtonClicked(self):
        self.updateMorphingView()

    def morphingSliderTValueChanged(self, value):
        self.morphing_t = value / 100
        self.morphing_label_t.setText(f'T: {self.morphing_t}')

    def morphingSliderAValueChanged(self, value):
        self.morphing_a = 1e-4 + value *1e-4
        self.morphing_label_a.setText(f'a: %.4f' % self.morphing_a)

    def morphingSliderBValueChanged(self, value):
        self.morphing_b = value * 2 / 100
        self.morphing_label_b.setText(f'b: {self.morphing_b}')

    def morphingSliderPValueChanged(self, value):
        self.morphing_p = value / 100
        self.morphing_label_p.setText(f'p: {self.morphing_p}')

    def loadImages(self, args):
        data_map = {}
        with open(os.path.join(args.data_dir, META_FILE)) as f:
            for line in f:
                line = line.strip()
                (tag, img_file, cl_file) = tuple(line.split(' '))
                img_file = os.path.join(args.data_dir, img_file)
                cl_file = os.path.join(args.data_dir, cl_file)
                img = Image(img_file, cl_file)
                self.images[tag] = img
                self.tags.append(tag)

    def loadViewImage(self, label, tag):
        img = self.images[tag]
        #label.setPixmap(QPixmap(utils.cvImgToQtImg(img.data_with_ctrl_lines())))
        label.image = img
        label.image_tag = tag
        self.refreshViewImage(label)
        #print(img.raw_data.shape)

    def refreshViewImage(self, label, cl_index=None, edit=False):
        img = label.image
        if edit:
            temp = utils.imgWithCtrlLine(img.data(), img.ctrl_lines_edit, index=cl_index)
        else:
            temp = utils.imgWithCtrlLine(img.data(), img.ctrl_lines, index=cl_index)
        #label.setPixmap(QPixmap(utils.cvImgToQtImg(temp)))
        #label.resize(img.size()[1], img.size()[0])
        self.setViewImage(label, temp)

    def setViewImage(self, label, raw_image):
        height = raw_image.shape[0]
        width = raw_image.shape[1]
        label.setPixmap(QPixmap(utils.cvImgToQtImg(raw_image)))
        label.resize(width, height)

    def keyPressEvent(self, e):
        if e.key() == Qt.Key_Q:
            self.close()

    def mousePressEventSrcImage(self, event):
        if event.buttons() & QtCore.Qt.LeftButton:
            self.mousePressEventImage(self.src_view, event)

    def mouseMoveEventSrcImage(self, event):
        self.mouseMoveEventImage(self.src_view, event)

    def mouseReleaseEventSrcImage(self, event):
        self.mouseReleaseEventImage(self.src_view, event)

    def mousePressEventDstImage(self, event):
        if event.buttons() & QtCore.Qt.LeftButton:
            self.mousePressEventImage(self.dst_view, event)

    def mouseMoveEventDstImage(self, event):
        self.mouseMoveEventImage(self.dst_view, event)

    def mouseReleaseEventDstImage(self, event):
        self.mouseReleaseEventImage(self.dst_view, event)

    def mousePressEventImage(self, label, event):
        event_pos = (event.pos().x(), event.pos().y())
        label.last_mouse_press_pos = event_pos

    def mouseMoveEventImage(self, label, event):
        event_pos = (event.pos().x(), event.pos().y())

        if self.mode == MODE_EDIT:
            start_pos = label.last_mouse_press_pos
            end_pos = event_pos
            image = label.image.data()
            ct_lines = label.image.ctrl_lines_edit[:]
            ct_lines.append(Line(start_pos, end_pos))
            temp = utils.imgWithCtrlLine(image, ct_lines)
            label.setPixmap(QPixmap(utils.cvImgToQtImg(temp)))

    def mouseReleaseEventImage(self, label, event):
        event_pos = (event.pos().x(), event.pos().y())

        if self.mode == MODE_EDIT:
            line = Line(label.last_mouse_press_pos, event_pos)
            label.image.ctrl_lines_edit.append(line)
            self.view_edit_order.append(label)

        label.last_mouse_press_pos = None

    def modeButtonClicked(self):
        self.mode = (self.mode + 1) % len(MODE)
        self.mode_label.setText(MODE[self.mode])

        if self.mode == MODE_NORMAL:
            self.leaveViewMode()
            self.enterNormalMode()
        elif self.mode == MODE_EDIT:
            self.leaveNormalMode()
            self.enterEditMode()
        elif self.mode == MODE_VIEW:
            self.leaveEditMode()
            self.enterViewMode()
        else:
            raise NotImplementedError(f'Mode has not been implemented: {self.mode}')

    def enterNormalMode(self):
        for image_view in self.image_views:
            self.refreshViewImage(image_view)

    def leaveNormalMode(self):
        pass

    def enterEditMode(self):
        for image_view in self.image_views:
            image = image_view.image
            image.ctrl_lines_edit = image.ctrl_lines[:]
            self.refreshViewImage(image_view, edit=True)
        self.view_edit_order = []

    def leaveEditMode(self):
        for image_view in self.image_views:
            self.refreshViewImage(image_view)

    def enterViewMode(self):
        for image_view in self.image_views:
            image_view.sel_line_index = 0
            self.refreshViewImage(image_view)

    def leaveViewMode(self):
        pass

    def clearButtonClicked(self):
        if self.mode == MODE_EDIT:
            for image_view in self.image_views:
                image_view.image.ctrl_lines_edit = []
                self.refreshViewImage(image_view, edit=True)
            self.view_edit_order = []

    def undoButtonClicked(self):
        if self.mode == MODE_EDIT:
            if len(self.view_edit_order) > 0:
                last_edit_view = self.view_edit_order.pop()
                last_edit_view.image.ctrl_lines_edit.pop()
                self.refreshViewImage(last_edit_view, edit=True)

    def saveButtonClicked(self):
        if self.mode == MODE_EDIT:
            for image_view in self.image_views:
                image = image_view.image
                image.ctrl_lines = image.ctrl_lines_edit[:]
                image.save()

    def showAllButtonClicked(self):
        if self.mode == MODE_VIEW:
            for image_view in self.image_views:
                self.refreshViewImage(image_view)

    def showCorrespondingButtonClicked(self):
        if self.mode == MODE_VIEW:
            for image_view in self.image_views:
                self.refreshViewImage(image_view, cl_index=[image_view.sel_line_index])

    def previousButtonClicked(self):
        if self.mode == MODE_VIEW:
            for image_view in self.image_views:
                cl_lines = image_view.image.ctrl_lines
                image_view.sel_line_index = (image_view.sel_line_index-1) % len(cl_lines)
                self.refreshViewImage(image_view, cl_index=[image_view.sel_line_index])

    def nextButtonClicked(self):
        if self.mode == MODE_VIEW:
            for image_view in self.image_views:
                cl_lines = image_view.image.ctrl_lines
                image_view.sel_line_index = (image_view.sel_line_index+1) % len(cl_lines)
                self.refreshViewImage(image_view, cl_index=[image_view.sel_line_index])

def main():
    argp = argparse.ArgumentParser()
    argp.add_argument('-d', dest='data_dir', type=str,
                      help='data directory')
    argp.add_argument('-a', dest='anima_dir', type=str,
                      help='animation directory')
    argp.add_argument('-i', dest='image_tags', nargs='+', default=[],
                      help='image tags')
    args = argp.parse_args()
    print('Data directory:', args.data_dir)
    print('Image tags:', args.image_tags)
    assert len(args.image_tags) >= 2, "More than 2 image tags are required"

    app = QtWidgets.QApplication(sys.argv)
    window = Main(args)
    window.show()
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()
