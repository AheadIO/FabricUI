import sys, os, traceback
import StringIO
from PySide import QtCore, QtGui
from FabricEngine.Canvas.BindingWrapper import BindingWrapper
from FabricEngine.Canvas.LogWidget import LogWidget
from FabricEngine.Canvas.PythonHighlighter import PythonHighlighter

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
        linesSelected = QtCore.Signal(int, int)

        class LineNumberArea(QtGui.QWidget):

          def __init__(self, cmdEditor):
            QtGui.QWidget.__init__(self, cmdEditor)
            self.__cmdEditor = cmdEditor

          def sizeHint(self):
            return QtCore.QSize(self.__cmdEditor.lineNumberAreaWidth(), 0)

          def paintEvent(self, event):
            self.__cmdEditor.lineNumberAreaPaintEvent(event)

          def mousePressEvent(self, event):
            self.__cmdEditor.lineNumberAreaMousePressEvent(event)

          def mouseMoveEvent(self, event):
            self.__cmdEditor.lineNumberAreaMouseMoveEvent(event)

          def mouseReleaseEvent(self, event):
            self.__cmdEditor.lineNumberAreaMouseReleaseEvent(event)

        def __init__(self, font):
            QtGui.QPlainTextEdit.__init__(self)

            self.setStyleSheet("background-color: #272822;");
            self.__highlighter = PythonHighlighter(self.document())

            lineNumberArea = self.LineNumberArea(self)
            self.__lineNumberArea = lineNumberArea

            self.setFont(font)
            self.setTabStopWidth(4 * self.fontMetrics().width(' '));

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
          if digits < 3:
            digits = 3
          return 3 + self.fontMetrics().width('9') * digits + 3

        def lineNumberAreaMousePressEvent(self, event):
            firstBlock = self.firstVisibleBlock()
            blockHeight = self.blockBoundingRect(firstBlock).height()
            self.__mousePressStartBlockNumber = int(event.y() / blockHeight) + firstBlock.blockNumber()
            self.linesSelected.emit(self.__mousePressStartBlockNumber, self.__mousePressStartBlockNumber)

        def lineNumberAreaMouseMoveEvent(self, event):
            firstBlock = self.firstVisibleBlock()
            blockHeight = self.blockBoundingRect(firstBlock).height()
            blockNumber = int(event.y() / blockHeight) + firstBlock.blockNumber()
            self.linesSelected.emit(self.__mousePressStartBlockNumber, blockNumber)

        def lineNumberAreaMouseReleaseEvent(self, event):
            pass

        def lineNumberAreaPaintEvent(self, event):
          lineNumberArea = self.__lineNumberArea
          painter = QtGui.QPainter(lineNumberArea)
          painter.fillRect(event.rect(), QtCore.Qt.lightGray)
          block = self.firstVisibleBlock()
          blockNumber = block.blockNumber()
          top = int(self.blockBoundingGeometry(block).translated(self.contentOffset()).top())
          bottom = top + int(self.blockBoundingRect(block).height())
          painter.setFont(self.font())
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

        def leadingSpaceCount(self, tb):
            s = tb.text()
            result = 0
            for c in s:
                if c == " ":
                    result += 1
                elif c == '\t':
                    result = int((result + 4)/4) * 4
                else:
                    break
            return result

        def isInLeadingSpace(self, tc):
            tb = tc.block()
            s = tb.text()
            for i in range(0, tc.positionInBlock()):
                if not s[i] in [" ", "\t"]:
                    return False
            return True

        def keyPressEvent(self, event):
            # print "event.key() = %s" % str(event.key())
            if event.key() == QtCore.Qt.Key_Return:
                if event.modifiers() == QtCore.Qt.ControlModifier:
                    event.ignore()
                    return
            elif event.key() == QtCore.Qt.Key_Tab:
                if (event.modifiers() & ~QtCore.Qt.ShiftModifier) == 0:
                    tc = self.textCursor()
                    if tc.hasSelection() or self.isInLeadingSpace(tc):
                        self.indent()
                    else:
                        self.insertTab()
                    event.accept()
                    return
            elif event.key() == QtCore.Qt.Key_Backtab:
                if (event.modifiers() & ~QtCore.Qt.ShiftModifier) == 0:
                    tc = self.textCursor()
                    if tc.hasSelection() or self.isInLeadingSpace(tc):
                        self.exdent()
                    else:
                        self.insertTab()
                    event.accept()
                    return
            elif event.key() == QtCore.Qt.Key_Backspace:
                if event.modifiers() == QtCore.Qt.NoModifier:
                    tc = self.textCursor()
                    if not tc.hasSelection() and not tc.atBlockStart() and self.isInLeadingSpace(tc):
                        self.exdent()
                        event.accept()
                        return
            elif event.key() == QtCore.Qt.Key_BracketLeft:
                if event.modifiers() == QtCore.Qt.ControlModifier:
                    self.exdent()
                    event.accept()
                    return
            elif event.key() == QtCore.Qt.Key_BracketRight:
                if event.modifiers() == QtCore.Qt.ControlModifier:
                    self.indent()
                    event.accept()
                    return
            QtGui.QPlainTextEdit.keyPressEvent(self, event)

        def insertTab(self):
            tc = self.textCursor()
            try:
                tc.beginEditBlock()
                tb = tc.block()
                s = tb.text()
                leadingChars = 0
                for i in range(0, tc.positionInBlock()):
                    if s[i] == '\t':
                        leadingChars = int((leadingChars + 4) / 4) * 4
                    else:
                        leadingChars += 1
                spacesToAdd = int((leadingChars + 4) / 4) * 4 - leadingChars
                tc.insertText(' '*spacesToAdd)
                self.setTextCursor(tc)
            finally:
                tc.endEditBlock()

        def indent(self):
            otc = self.textCursor()
            tc = self.textCursor()
            try:
                tc.beginEditBlock()
                td = self.document()
                sp = tc.selectionStart()
                ep = tc.selectionEnd()
                tc.setPosition(sp)
                sbn = tc.blockNumber()
                tc.setPosition(ep)
                ebn = tc.blockNumber()
                if tc.positionInBlock() == 0 and ebn > sbn:
                    ebn -= 1
                for bn in range(sbn, ebn+1):
                    tb = td.findBlockByNumber(bn)
                    oldLeadingSpaceCount = self.leadingSpaceCount(tb)
                    newLeadingSpaceCount = int((oldLeadingSpaceCount + 4) / 4) * 4
                    tc.setPosition(tb.position())
                    tc.setPosition(tb.position() + oldLeadingSpaceCount, tc.KeepAnchor)
                    tc.insertText(' ' * newLeadingSpaceCount)
                self.setTextCursor(otc)
            finally:
                tc.endEditBlock()

        def exdent(self):
            otc = self.textCursor()
            tc = self.textCursor()
            try:
                tc.beginEditBlock()
                td = self.document()
                sp = tc.selectionStart()
                ep = tc.selectionEnd()
                tc.setPosition(sp)
                sbn = tc.blockNumber()
                tc.setPosition(ep)
                ebn = tc.blockNumber()
                if tc.positionInBlock() == 0 and ebn > sbn:
                    ebn -= 1
                for bn in range(sbn, ebn+1):
                    tb = td.findBlockByNumber(bn)
                    s = tb.text()
                    oldLeadingSpaceCount = self.leadingSpaceCount(tb)
                    if oldLeadingSpaceCount > 4:
                        newLeadingSpaceCount = int((oldLeadingSpaceCount - 3) / 4) * 4
                    else:
                        newLeadingSpaceCount = 0
                    tc.setPosition(tb.position())
                    tc.setPosition(tb.position() + oldLeadingSpaceCount, tc.KeepAnchor)
                    tc.insertText(' ' * newLeadingSpaceCount)
                self.setTextCursor(otc)
            finally:
                tc.endEditBlock()

    titleDataChanged = QtCore.Signal(str, bool)

    def __init__(self, client, binding, qUndoStack, dfgLogWidget, settings, canvasWindow):
        QtGui.QWidget.__init__(self)

        self.setObjectName('ScriptEditorWidget')

        widgetStyleSheet = """

        QWidget#ScriptEditorWidget QToolButton {
            border: 1px #222222;
            border-radius: 3px;
            border-style: outset;
            padding: 3px;
            margin: 2px;

            background-color: #2B2B2B;
        }

        QWidget#ScriptEditorWidget QToolButton::disabled {
            background-color: #444;
            color: #666666;
        }

        QWidget#ScriptEditorWidget QToolButton::checked {
            background-color: #335252;
        }

        QWidget#ScriptEditorWidget QToolButton::checked:hover {
            background-color: #335959;
        }

        QWidget#ScriptEditorWidget QToolButton:hover {
            background-color: #3B3B3B;
        }

        QWidget#ScriptEditorWidget QToolButton::pressed {
            border-style: inset;
            padding-top: 5px;
            padding-left: 5px;

            background-color: #2B2B2B;
        }

        """

        self.setStyleSheet(widgetStyleSheet)

        self.__undoStackIndex = qUndoStack.index()
        qUndoStack.indexChanged.connect(self.undoStackIndexChanged)
        self.__qUndoStack = qUndoStack
        self.__echoStackIndexChanges = True

        self.eval_globals = {
            "binding": BindingWrapper(client, binding, qUndoStack),
            "newGraph": canvasWindow.onNewGraph,
            "newScript": self.newScript,
            "loadScript": self.loadScript,
            }

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

        self.cmd = self.CmdEditor(fixedFont)
        self.cmd.returnPressed.connect(self.execute)
        self.cmd.linesSelected.connect(self.onLinesSelected)

        splitter = QtGui.QSplitter(QtCore.Qt.Vertical)
        splitter.setContentsMargins(0,0,0,0)
        splitter.addWidget(self.cmd)
        splitter.addWidget(self.log)

        newAction = QtGui.QAction("New", self)
        newAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+N"))
        newAction.setToolTip("New script (%s)" % newAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        newAction.triggered.connect(self.newScript)

        openAction = QtGui.QAction("Open", self)
        openAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+O"))
        openAction.setToolTip("Open script (%s)" % openAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        openAction.triggered.connect(self.open)

        self.saveAction = QtGui.QAction("Save", self)
        self.saveAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+S"))
        self.saveAction.setToolTip("Save script (%s)" % self.saveAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        self.saveAction.triggered.connect(self.save)
        self.saveAction.setEnabled(False)
        self.cmd.modificationChanged.connect(self.onModificationChanged)

        saveAsAction = QtGui.QAction("Save As...", self)
        saveAsAction.setShortcut(QtGui.QKeySequence("Alt+Shift+Ctrl+S"))
        saveAsAction.setToolTip("Save script As... (%s)" % saveAsAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        saveAsAction.triggered.connect(self.saveAs)

        executeAction = QtGui.QAction("Execute", self)
        executeAction.setShortcut(QtGui.QKeySequence("Ctrl+Return"))
        executeAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
        executeAction.setToolTip("Execute script (%s)" % executeAction.shortcut().toString(QtGui.QKeySequence.NativeText))
        executeAction.triggered.connect(self.execute)

        self.echoCommandsAction = QtGui.QAction("Echo Commands", self)
        self.echoCommandsAction.setCheckable(True)
        self.echoCommandsAction.setChecked(bool(self.settings.value("scriptEditor/echoCommands", True)))
        self.echoCommandsAction.toggled.connect(self.echoCommandsToggled)

        toolBar = QtGui.QToolBar()
        toolBar.addAction(newAction)
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

    def onLinesSelected(self, startLineNum, endLineNum):
        startTextBlock = self.cmd.document().findBlockByLineNumber(startLineNum)
        endTextBlock = self.cmd.document().findBlockByLineNumber(endLineNum)
        textCursor = self.cmd.textCursor()
        if startLineNum > endLineNum:
            textCursor.setPosition(startTextBlock.position() + startTextBlock.length())
            textCursor.setPosition(endTextBlock.position(), textCursor.KeepAnchor)
        else:
            textCursor.setPosition(startTextBlock.position())
            textCursor.setPosition(endTextBlock.position() + endTextBlock.length(), textCursor.KeepAnchor)
        self.cmd.setTextCursor(textCursor)

    def undoStackIndexChanged(self, index):
        if self.__echoStackIndexChanges and self.echoCommandsAction.isChecked():
            if index < self.__undoStackIndex:
                for i in range(self.__undoStackIndex-1, index-1, -1):
                    s = self.__qUndoStack.text(i)
                    if s:
                        self.log.appendCommand("# [UNDO] %s\n" % s)
            else:
                for i in range(self.__undoStackIndex, index, 1):
                    s = self.__qUndoStack.text(i)
                    if s:
                        self.log.appendCommand("# [REDO] %s\n" % s)
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
        self.titleDataChanged.emit(self.filename, modification)

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

    def newScript(self):
        if not self.checkUnsavedChanges():
            return False
        self.filename = ""
        textDocument = self.cmd.document()
        textDocument.setPlainText("")
        textDocument.setModified(False)
        self.titleDataChanged.emit("", False)
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
        return self.loadScript(filename)

    def loadScript(self, filename):
        textDocument = self.cmd.document()
        try:
            with open(filename, "r") as fh:
                textDocument.setPlainText(fh.read())
            textDocument.setModified(False)
            self.filename = filename
            self.titleDataChanged.emit(filename, False)
            return True
        except:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            sys.stderr.writelines(
                traceback.format_exception(exc_type, exc_value, exc_traceback)
                )
            return False

    def save(self):
        if not self.filename:
            return self.saveAs()
        else:
            with open(self.filename, "w") as fh:
                fh.write(self.cmd.toPlainText())
            self.cmd.document().setModified(False)
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
        self.titleDataChanged.emit(filename, self.cmd.document().isModified())
        return self.save()

    def execute(self):
        if self.cmd.textCursor().hasSelection():
            code = self.cmd.textCursor().selectedText().replace(u"\u2029", "\n")
        else:
            code = self.cmd.toPlainText()
        self.exec_(code)

    def eval(self, code):
        if self.echoCommandsAction.isChecked():
            self.log.appendCommand(code + "\n")
        oldEchoStackIndexChanges = self.__echoStackIndexChanges
        self.__echoStackIndexChanges = False
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
        self.__echoStackIndexChanges = oldEchoStackIndexChanges
        return result

    def exec_(self, code):
        if self.echoCommandsAction.isChecked():
            self.log.appendCommand(code + "\n")
        oldEchoStackIndexChanges = self.__echoStackIndexChanges
        self.__echoStackIndexChanges = False
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
        self.__echoStackIndexChanges = oldEchoStackIndexChanges
