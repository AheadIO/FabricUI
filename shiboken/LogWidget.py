import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, Style, Viewports
from PySide import QtCore, QtGui, QtOpenGL


class AppendingTextWidget(QtGui.QTextEdit):
  def __init__(self):
    QtGui.QTextEdit.__init__(self)

    self.setReadOnly(True)

  def append(self, text, color):
    self.setTextColor(color)
    charFormat = self.currentCharFormat()
    textCursor = self.textCursor()
    textCursor.movePosition(QtGui.QTextCursor.End)
    textCursor.insertText(text + "\n", charFormat)
    self.setTextCursor(textCursor)
    self.ensureCursorVisible()

  def appendSeparator(self):
    textCursor = self.textCursor()
    textCursor.movePosition(QtGui.QTextCursor.End)
    textCursor.insertHtml("<hr /><br />")
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

    self.clearAction = QtGui.QAction("Clear", self)
    self.clearAction.triggered.connect(self.clear)

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
    menu.addAction(self.clearAction)
    menu.exec_(self.mapToGlobal(event.pos()))
