"""Alembic File List Widget that shows Alembic files in a specified directory."""

import glob
import os
import sys

from PySide import QtCore, QtGui


class AlembicFileListWidget(QtGui.QListWidget):
    """List widget that displays all Alembic files in the specified directory.

    Base sample code taken from:
    http://pythoncentral.io/pyside-pyqt-tutorial-the-qlistwidget/

    Arguments:
        dirPath (str): The initial directory the widget will display files for.
        parent (QWidget): Parent widget item.

    """

    def __init__(self, dirPath, parent=None):
        super(AlembicFileListWidget, self).__init__(parent)
        self.setDirpath(dirPath)

    def setDirpath(self, dirPath):
        """Set the current directory and refresh the list.

        Args:
            dirPath (str): Directory path to search for Alembic files in.

        """

        self._dirpath = dirPath
        self._populate()

    def _abcFiles(self):
        """Get the list of filenames of Alembic files in specified path.

        Returns:
            list: List of Alembic file in the specified directory.

        """

        abcFiles = []

        pattern = os.path.join(self._dirpath, '*.%s' % 'abc')
        abcFiles.extend(glob.glob(pattern))

        return abcFiles

    def _populate(self):
        """Fill the list with Alembic files from the specified directory.

        """

        self.clear()

        # Create a list item for each file, setting the text and icon appropriately
        for abcFile in self._abcFiles():
            item = QtGui.QListWidgetItem(self)
            item.setText(os.path.split(abcFile)[1])


if __name__ == '__main__':
    # This only runs when launched directly from the command line.
    # A QApplication is setup and the AlembicFileListWidget is instanced and
    # attached to the QApplication and shown.
    #
    # This is used to test that the widget is functioning properly outside of
    # the application context.

    app = QtGui.QApplication(sys.argv)

    win = QtGui.QWidget()
    win.setWindowTitle('Alembic File List')
    win.setMinimumSize(600, 400)
    layout = QtGui.QVBoxLayout()
    win.setLayout(layout)

    # Set the default directory to load.
    default_path = os.getcwd()

    fabricDir = os.environ.get('FABRIC_DIR', None)
    if fabricDir is not None:
        default_path = os.path.join(fabricDir, 'Resources')

    # Add Widgets to the layout
    abcListWidget = AlembicFileListWidget(default_path)
    layout.addWidget(abcListWidget)
    entry = QtGui.QLineEdit(win)
    layout.addWidget(entry)

    # Create a method to update a text field when item in AlembicFileListWidget
    # has changed to test the output from currentItemChanged signal.
    def on_item_changed(curr, prev):
        entry.setText(curr.text())

    abcListWidget.currentItemChanged.connect(on_item_changed)

    win.show()
    app.exec_()