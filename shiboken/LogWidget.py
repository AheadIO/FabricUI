
from FabricEngine import Core
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
    self.commentColor = QtGui.QColor(QtCore.Qt.black)
    self.exceptionColor = QtGui.QColor("#E04040")

  def appendCommand(self, text):
    self.append(text, self.commandColor)

  def appendComment(self, text):
    self.append(text, self.commentColor)

  def appendException(self, text):
    self.append(text, self.exceptionColor)
