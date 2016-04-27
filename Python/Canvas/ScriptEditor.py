import sys, os, traceback
import StringIO
from PySide import QtCore, QtGui
from FabricEngine.Canvas.BindingWrapper import BindingWrapper
from FabricEngine.Canvas.LogWidget import LogWidget

class LogStd:

    def __init__(self, log):
        self._log = log

    def _validate(self):
        if not self._log:
            raise ValueError("I/O operation on closed file")

    def writelines(self, ss):
        self._validate()
        for s in ss:
            self.write(s)

    def close(self):
        self._log = None

    def flush(self):
        self._validate()

    def _raiseNotOpenForReading(self):
        raise IOError('File not open for reading')

    def next(self):
        self._raiseNotOpenForReading()

    def read(self, size=-1):
        self._raiseNotOpenForReading()

    def readline(self, sizehint=-1):
        self._raiseNotOpenForReading()

    def xreadlines(self):
        self._raiseNotOpenForReading()

    def seek(self, offset, whence=None):
        pass

    def tell(self):
        return 0

    def truncate(self, size=-1):
        pass

class LogStdOut(LogStd):

    def __init__(self, log):
        LogStd.__init__(self, log)

    def write(self, s):
        self._log.appendComment(s)

class LogStdErr(LogStd):

    def __init__(self, log):
        LogStd.__init__(self, log)

    def write(self, s):
        self._log.appendException(s)

