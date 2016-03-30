import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, Style, Viewports
from PySide import QtCore, QtGui, QtOpenGL
from BindingWrapper import BindingWrapper
from LogWidget import LogWidget


class ScriptEditor(QtGui.QWidget):

    def __init__(self, client, binding, qUndoStack):
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

        self.log = LogWidget()
        self.log.setFont(fixedFont);

        self.cmd = QtGui.QLineEdit()
        self.cmd.setFont(fixedFont)
        self.cmd.returnPressed.connect(self.onReturnPressed)

        layout = QtGui.QHBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(QtGui.QLabel("Python:"))
        layout.addWidget(self.cmd)

        bottom = QtGui.QWidget()
        bottom.setContentsMargins(0,0,0,0)
        bottom.setLayout(layout)

        layout = QtGui.QVBoxLayout()
        layout.setContentsMargins(0,0,0,0)
        layout.addWidget(self.log)
        layout.addWidget(bottom)

        self.setContentsMargins(0,0,0,0)
        self.setLayout(layout)

    def updateBinding(self, binding):
        prev = self.eval_globals['binding'] 
        self.eval_globals['binding'] = BindingWrapper(prev.client, binding, prev.qUndoStack)

    def onReturnPressed(self):
        code = self.cmd.text()
        self.execute(code)

    def eval(self, code):
        self.log.appendCommand(code)
        try:
            result = eval(code, self.eval_globals)
            if result is not None:
                self.log.appendComment("# Result: " + str(result))
            return result
        except Exception as e:
            self.log.appendException("# Exception: " + str(e))

    def exec_(self, code):
        self.log.appendCommand(code)
        try:
            exec code in self.eval_globals
        except Exception as e:
            self.log.appendException("# Exception: " + str(e))
