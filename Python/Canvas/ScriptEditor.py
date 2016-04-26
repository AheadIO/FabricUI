import sys, os
import StringIO
import contextlib
from PySide import QtCore, QtGui
from FabricEngine.Canvas.BindingWrapper import BindingWrapper
from FabricEngine.Canvas.LogWidget import LogWidget

@contextlib.contextmanager
def stdoutIO():
    old_stdout = sys.stdout
    old_stderr = sys.stderr
    sio = StringIO.StringIO()
    sys.stdout = sio
    sys.stderr = sio
    yield sio
    sys.stdout = old_stdout
    sys.stderr = old_stderr

class ScriptEditor(QtGui.QWidget):

    class CmdEditor(QtGui.QPlainTextEdit):

        returnPressed = QtCore.Signal()

        def __init__(self):
            QtGui.QPlainTextEdit.__init__(self)

        def keyPressEvent(self, event):
            if event.key() == QtCore.Qt.Key_Return and event.modifiers() == QtCore.Qt.ControlModifier:
                event.ignore()
                return
            QtGui.QPlainTextEdit.keyPressEvent(self, event)

    def __init__(self, client, binding, qUndoStack, dfgLogWidget, settings):
        QtGui.QWidget.__init__(self)

        self.eval_globals = {
            "binding": BindingWrapper(client, binding, qUndoStack),
            "undo": qUndoStack.undo,
            "redo": qUndoStack.redo,
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

        self.cmd = self.CmdEditor()
        self.cmd.setFont(fixedFont)
        self.cmd.returnPressed.connect(self.execute)

        cmdLayout = QtGui.QHBoxLayout()
        cmdLayout.setContentsMargins(0,0,0,0)
        cmdLayout.addWidget(QtGui.QLabel("Editor:"))
        cmdLayout.addWidget(self.cmd)

        cmdContainer = QtGui.QWidget()
        cmdContainer.setContentsMargins(0,0,0,0)
        cmdContainer.setLayout(cmdLayout)

        logLayout = QtGui.QHBoxLayout()
        logLayout.setContentsMargins(0,0,0,0)
        logLayout.addWidget(QtGui.QLabel("History:"))
        logLayout.addWidget(self.log)

        logContainer = QtGui.QWidget()
        logContainer.setContentsMargins(0,0,0,0)
        logContainer.setLayout(logLayout)

        splitter = QtGui.QSplitter(QtCore.Qt.Vertical)
        splitter.setContentsMargins(0,0,0,0)
        splitter.addWidget(cmdContainer)
        splitter.addWidget(logContainer)

        openAction = QtGui.QAction("Open", self)
        openAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+O"))
        openAction.setToolTip("Open Python Script (%s)" % openAction.shortcut().toString())
        openAction.triggered.connect(self.open)

        self.saveAction = QtGui.QAction("Save", self)
        self.saveAction.setShortcut(QtGui.QKeySequence("Alt+Ctrl+S"))
        self.saveAction.setToolTip("Save Python Script (%s)" % self.saveAction.shortcut().toString())
        self.saveAction.triggered.connect(self.save)
        self.saveAction.setEnabled(False)
        self.cmd.modificationChanged.connect(self.onModificationChanged)

        saveAsAction = QtGui.QAction("Save As...", self)
        saveAsAction.setShortcut(QtGui.QKeySequence("Alt+Shift+Ctrl+S"))
        saveAsAction.setToolTip("Save Python Script As... (%s)" % saveAsAction.shortcut().toString())
        saveAsAction.triggered.connect(self.saveAs)

        executeAction = QtGui.QAction("Execute", self)
        executeAction.setShortcut(QtGui.QKeySequence("Ctrl+Return"))
        executeAction.setToolTip("Execute Python Script (%s)" % executeAction.shortcut().toString())
        executeAction.triggered.connect(self.execute)

        toolBar = QtGui.QToolBar()
        toolBar.addAction(openAction)
        toolBar.addAction(self.saveAction)
        toolBar.addAction(saveAsAction)
        toolBar.addSeparator()
        toolBar.addAction(executeAction)

        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(toolBar)
        layout.addWidget(splitter)

        self.setContentsMargins(0,0,0,0)
        self.setLayout(layout)

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
        lastFolder = str(self.settings.value("mainWindow/lastScriptFolder"))
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
            filename = str(self.settings.value("mainWindow/lastScriptFolder"))
        filename, _ = QtGui.QFileDialog.getSaveFileName(
            self, "Save Python Script", filename, "*.py"
            )
        if not filename:
            return False
        self.settings.setValue("mainWindow/lastScriptFolder", os.path.dirname(filename))
        self.filename = filename
        return self.save()

    def execute(self):
        if not self.cmd.textCursor().hasSelection():
            self.cmd.selectAll()
        code = self.cmd.textCursor().selectedText().replace(u"\u2029", "\n")
        self.exec_(code)

    def eval(self, code):
        self.log.appendCommand(code)
        try:
            result = eval(code, self.eval_globals)
            if result is not None:
                self.log.appendComment("# Result: " + str(result))
            return result
        except Exception as e:
            self.log.appendException("# Exception: " + str(e))
            sys.stderr.write("# Exception: " + str(e) + "\n")

    def exec_(self, code):
        self.log.appendCommand(code)
        try:
            with stdoutIO() as sio:
                exec code in self.eval_globals
                for s in sio.getvalue()[:-1].split("\n"):
                    self.dfgLogWidget.log(s)
        except Exception as e:
            self.log.appendException("# Exception: " + str(e))
            sys.stderr.write("# Exception: " + str(e) + "\n")