class ScriptEditor(QtGui.QWidget):

    class CmdEditor(QtGui.QPlainTextEdit):

        returnPressed = QtCore.Signal()


        class LineNumberArea(QtGui.QWidget):

          def __init__(self, cmdEditor):
            QtGui.QWidget.__init__(self, cmdEditor)
            self.__cmdEditor = cmdEditor

          def sizeHint(self):
            return QtCore.QSize(self.__cmdEditor.lineNumberAreaWidth(), 0)

          def paintEvent(self, event):
            self.__cmdEditor.lineNumberAreaPaintEvent(event)

          def mousePressEvent(self, event):
            self.__cmdEditor.lineNumberAreaClickEvent(event)

        def __init__(self):
            QtGui.QPlainTextEdit.__init__(self)

            lineNumberArea = self.LineNumberArea(self)
            self.__lineNumberArea = lineNumberArea

            def updateLineNumberAreaWidth(newBlockCount):
                self.setViewportMargins(self.lineNumberAreaWidth(), 0, 0, 0)
            self.blockCountChanged.connect(updateLineNumberAreaWidth)

            def updateLineNumberArea(rect, dy):
                if dy != 0:
                  lineNumberArea.scroll(0, dy)
                else:
                  lineNumberArea.update(0, rect.y(), lineNumberArea.width(), rect.height())

                if rect.contains(self.viewport().rect()):
                  updateLineNumberAreaWidth(0)
            self.updateRequest.connect(updateLineNumberArea)

            updateLineNumberAreaWidth(0)

        def resizeEvent(self, event):
          QtGui.QWidget.resizeEvent(self, event)
          cr = self.contentsRect()
          self.__lineNumberArea.setGeometry(
            QtCore.QRect(cr.left(), cr.top(), self.lineNumberAreaWidth(), cr.height())
            )

        def lineNumberAreaWidth(self):
          digits = 1
          max = self.blockCount()
          if max < 1:
            max = 1
          while max >= 10:
            max = max / 10
            digits = digits + 1
          return 3 + self.fontMetrics().width('9') * digits + 3

        def lineNumberAreaClickEvent(self, event):
          firstBlock = self.firstVisibleBlock()
          blockHeight = self.blockBoundingRect(firstBlock).height()
          blockNum = int(event.y() / blockHeight) + firstBlock.blockNumber()
          lineNum = blockNum+1
          self.lineClicked.emit(self.__pathname, lineNum)

        def lineNumberAreaPaintEvent(self, event):
          lineNumberArea = self.__lineNumberArea
          painter = QtGui.QPainter(lineNumberArea)
          painter.fillRect(event.rect(), QtCore.Qt.lightGray)
          block = self.firstVisibleBlock()
          blockNumber = block.blockNumber()
          top = int(self.blockBoundingGeometry(block).translated(self.contentOffset()).top())
          bottom = top + int(self.blockBoundingRect(block).height())
          while block.isValid() and top <= event.rect().bottom():
            if block.isVisible() and bottom >= event.rect().top():
              blockNumberText = str(blockNumber + 1)
              painter.setPen(QtCore.Qt.black)
              painter.drawText(
                0, top,
                lineNumberArea.width() - 3, self.fontMetrics().height(),
                QtCore.Qt.AlignRight, blockNumberText
                )
            block = block.next()
            top = bottom
            bottom = top + int(self.blockBoundingRect(block).height())
            blockNumber = blockNumber + 1

        def keyPressEvent(self, event):
            if event.key() == QtCore.Qt.Key_Return and event.modifiers() == QtCore.Qt.ControlModifier:
                event.ignore()
                return
            QtGui.QPlainTextEdit.keyPressEvent(self, event)

    def __init__(self, client, binding, qUndoStack, dfgLogWidget, settings, canvasWindow):
        QtGui.QWidget.__init__(self)

        self.eval_globals = {
            "binding": BindingWrapper(client, binding, qUndoStack),
            "undo": qUndoStack.undo,
            "redo": qUndoStack.redo,
            "newGraph": canvasWindow.onNewGraph,
            }

        self.__undoStackIndex = qUndoStack.index()
        qUndoStack.indexChanged.connect(self.undoStackIndexChanged)
        self.__qUndoStack = qUndoStack

        fixedFont = QtGui.QFont("Courier", 12)
        # fixedFont.setFamily("Monospace")
        # fixedFont.setStyleHint(QtGui.QFont.TypeWriter)
        # fixedFont.setPointSize(14)

        self.dfgLogWidget = dfgLogWidget

        self.settings = settings

        self.log = LogWidget()
        self.log.setFont(fixedFont);

        self.filename = ""

        self.stdout = LogStdOut(self.log)
        self.stderr = LogStdErr(self.log)

        self.cmd = self.CmdEditor()
        self.cmd.setFont(fixedFont)
        self.cmd.returnPressed.connect(self.execute)

        splitter = QtGui.QSplitter(QtCore.Qt.Vertical)
        splitter.setContentsMargins(0,0,0,0)
        splitter.addWidget(self.cmd)
        splitter.addWidget(self.log)

        openAction = QtGui.QAction("Open", self)
        openAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+O"))
        openAction.setToolTip("Open Python script (%s)" % openAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        openAction.triggered.connect(self.open)

        self.saveAction = QtGui.QAction("Save", self)
        self.saveAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+S"))
        self.saveAction.setToolTip("Save Python script (%s)" % self.saveAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        self.saveAction.triggered.connect(self.save)
        self.saveAction.setEnabled(False)
        self.cmd.modificationChanged.connect(self.onModificationChanged)

        saveAsAction = QtGui.QAction("Save As...", self)
        saveAsAction.setShortcut(QtGui.QKeySequence("Alt+Shift+Ctrl+S"))
        saveAsAction.setToolTip("Save Python script As... (%s)" % saveAsAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        saveAsAction.triggered.connect(self.saveAs)

        executeAction = QtGui.QAction("Execute", self)
        executeAction.setShortcut(QtGui.QKeySequence("Ctrl+Return"))
        executeAction.setToolTip("Execute Python script (%s)" % executeAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        executeAction.triggered.connect(self.execute)

        self.echoCommandsAction = QtGui.QAction("Echo Commands", self)
        self.echoCommandsAction.setCheckable(True)
        self.echoCommandsAction.setChecked(bool(self.settings.value("scriptEditor/echoCommands", True)))
        self.echoCommandsAction.toggled.connect(self.echoCommandsToggled)

        toolBar = QtGui.QToolBar()
        toolBar.addAction(openAction)
        toolBar.addAction(self.saveAction)
        toolBar.addAction(saveAsAction)
        toolBar.addSeparator()
        toolBar.addAction(executeAction)
        toolBar.addSeparator()
        toolBar.addAction(self.echoCommandsAction)
        toolBar.addAction(self.log.clearAction("Clear Output"))

        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(toolBar)
        layout.addWidget(splitter)

        self.setContentsMargins(0,0,0,0)
        self.setLayout(layout)

    def undoStackIndexChanged(self, index):
        if self.echoCommandsAction.isChecked():
            if index < self.__undoStackIndex:
                for i in range(self.__undoStackIndex-1, index-1, -1):
                    s = self.__qUndoStack.text(i)
                    if s:
                        self.log.appendCommand("# [UNDO] %s\n" % s)
            else:
                for i in range(self.__undoStackIndex, index, 1):
                    s = self.__qUndoStack.text(i)
                    if s:
                        self.log.appendCommand("# %s\n" % s)
        self.__undoStackIndex = index

    def echoCommandsToggled(self, state):
        self.settings.setValue("scriptEditor/echoCommands", state)

    def updateBinding(self, binding):
        prev = self.eval_globals['binding'] 
        self.eval_globals['binding'] = BindingWrapper(prev.client, binding, prev.qUndoStack)

    def onModificationChanged(self, modification):
        textDocument = self.cmd.document()
        self.saveAction.setEnabled(
            not textDocument.isEmpty() and textDocument.isModified() and len(self.filename) > 0
            )

    def checkUnsavedChanges(self):
        textDocument = self.cmd.document()
        if textDocument.isModified() and not textDocument.isEmpty():
            msgBox = QtGui.QMessageBox()
            msgBox.setText("Do you want to save your Python script?")
            msgBox.setInformativeText(
                "Your changes will be lost if you don't save them.")
            msgBox.setStandardButtons(QtGui.QMessageBox.Save |
                                      QtGui.QMessageBox.Discard |
                                      QtGui.QMessageBox.Cancel)
            msgBox.setDefaultButton(QtGui.QMessageBox.Save)
            result = msgBox.exec_()
            if result == QtGui.QMessageBox.Cancel:
                return False
            if result == QtGui.QMessageBox.Save:
                return self.save()
        return True

    def open(self):
        if not self.checkUnsavedChanges():
            return False
        lastFolder = str(self.settings.value("scriptEditor/lastFolder"))
        filename, _ = QtGui.QFileDialog.getOpenFileName(
            self, "Open Python Script", lastFolder, "*.py"
            )
        filename = str(filename)
        if len(filename) == 0:
            return False
        textDocument = self.cmd.document()
        with open(filename, "r") as fh:
            textDocument.setPlainText(fh.read())
        textDocument.setModified(False)
        self.filename = filename
        return True

    def save(self):
        if not self.filename:
            return self.saveAs()
        else:
            with open(self.filename, "w") as fh:
                fh.write(self.cmd.toPlainText())
            self.cmd.document().setModified(False)
            self.log.appendComment("# Saved Python script as '%s'" % self.filename)
            return True

    def saveAs(self):
        filename = self.filename
        if not filename:
            filename = str(self.settings.value("scriptEditor/lastFolder"))
        filename, _ = QtGui.QFileDialog.getSaveFileName(
            self, "Save Python Script", filename, "*.py"
            )
        if not filename:
            return False
        self.settings.setValue("scriptEditor/lastFolder", os.path.dirname(filename))
        self.filename = filename
        return self.save()

    def execute(self):
        if not self.cmd.textCursor().hasSelection():
            self.cmd.selectAll()
        code = self.cmd.textCursor().selectedText().replace(u"\u2029", "\n")
        self.exec_(code)

    def eval(self, code):
        if self.echoCommandsAction.isChecked():
            self.log.appendCommand(code + "\n")
        old_stdout = sys.stdout
        old_stderr = sys.stderr
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        try:
            result = eval(code, self.eval_globals)
            if self.echoCommandsAction.isChecked() and result is not None:
                self.log.appendCommand("# Result: %s\n" % str(result))
        except:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            sys.stderr.writelines(
                traceback.format_exception(exc_type, exc_value, exc_traceback)
                )
        sys.stderr = old_stderr
        sys.stdout = old_stdout

    def exec_(self, code):
        if self.echoCommandsAction.isChecked():
            self.log.appendCommand(code + "\n")
        old_stdout = sys.stdout
        old_stderr = sys.stderr
        sys.stdout = self.stdout
        sys.stderr = self.stderr
        try:
            exec code in self.eval_globals
        except:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            sys.stderr.writelines(
                traceback.format_exception(exc_type, exc_value, exc_traceback)
                )
        sys.stderr = old_stderr
        sys.stdout = old_stdout
