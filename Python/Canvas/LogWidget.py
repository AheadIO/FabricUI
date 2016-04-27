from PySide import QtCore, QtGui


class AppendingTextWidget(QtGui.QTextEdit):
    def __init__(self):
        QtGui.QTextEdit.__init__(self)

        self.setReadOnly(True)

    def append(self, text, color):
        self.setTextColor(color)
        charFormat = self.currentCharFormat()
        textCursor = self.textCursor()
        textCursor.movePosition(QtGui.QTextCursor.End)
        textCursor.insertText(text, charFormat)
        self.setTextCursor(textCursor)
        self.ensureCursorVisible()

class LogWidget(AppendingTextWidget):
    def __init__(self):
        AppendingTextWidget.__init__(self)

        self.setFocusPolicy(QtCore.Qt.NoFocus)

        self.commandColor = QtGui.QColor(QtCore.Qt.white)
        self.commentColor = QtGui.QColor("#9AD6D6")
        self.exceptionColor = QtGui.QColor("#E14D59")

        self.copyAction = QtGui.QAction("Copy", self)
        self.copyAction.setShortcut(QtGui.QKeySequence.Copy)
        self.copyAction.setEnabled(self.textCursor().hasSelection())
        self.copyAction.triggered.connect(self.copy)
        self.copyAvailable.connect(self.copyAction.setEnabled)

    def clearAction(self, desc):
        result = QtGui.QAction(desc, self)
        result.triggered.connect(self.clear)
        return result

    def appendCommand(self, text):
        self.append(text, self.commandColor)

    def appendComment(self, text):
        self.append(text, self.commentColor)

    def appendException(self, text):
        self.append(text, self.exceptionColor)

    def contextMenuEvent(self, event):
        menu = QtGui.QMenu()
        menu.addAction(self.copyAction)
        menu.addSeparator()
        menu.addAction(self.clearAction("Clear"))
        menu.exec_(self.mapToGlobal(event.pos()))
