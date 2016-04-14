import sys
from PySide import QtCore, QtGui
from FabricEngine.Canvas.BindingWrapper import BindingWrapper
from FabricEngine.Canvas.LogWidget import LogWidget
import StringIO
import contextlib

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
            QtGui.QPlainTextEdit.keyPressEvent(self, event)

            if event.key() == QtCore.Qt.Key_Return and event.modifiers() == QtCore.Qt.NoModifier:
                self.selectAll()
                self.returnPressed.emit()

    def __init__(self, client, binding, qUndoStack, dfgLogWidget):
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

        self.log = LogWidget()
        self.log.setFont(fixedFont);

        self.cmd = self.CmdEditor()
        self.cmd.setFont(fixedFont)
        self.cmd.returnPressed.connect(self.onReturnPressed)

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

        layout = QtGui.QHBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(splitter)

        self.setContentsMargins(0,0,0,0)
        self.setLayout(layout)

    def updateBinding(self, binding):
        prev = self.eval_globals['binding'] 
        self.eval_globals['binding'] = BindingWrapper(prev.client, binding, prev.qUndoStack)

    def onReturnPressed(self):
        code = self.cmd.toPlainText()
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
