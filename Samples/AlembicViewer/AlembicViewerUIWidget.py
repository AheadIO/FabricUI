"""Alembic Viewer UI Widget."""

import glob
import os
import sys

from PySide import QtCore, QtGui

from AlembicViewer.AlembicFileListWidget import AlembicFileListWidget


class AlembicViewerUIWidget(QtGui.QWidget):
    """AlembicViewerUIWidget

    This widget is the main widget that users are presented with in the Alembic
    Viewer sample application.

    Note:
        This widget incorporates the AlembicFileListWidget widget

    Attributes:
        dirChanged (QtCore.Signal): Emitted when the directory is changed in the
            AlembicFileListWidget.
        selItemChanged (QtCore.Signal): Emitted when the selected file from the
            AlembicFileListWidget is changed.
        sceneScaleChanged (QtCore.Signal): Emitted when the sceneScaleSlider
            widget is changed.
        fpsChanged (QtCore.Signal): Emitted when the user changes the
            fpsValueWidget

    Arguments:
        dirPath (str): The initial directory the AlembicFileListWidget widget
            will display files for.
        parent (QWidget): Parent widget item.

    """

    dirChanged = QtCore.Signal(str)
    selItemChanged = QtCore.Signal(str)
    sceneScaleChanged = QtCore.Signal(float)
    fpsChanged = QtCore.Signal(float)


    def __init__(self, dirPath, parent=None):
        super(AlembicViewerUIWidget, self).__init__(parent)

        self._currDir = dirPath
        self.createLayout()
        self.createConnections()


    def createLayout(self):
        """Setup the layout and add widgets."""

        # ==============
        # Setup Layouts
        # ==============
        self.mainLayout = QtGui.QVBoxLayout(self)
        self.mainLayout.setContentsMargins(10, 10, 10, 10)
        self.mainLayout.setSpacing(15)


        self.fpsLayout = QtGui.QHBoxLayout()
        self.fpsLayout.setContentsMargins(0, 0, 0, 0)

        self.sceneScaleLayout = QtGui.QHBoxLayout()
        self.sceneScaleLayout.setContentsMargins(0, 0, 0, 0)

        self.dirPathLayout = QtGui.QHBoxLayout()
        self.dirPathLayout.setContentsMargins(0, 0, 0, 0)

        # ===============
        # Create Widgets
        # ===============

        # FPS Widgets
        fpsValidator = QtGui.QIntValidator(self)
        fpsValidator.setRange(1, 60)
        self.fpsValueWidget = QtGui.QLineEdit(self)
        self.fpsValueWidget.setText(str(24.0))
        self.fpsValueWidget.setValidator(fpsValidator)
        self.fpsValueWidget.setSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)


        # Scene Scale Widgets
        self.sceneScaleSlider = QtGui.QSlider(QtCore.Qt.Horizontal, self)
        self.sceneScaleSlider.setObjectName('sceneScaleSlider')
        self.sceneScaleSlider.setMinimum(0.0)
        self.sceneScaleSlider.setMaximum(10 * 1000)
        self.sceneScaleSlider.setTickInterval(1000)
        self.sceneScaleSlider.setSingleStep(500)
        self.sceneScaleSlider.setPageStep(1000)
        self.sceneScaleSlider.setTickPosition(QtGui.QSlider.TicksBelow)
        self.sceneScaleSlider.setSizePolicy(QtGui.QSizePolicy.MinimumExpanding, QtGui.QSizePolicy.Fixed)
        self.sceneScaleSlider.setValue(1 * 1000)

        sceneScaleValidator = QtGui.QDoubleValidator(self)
        sceneScaleValidator.setRange(0.1, 10, 3)
        self.sceneScaleValueWidget = QtGui.QLineEdit(self)
        self.sceneScaleValueWidget.setText(str(1.0))
        self.sceneScaleValueWidget.setValidator(sceneScaleValidator)
        self.sceneScaleValueWidget.setSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)

        # Folder Select Widgets
        self.dirPathWidget = QtGui.QLineEdit(self)
        self.dirPathWidget.setText(self._currDir)
        self.dirPathWidget.setReadOnly(True)
        self.dirPathWidget.setCursorPosition(0)

        self.dirPathDialogWidget = QtGui.QPushButton(' ... ', self)
        self.dirPathDialogWidget.setMaximumWidth(75)

        # File List Widget
        self.fileListWidget = AlembicFileListWidget(self._currDir, self)


        # =======================
        # Add Widgets to Layouts
        # =======================
        self.fpsLayout.addWidget(self.fpsValueWidget)

        self.sceneScaleLayout.addWidget(self.sceneScaleSlider, 1)
        self.sceneScaleLayout.addWidget(self.sceneScaleValueWidget, 0)

        self.dirPathLayout.addWidget(self.dirPathWidget)
        self.dirPathLayout.addWidget(self.dirPathDialogWidget)

        self.fpsLabel = QtGui.QLabel('FPS Setting')
        self.mainLayout.addWidget(self.fpsLabel)
        self.mainLayout.addLayout(self.fpsLayout)

        self.sceneScaleLabel = QtGui.QLabel('Scene Scale')
        self.mainLayout.addWidget(self.sceneScaleLabel)
        self.mainLayout.addLayout(self.sceneScaleLayout)

        self.directoryLabel = QtGui.QLabel('Directory')
        self.mainLayout.addWidget(self.directoryLabel)
        self.mainLayout.addLayout(self.dirPathLayout)

        self.mainLayout.addWidget(self.fileListWidget)

        self.setLayout(self.mainLayout)

    def createConnections(self):
        """Sets connections between widgets and emits from the custom signals.

        """

        self.fpsValueWidget.editingFinished.connect(self.fpsValueChanged)

        self.sceneScaleSlider.valueChanged.connect(self.updateSceneScaleValueWidget)
        self.sceneScaleValueWidget.editingFinished.connect(self.updateSceneScaleSlider)

        self.dirPathDialogWidget.clicked.connect(self.openDir)
        self.dirPathWidget.textChanged.connect(self.fileListWidget.setDirpath)

        self.fileListWidget.currentItemChanged.connect(self.selectedItemChanged)

    # =============
    # Util Methods
    # =============
    def fpsValueChanged(self):
        """Converts the fps value to a float and emits it through the custom
        fpsChanged signal.

        """

        self.fpsChanged.emit(float(self.fpsValueWidget.text()))

    def updateSceneScaleValueWidget(self, value):
        """Remap the incoming value, set the sceneScaleValueWidget value, and
        emit the custom sceneScaleChanged signal.

        """

        remap_value = float(value) / 1000

        self.sceneScaleValueWidget.setText(str(remap_value))
        self.sceneScaleChanged.emit(remap_value)

    def updateSceneScaleSlider(self):
        """Remap the value from the sceneScaleSlider widget and set the
        sceneScaleValueWidget with this value.

        """

        remap_value = float(self.sceneScaleValueWidget.text()) * 1000

        self.sceneScaleSlider.setValue(remap_value)
        self.sceneScaleChanged.emit(float(self.sceneScaleValueWidget.text()))

    def selectedItemChanged(self, current, previous):
        """Emit the selItemChanged signal when the selected file changes."""

        if current is not None:
            abcFilePath = os.path.join(self._currDir, current.text())
            self.selItemChanged.emit(abcFilePath)

    def openDir(self):
        """Open a widget to allow users to select a directory to load and emit
        the dirChanged signal.

        """

        directory = QtGui.QFileDialog.getExistingDirectory(self, 'Select Directory', self._currDir)
        if directory != '':
            self.dirChanged.emit(directory)
            self.dirPathWidget.setText(directory)
            self.dirPathWidget.setCursorPosition(0)
            self._currDir = directory


if __name__ == '__main__':
    # This only runs when launched directly from the command line.
    # A QApplication is setup and the AlembicViewerUIWidget is instanced and
    # attached to the QApplication and shown.

    app = QtGui.QApplication(sys.argv)

    win = QtGui.QWidget()
    win.setWindowTitle('Alembic Directory Select')
    win.setMinimumSize(600, 400)
    layout = QtGui.QVBoxLayout()
    win.setLayout(layout)

    default_path = os.getcwd()

    fabricDir = os.environ.get('FABRIC_DIR', None)
    if fabricDir is not None:
        default_path = os.path.join(fabricDir, 'Resources')

    abcViewerWidget = AlembicViewerUIWidget(default_path)

    layout.addWidget(abcViewerWidget)

    win.show()
    app.exec_()