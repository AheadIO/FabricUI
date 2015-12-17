import optparse, os, sys
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG, KLASTManager, Style, Viewports
from PySide import QtCore, QtGui, QtOpenGL

# [andrew 20151028] shiboken thinks FabricStyle is an abstract class
# unless we re-define the virtual standardPixmap method
class FabricStyle(Style.FabricStyle):
    def __init__(self):
        super(FabricStyle, self).__init__()

    def standardPixmap(self, _standardPixmap, _option=None, _widget=None):
        return QtGui.QPixmap()

class MainWindowEventFilter(QtCore.QObject):
    def __init__(self, window):
        super(MainWindowEventFilter, self).__init__()
        self.window = window

    def eventFilter(self, obj, event):
        eventType = event.type()

        if eventType == QtCore.QEvent.KeyPress:
            keyEvent = QtGui.QKeyEvent(event)

            if keyEvent.key() != QtCore.Qt.Key_Tab:
                self.window.viewport.onKeyPressed(keyEvent)
                if keyEvent.isAccepted():
                    return True

                self.window.dfgWidget.onKeyPressed(keyEvent)
                if keyEvent.isAccepted():
                    return True
        elif eventType == QtCore.QEvent.KeyRelease:
            keyEvent = QtGui.QKeyEvent(event)

            if keyEvent.key() != QtCore.Qt.Key_Tab:
                self.window.dfgWidget.onKeyReleased(keyEvent)
                if keyEvent.isAccepted():
                    return True

        return QtCore.QObject.eventFilter(obj, event)

class UndoCmd(QtGui.QUndoCommand):
    def __init__(self, cmd):
        QtGui.QUndoCommand.__init__(self)
        self.cmd = cmd
        self.done = False
    def redo(self):
        if not self.done:
            self.done = True
            self.cmd.doit()
        else:
            self.cmd.redo()
    def undo(self):
        self.cmd.undo()

def InvokeCmd(cmd, qUndoStack):
    undoCmd = UndoCmd(cmd)
    qUndoStack.push(undoCmd)
    undoCmd.setText(cmd.getDesc())

class BindingWrapper:

    def __init__(self, client, binding, qUndoStack):
        self.client = client
        self.binding = binding
        self.qUndoStack = qUndoStack

    @staticmethod
    def splitInts(packedIndices):
        return map(
            lambda indexStr: int(indexStr),
            packedIndices.split('|')
            )

    @staticmethod
    def splitNames(names):
        return names.split('|')

    @staticmethod
    def splitPoss(posXs, posYs):
        if isinstance(posXs, basestring):
            posXs = map(
                lambda str: float(str),
                posXs.split('|')
                )
        elif not hasattr(posXs, '__iter__'):
            posXs = [posXs]
        if isinstance(posYs, basestring):
            posYs = map(
                lambda str: float(str),
                posYs.split('|')
                )
        elif not hasattr(posYs, '__iter__'):
            posYs = [posYs]
        result = []
        for i in range(0, len(posXs)):
            result.append(QtCore.QPointF(posXs[i], posYs[i]))
        return result

    def undo(
        self,
        ):
        self.qUndoStack.undo()

    def redo(
        self,
        ):
        self.qUndoStack.redo()

    def instPreset(
        self,
        execPath,
        presetPath,
        posX,
        posY
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_InstPreset(
            self.binding,
            execPath,
            exec_,
            presetPath,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def addGraph(
        self,
        execPath,
        title,
        posX,
        posY
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_AddGraph(
            self.binding,
            execPath,
            exec_,
            title,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def addFunc(
        self,
        execPath,
        title,
        initialCode,
        posX,
        posY
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_AddFunc(
            self.binding,
            execPath,
            exec_,
            title,
            initialCode,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def addBackDrop(
        self,
        execPath,
        title,
        posX,
        posY,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_AddBackDrop(
            self.binding,
            execPath,
            exec_,
            title,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def addVar(
        self,
        execPath,
        desiredNodeName,
        dataType,
        extDep,
        posX,
        posY
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_AddVar(
            self.binding,
            execPath,
            exec_,
            desiredNodeName,
            dataType,
            extDep,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def addGet(
        self,
        execPath,
        desiredNodeName,
        varPath,
        posX,
        posY
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_AddGet(
            self.binding,
            execPath,
            exec_,
            desiredNodeName,
            varPath,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def addSet(
        self,
        execPath,
        desiredNodeName,
        varPath,
        posX,
        posY
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_AddSet(
            self.binding,
            execPath,
            exec_,
            desiredNodeName,
            varPath,
            QtCore.QPointF(posX, posY)
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNodeName()

    def moveNodes(
        self,
        execPath,
        packedNodeNames,
        packedNewTopLeftPosXs,
        packedNewTopLeftPosYs
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        nodeNames = BindingWrapper.splitNames(packedNodeNames)
        newTopLeftPoss = BindingWrapper.splitPoss(
            packedNewTopLeftPosXs,
            packedNewTopLeftPosYs
            )
        cmd = DFG.DFGUICmd_MoveNodes(
            self.binding,
            execPath,
            exec_,
            nodeNames,
            newTopLeftPoss
            )
        InvokeCmd(cmd, self.qUndoStack)

    def resizeBackDrop(
        self,
        execPath,
        backDropNodeName,
        newTopLeftPosX,
        newTopLeftPosY,
        newSizeW,
        newSizeH,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_ResizeBackDrop(
            self.binding,
            execPath,
            exec_,
            backDropNodeName,
            QtCore.QPointF(newTopLeftPosX, newTopLeftPosY),
            QtCore.QSizeF(newSizeW, newSizeH),
            )
        InvokeCmd(cmd, self.qUndoStack)

    def removeNodes(
        self,
        execPath,
        packedNodeNames,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        nodeNames = BindingWrapper.splitNames(packedNodeNames)
        cmd = DFG.DFGUICmd_RemoveNodes(
            self.binding,
            execPath,
            exec_,
            nodeNames,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def implodeNodes(
        self,
        execPath,
        packedNodeNames,
        desiredNodeName
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        nodeNames = BindingWrapper.splitNames(packedNodeNames)
        cmd = DFG.DFGUICmd_ImplodeNodes(
            self.binding,
            execPath,
            exec_,
            nodeNames,
            desiredNodeName
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualImplodedNodeName()

    def explodeNode(
        self,
        execPath,
        nodeName
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_Explode(
            self.binding,
            execPath,
            exec_,
            nodeName
            )
        InvokeCmd(cmd, self.qUndoStack)
        return "|".join(cmd.getExplodedNodeNames())

    def connect(
        self,
        execPath,
        srcPort,
        dstPort,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_Connect(
            self.binding,
            execPath,
            exec_,
            srcPort,
            dstPort
            )
        InvokeCmd(cmd, self.qUndoStack)

    def disconnect(
        self,
        execPath,
        srcPort,
        dstPort,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_Disonnect(
            self.binding,
            execPath,
            exec_,
            srcPort,
            dstPort
            )
        InvokeCmd(cmd, self.qUndoStack)

    def addPort(
        self,
        execPath,
        desiredPortName,
        portTypeStr,
        typeSpec,
        portToConnect,
        extDep,
        metaData,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        portTypeStrLower = portTypeStr.lower()
        if portTypeStrLower == "io":
            portType = self.client.DFG.PortTypes.IO
        elif portTypeStrLower == "out":
            portType = self.client.DFG.PortTypes.Out
        else:
            portType = self.client.DFG.PortTypes.In
        cmd = DFG.DFGUICmd_AddPort(
            self.binding,
            execPath,
            exec_,
            desiredPortName,
            portType,
            typeSpec,
            portToConnect,
            extDep,
            metaData,
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualPortName()

    def editPort(
        self,
        execPath,
        oldPortName,
        desiredNewPortName,
        typeSpec,
        extDep,
        metaData,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_EditPort(
            self.binding,
            execPath,
            exec_,
            oldPortName,
            desiredNewPortName,
            typeSpec,
            extDep,
            metaData,
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNewPortName()

    def removePort(
        self,
        execPath,
        portName,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_RemovePort(
            self.binding,
            execPath,
            exec_,
            portName,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def createPreset(
        self,
        execPath,
        nodeName,
        presetDirPath,
        presetName,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_CreatePreset(
            self.binding,
            execPath,
            exec_,
            nodeName,
            presetDirPath,
            presetName,
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getPathname()

    def setNodeComment(
        self,
        execPath,
        nodeName,
        comment,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_SetNodeComment(
            self.binding,
            execPath,
            exec_,
            nodeName,
            comment,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def setCode(
        self,
        execPath,
        code,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_SetCode(
            self.binding,
            execPath,
            exec_,
            code,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def editNode(
        self,
        execPath,
        oldNodeName,
        desiredNewNodeName,
        nodeMetadata,
        execMetadata,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_EditNode(
            self.binding,
            execPath,
            exec_,
            oldNodeName,
            desiredNewNodeName,
            nodeMetadata,
            execMetadata,
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNewNodeName()

    def renamePort(
        self,
        execPath,
        oldPortName,
        desiredNewPortName,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_RenamePort(
            self.binding,
            execPath,
            exec_,
            oldPortName,
            desiredNewPortName,
            )
        InvokeCmd(cmd, self.qUndoStack)
        return cmd.getActualNewPortName()

    def paste(
        self,
        execPath,
        json,
        cursorPosX,
        cursorPosY,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_Paste(
            self.binding,
            execPath,
            exec_,
            json,
            QtCore.QPointF(cursorPosX, cursorPosY),
            )
        InvokeCmd(cmd, self.qUndoStack)
        return "|".join(cmd.getPastedNodeNames())

    def setArgValue(
        self,
        argName,
        typeName,
        valueJSON,
        ):
        value = getattr(self.client.RT.types, typeName)()
        value.setJSON(valueJSON)
        cmd = DFG.DFGUICmd_SetArgValue(
            self.binding,
            argName,
            value,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def setPortDefaultValue(
        self,
        execPath,
        portPath,
        typeName,
        valueJSON,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        value = getattr(self.client.RT.types, typeName)()
        value.setJSON(valueJSON)
        cmd = DFG.DFGUICmd_SetPortDefaultValue(
            self.binding,
            execPath,
            exec_,
            portPath,
            value,
            )

    def setRefVarPath(
        self,
        execPath,
        refName,
        varPath,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_SetRefVarPath(
            self.binding,
            execPath,
            exec_,
            refName,
            varPath,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def reorderPorts(
        self,
        execPath,
        packedIndices,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        indices = BindingWrapper.splitInts(packedIndices)
        cmd = DFG.DFGUICmd_ReorderPorts(
            self.binding,
            execPath,
            exec_,
            indices,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def setExtDeps(
        self,
        execPath,
        packedExtDeps,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        extDeps = BindingWrapper.splitNames(packedExtDeps)
        cmd = DFG.DFGUICmd_SetExtDeps(
            self.binding,
            execPath,
            exec_,
            extDeps,
            )
        InvokeCmd(cmd, self.qUndoStack)

    def splitFromPreset(
        self,
        execPath,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        cmd = DFG.DFGUICmd_SplitFromPreset(
            self.binding,
            execPath,
            exec_,
            )
        InvokeCmd(cmd, self.qUndoStack)

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

class UICmdHandler(DFG.DFGUICmdHandler_Python):

    def __init__(self, client, scriptEditor):
        super(UICmdHandler, self).__init__()
        self.client = client
        self.scriptEditor = scriptEditor

    @staticmethod
    def encodeStringChars(string):
        result = ""
        for ch in string:
            if ch == "\"":
                result += "\\\""
            elif ch == "\r":
                result += "\\r"
            elif ch == "\n":
                result += "\\n"
            elif ch == "\t":
                result += "\\t"
            elif ch == "\\":
                result += "\\\\"
            else:
                result += ch
        return result
    
    @staticmethod
    def encodeString(string):
        return "\"" + UICmdHandler.encodeStringChars(string) + "\""

    @staticmethod
    def encodeStrings(strings):
        result = "\""
        for i in range(0, len(strings)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeStringChars(strings[i])
        result += "\""
        return result

    @staticmethod
    def encodeInt(x):
        return str(x)

    @staticmethod
    def encodeInts(xs):
        result = "\""
        for i in range(0, len(xs)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeInt(xs[i])
        result += "\""
        return result

    @staticmethod
    def encodeFloat(x):
        return str(x)

    @staticmethod
    def encodePosXs(poss):
        result = "\""
        for i in range(0, len(poss)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeFloat(poss[i].x())
        result += "\""
        return result

    @staticmethod
    def encodePosYs(poss):
        result = "\""
        for i in range(0, len(poss)):
            if i > 0:
                result += "|"
            result += UICmdHandler.encodeFloat(poss[i].y())
        result += "\""
        return result

    def evalCmdWithArgs(self, cmd, encodedArgs):
        code = "binding."
        code += cmd
        code += "("
        code += ", ".join(encodedArgs)
        code += ")"
        return self.scriptEditor.eval(code)

    def dfgDoInstPreset(
        self,
        binding,
        execPath,
        exec_,
        presetPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "instPreset",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(presetPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddGraph(
        self,
        binding,
        execPath,
        exec_,
        title,
        pos
        ):
        return self.evalCmdWithArgs(
            "addGraph",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(title),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddFunc(
        self,
        binding,
        execPath,
        exec_,
        title,
        initialCode,
        pos
        ):
        return self.evalCmdWithArgs(
            "addFunc",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(title),
                UICmdHandler.encodeString(initialCode),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddBackDrop(
        self,
        binding,
        execPath,
        exec_,
        title,
        pos
        ):
        return self.evalCmdWithArgs(
            "addBackDrop",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(title),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddVar(
        self,
        binding,
        execPath,
        exec_,
        desiredNodeName,
        dataType,
        extDep,
        pos
        ):
        return self.evalCmdWithArgs(
            "addVar",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredNodeName),
                UICmdHandler.encodeString(dataType),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddGet(
        self,
        binding,
        execPath,
        exec_,
        desiredNodeName,
        varPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "addGet",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredNodeName),
                UICmdHandler.encodeString(varPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoAddSet(
        self,
        binding,
        execPath,
        exec_,
        desiredNodeName,
        varPath,
        pos
        ):
        return self.evalCmdWithArgs(
            "addSet",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredNodeName),
                UICmdHandler.encodeString(varPath),
                UICmdHandler.encodeFloat(pos.x()),
                UICmdHandler.encodeFloat(pos.y()),
                ]
            )

    def dfgDoMoveNodes(
        self,
        binding,
        execPath,
        exec_,
        nodeNames,
        newTopLeftPoss
        ):
        self.evalCmdWithArgs(
            "moveNodes",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(nodeNames),
                UICmdHandler.encodePosXs(newTopLeftPoss),
                UICmdHandler.encodePosYs(newTopLeftPoss),
                ]
            )

    def dfgDoResizeBackDrop(
        self,
        binding,
        execPath,
        exec_,
        backDropNodeName,
        newTopLeftPos,
        newSize,
        ):
        self.evalCmdWithArgs(
            "resizeBackDrop",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(backDropNodeName),
                UICmdHandler.encodeFloat(newTopLeftPos.x()),
                UICmdHandler.encodeFloat(newTopLeftPos.y()),
                UICmdHandler.encodeFloat(newSize.width()),
                UICmdHandler.encodeFloat(newSize.height()),
                ]
            )

    def dfgDoRemoveNodes(
        self,
        binding,
        execPath,
        exec_,
        nodeNames,
        ):
        self.evalCmdWithArgs(
            "removeNodes",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(nodeNames),
                ]
            )
 
    def dfgDoImplodeNodes(
        self,
        binding,
        execPath,
        exec_,
        nodeNames,
        desiredNodeName
        ):
        return self.evalCmdWithArgs(
            "implodeNodes",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(nodeNames),
                UICmdHandler.encodeString(desiredNodeName),
                ]
            )
 
    def dfgDoExplodeNode(
        self,
        binding,
        execPath,
        exec_,
        nodeName
        ):
        return self.evalCmdWithArgs(
            "explodeNode",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                ]
            )
 
    def dfgDoConnect(
        self,
        binding,
        execPath,
        exec_,
        srcPort,
        dstPort,
        ):
        return self.evalCmdWithArgs(
            "connect",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(srcPort),
                UICmdHandler.encodeString(dstPort),
                ]
            )
 
    def dfgDoDisconnect(
        self,
        binding,
        execPath,
        exec_,
        srcPort,
        dstPort,
        ):
        return self.evalCmdWithArgs(
            "disconnect",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(srcPort),
                UICmdHandler.encodeString(dstPort),
                ]
            )

    def dfgDoAddPort(
        self,
        binding,
        execPath,
        exec_,
        desiredPortName,
        portType,
        typeSpec,
        portToConnect,
        extDep,
        metaData,
        ):
        if portType == self.client.DFG.PortTypes.IO:
            portTypeStr = "io"
        elif portType == self.client.DFG.PortTypes.Out:
            portTypeStr = "out"
        else:
            portTypeStr = "in"
        return self.evalCmdWithArgs(
            "addPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(desiredPortName),
                UICmdHandler.encodeString(portTypeStr),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(portToConnect),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoEditPort(
        self,
        binding,
        execPath,
        exec_,
        oldPortName,
        desiredNewPortName,
        typeSpec,
        extDep,
        metaData,
        ):
        return self.evalCmdWithArgs(
            "editPort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(oldPortName),
                UICmdHandler.encodeString(desiredNewPortName),
                UICmdHandler.encodeString(typeSpec),
                UICmdHandler.encodeString(extDep),
                UICmdHandler.encodeString(metaData),
                ]
            )

    def dfgDoRemovePort(
        self,
        binding,
        execPath,
        exec_,
        portName,
        ):
        return self.evalCmdWithArgs(
            "removePort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(portName),
                ]
            )

    def dfgDoCreatePreset(
        self,
        binding,
        execPath,
        exec_,
        nodeName,
        presetDirPath,
        presetName,
        ):
        return self.evalCmdWithArgs(
            "createPreset",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                UICmdHandler.encodeString(presetDirPath),
                UICmdHandler.encodeString(presetName),
                ]
            )

    def dfgDoSetNodeComment(
        self,
        binding,
        execPath,
        exec_,
        nodeName,
        comment,
        ):
        return self.evalCmdWithArgs(
            "setNodeComment",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(nodeName),
                UICmdHandler.encodeString(comment),
                ]
            )

    def dfgDoSetCode(
        self,
        binding,
        execPath,
        exec_,
        code,
        ):
        return self.evalCmdWithArgs(
            "setCode",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(code),
                ]
            )

    def dfgDoEditNode(
        self,
        binding,
        execPath,
        exec_,
        oldNodeName,
        desiredNewNodeName,
        nodeMetadata,
        execMetadata,
        ):
        return self.evalCmdWithArgs(
            "editNode",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(oldNodeName),
                UICmdHandler.encodeString(desiredNewNodeName),
                UICmdHandler.encodeString(nodeMetadata),
                UICmdHandler.encodeString(execMetadata),
                ]
            )

    def dfgDoRenamePort(
        self,
        binding,
        execPath,
        exec_,
        oldPortName,
        desiredNewPortName,
        ):
        return self.evalCmdWithArgs(
            "renamePort",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(oldPortName),
                UICmdHandler.encodeString(desiredNewPortName),
                ]
            )

    def dfgDoPaste(
        self,
        binding,
        execPath,
        exec_,
        json,
        cursorPos,
        ):
        return self.evalCmdWithArgs(
            "paste",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(json),
                UICmdHandler.encodeFloat(cursorPos.x()),
                UICmdHandler.encodeFloat(cursorPos.y()),
                ]
            )

    def dfgDoSetArgValue(
        self,
        binding,
        argName,
        value,
        ):
        return self.evalCmdWithArgs(
            "setArgValue",
            [
                UICmdHandler.encodeString(argName),
                UICmdHandler.encodeString(value.getTypeNameStr()),
                UICmdHandler.encodeString(value.getJSONStr()),
                ]
            )

    def dfgDoSetPortDefaultValue(
        self,
        binding,
        execPath,
        exec_,
        portPath,
        value,
        ):
        return self.evalCmdWithArgs(
            "setPortDefaultValue",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(portPath),
                UICmdHandler.encodeString(value.getTypeNameStr()),
                UICmdHandler.encodeString(value.getJSONStr()),
                ]
            )

    def dfgDoSetRefVarPath(
        self,
        binding,
        execPath,
        exec_,
        refName,
        varPath,
        ):
        return self.evalCmdWithArgs(
            "setRefVarPath",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeString(refName),
                UICmdHandler.encodeString(varPath),
                ]
            )

    def dfgDoReorderPorts(
        self,
        binding,
        execPath,
        exec_,
        indices,
        ):
        return self.evalCmdWithArgs(
            "reorderPorts",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeInts(indices),
                ]
            )

    def dfgDoSetExtDeps(
        self,
        binding,
        execPath,
        exec_,
        extDeps,
        ):
        return self.evalCmdWithArgs(
            "setExtDeps",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(extDeps),
                ]
            )

    def dfgDoSplitFromPreset(
        self,
        binding,
        execPath,
        exec_,
        ):
        return self.evalCmdWithArgs(
            "splitFromPreset",
            [
                UICmdHandler.encodeString(execPath),
                ]
            )

class MainWindow(DFG.DFGMainWindow):
    contentChanged = QtCore.Signal()

    defaultFrameIn = 1
    defaultFrameOut = 50
    autosaveIntervalSecs = 30

    def __init__(self, settings, unguarded):
        super(MainWindow, self).__init__()

        self.settings = settings
        DFG.DFGWidget.setSettings(settings)

        self.viewport = None
        self.dfgWidget = None
        self.currentGraph = None

        self.config = DFG.DFGConfig()

        self.autosaveFilename = os.path.join(fabricDir, 'autosave')
        if not os.path.exists(self.autosaveFilename):
            os.makedirs(self.autosaveFilename)
        autosaveBasename = 'autosave.' + str(os.getpid()) + '.canvas'
        self.autosaveFilename = os.path.join(self.autosaveFilename,
                                             autosaveBasename)
        print 'Will autosave to ' + self.autosaveFilename + ' every ' + str(
            MainWindow.autosaveIntervalSecs) + ' seconds'

        self.autosaveTimer = QtCore.QTimer()
        self.autosaveTimer.timeout.connect(self.autosave)
        self.autosaveTimer.start(MainWindow.autosaveIntervalSecs * 1000)

        self.windowTitle = 'Fabric Engine'
        self.lastFileName = ''
        self.onFileNameChanged('')

        statusBar = QtGui.QStatusBar(self)
        self.fpsLabel = QtGui.QLabel(statusBar)
        statusBar.addPermanentWidget(self.fpsLabel)
        self.setStatusBar(statusBar)
        statusBar.show()

        self.fpsTimer = QtCore.QTimer()
        self.fpsTimer.setInterval(1000)
        self.fpsTimer.timeout.connect(self.updateFPS)
        self.fpsTimer.start()

        clientOpts = {
            'guarded': not unguarded,
            'reportCallback': self.reportCallback
            }
        client = Core.createClient(clientOpts)
        #options.licenseType = FabricCore::ClientLicenseType_Interactive
        client.loadExtension('Math')
        client.loadExtension('Parameters')
        client.loadExtension('Util')
        client.setStatusCallback(self.statusCallback)
        self.client = client

        self.qUndoStack = QtGui.QUndoStack()

        astManager = KLASTManager(client)

        self.evalContext = client.RT.types.EvalContext.create()
        self.evalContext = self.evalContext.getInstance('EvalContext')
        self.evalContext.host = 'Canvas'
        self.evalContext.graph = ''

        self.host = client.getDFGHost()
        binding = self.host.createBindingToNewGraph()
        self.lastSavedBindingVersion = binding.getVersion()
        self.lastAutosaveBindingVersion = self.lastSavedBindingVersion

        graph = binding.getExec()

        glFormat = QtOpenGL.QGLFormat()
        glFormat.setDoubleBuffer(True)
        glFormat.setDepth(True)
        glFormat.setAlpha(True)
        glFormat.setSampleBuffers(True)
        glFormat.setSamples(4)

        self.viewport = Viewports.GLViewportWidget(
            client, self.config.defaultWindowColor, glFormat, None, None)
        self.setCentralWidget(self.viewport)

        self.scriptEditor = ScriptEditor(client, binding, self.qUndoStack)
        self.dfguiCommandHandler = UICmdHandler(client, self.scriptEditor)

        self.dfgWidget = DFG.DFGWidget(None, client, self.host,
                                       binding, '', graph, astManager,
                                       self.dfguiCommandHandler, self.config)

        self.contentChanged.connect(self.viewport.redraw)
        self.viewport.portManipulationRequested.connect(
            self.onPortManipulationRequested)

        dockFeatures = QtGui.QDockWidget.DockWidgetMovable | QtGui.QDockWidget.DockWidgetFloatable | QtGui.QDockWidget.DockWidgetClosable

        dfgDock = QtGui.QDockWidget('Canvas Graph', self)
        dfgDock.setObjectName('Canvas Graph')
        dfgDock.setFeatures(dockFeatures)
        dfgDock.setWidget(self.dfgWidget)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, dfgDock,
                           QtCore.Qt.Vertical)

        scriptEditorDock = QtGui.QDockWidget('Script Editor', self)
        scriptEditorDock.setObjectName('Script Editor')
        scriptEditorDock.setFeatures(dockFeatures)
        scriptEditorDock.setWidget(self.scriptEditor)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, scriptEditorDock,
                           QtCore.Qt.Vertical)

        self.timeLinePortIndex = -1
        self.timeLinePortPath = None
        self.timeLine = Viewports.TimeLineWidget()
        self.timeLine.setTimeRange(MainWindow.defaultFrameIn,
                                   MainWindow.defaultFrameOut)
        self.timeLine.updateTime(1)
        timeLineDock = QtGui.QDockWidget("TimeLine", self)
        timeLineDock.setObjectName("TimeLine")
        timeLineDock.setFeatures(dockFeatures)
        timeLineDock.setWidget(self.timeLine)
        self.addDockWidget(QtCore.Qt.BottomDockWidgetArea, timeLineDock,
                           QtCore.Qt.Vertical)

        treeWidget = DFG.PresetTreeWidget(self.dfgWidget.getDFGController(),
                                          self.config, True, False, True)
        treeDock = QtGui.QDockWidget("Explorer", self)
        treeDock.setObjectName("Explorer")
        treeDock.setFeatures(dockFeatures)
        treeDock.setWidget(treeWidget)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, treeDock)

        self.dfgWidget.newPresetSaved.connect(treeWidget.refresh)

        self.dfgValueEditor = DFG.DFGValueEditor(
            self.dfgWidget.getDFGController(), self.config)
        dfgValueEditorDockWidget = QtGui.QDockWidget("Value Editor", self)
        dfgValueEditorDockWidget.setObjectName("Values")
        dfgValueEditorDockWidget.setFeatures(dockFeatures)
        dfgValueEditorDockWidget.setWidget(self.dfgValueEditor)
        self.addDockWidget(QtCore.Qt.RightDockWidgetArea,
                           dfgValueEditorDockWidget)

        self.logWidget = DFG.DFGLogWidget(self.config)
        logDockWidget = QtGui.QDockWidget("Log Messages", self)
        logDockWidget.setObjectName("Log")
        logDockWidget.setFeatures(dockFeatures)
        logDockWidget.setWidget(self.logWidget)
        logDockWidget.hide()
        self.addDockWidget(QtCore.Qt.TopDockWidgetArea, logDockWidget,
                           QtCore.Qt.Vertical)

        self.qUndoView = QtGui.QUndoView(self.qUndoStack)
        self.qUndoView.setEmptyLabel("New Graph")
        undoDockWidget = QtGui.QDockWidget("History", self)
        undoDockWidget.setObjectName("History")
        undoDockWidget.setFeatures(dockFeatures)
        undoDockWidget.setWidget(self.qUndoView)
        undoDockWidget.hide()
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, undoDockWidget)

        self.newGraphAction = None
        self.loadGraphAction = None
        self.saveGraphAction = None
        self.saveGraphAsAction = None
        self.quitAction = None
        self.manipAction = None
        self.setGridVisibleAction = None
        self.resetCameraAction = None
        self.clearLogAction = None
        self.blockCompilationsAction = None

        controller = self.dfgWidget.getDFGController()
        controller.varsChanged.connect(treeWidget.refresh)
        controller.argsChanged.connect(self.onStructureChanged)
        controller.argValuesChanged.connect(self.onValueChanged)
        controller.defaultValuesChanged.connect(self.onValueChanged)
        self.dfgWidget.nodeInspectRequested.connect(
            self.onNodeInspectRequested)
        controller.dirty.connect(self.onDirty)
        controller.bindingChanged.connect(self.dfgValueEditor.setBinding)
        controller.nodeRemoved.connect(self.dfgValueEditor.onNodeRemoved)
        tabSearchWidget = self.dfgWidget.getTabSearchWidget()
        tabSearchWidget.enabled.connect(self.enableShortCuts)
        self.timeLine.frameChanged.connect(self.onFrameChanged)
        self.dfgWidget.onGraphSet.connect(self.onGraphSet)

        self.restoreGeometry(settings.value("mainWindow/geometry"))
        self.restoreState(settings.value("mainWindow/state"))

        self.dfgWidget.additionalMenuActionsRequested.connect(self.onAdditionalMenuActionsRequested)

        self.dfgWidget.populateMenuBar(self.menuBar())
        windowMenu = self.menuBar().addMenu("&Window")

        toggleAction = dfgDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_1)
        windowMenu.addAction(toggleAction)
        toggleAction = treeDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_2)
        windowMenu.addAction(toggleAction)
        toggleAction = dfgValueEditorDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_3)
        windowMenu.addAction(toggleAction)
        toggleAction = timeLineDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_4)
        windowMenu.addAction(toggleAction)
        windowMenu.addSeparator()
        toggleAction = undoDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_5)
        windowMenu.addAction(toggleAction)
        toggleAction = logDockWidget.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_6)
        windowMenu.addAction(toggleAction)
        toggleAction = scriptEditorDock.toggleViewAction()
        toggleAction.setShortcut(QtCore.Qt.CTRL + QtCore.Qt.Key_7)
        windowMenu.addAction(toggleAction)

        self.onFrameChanged(self.timeLine.getTime())
        self.onGraphSet(self.dfgWidget.getUIGraph())
        self.onSidePanelInspectRequested()

        self.installEventFilter(MainWindowEventFilter(self))

    def reportCallback(self, source, level, line):
        if self.dfgWidget:
            self.dfgWidget.getDFGController().log(line)

        if source == Core.ReportSource.User or level == Core.ReportLevel.Error or 'Ignoring' in line:
            sys.stdout.write(line + "\n")
        else:
            sys.stderr.write(line + "\n")

    def statusCallback(self, target, data):
        if target == "licensing":
            try:
                FabricUI.HandleLicenseData(self, self.client, data, True)
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))

    def onPortManipulationRequested(self, portName):
        try:
            controller = self.dfgWidget.getDFGController()
            binding = controller.getBinding()
            dfgExec = binding.getExec()
            portResolvedType = dfgExec.getExecPortResolvedType(portName)
            value = self.viewport.getManipTool().getLastManipVal()
            if portResolvedType == 'Xfo':
                pass
            elif portResolvedType == 'Mat44':
                value = value.toMat44('Mat44')
            elif portResolvedType == 'Vec3':
                value = value.tr
            elif portResolvedType == 'Quat':
                value = value.ori
            else:
                message = "Port '" + portName
                message += "'to be driven has unsupported type '"
                message += portResolvedType.data()
                message += "'."
                self.dfgWidget.getDFGController().logError(message)
                return
            controller.cmdSetArgValue(portName, value)
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def onDirty(self):
        self.dfgWidget.getDFGController().execute()
        self.onValueChanged()
        self.contentChanged.emit()

    def onValueChanged(self):
        try:
            self.dfgValueEditor.updateOutputs()
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def loadGraph(self, filePath):
        self.timeLine.pause()
        self.timeLinePortPath = None

        try:
            dfgController = self.dfgWidget.getDFGController()
            binding = dfgController.getBinding()
            binding.deallocValues()

            self.dfgValueEditor.clear()
            self.host.flushUndoRedo()
            self.qUndoStack.clear()
            self.qUndoView.setEmptyLabel("Load Graph")
            self.viewport.clearInlineDrawing()

            QtCore.QCoreApplication.processEvents()

            jsonVal = open(filePath, 'rb').read()
            binding = self.host.createBindingFromJSON(jsonVal)
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            dfgController.setBindingExec(binding, "", dfgExec)
            self.onSidePanelInspectRequested()

            dfgController.checkErrors()
            self.evalContext.currentFilePath = filePath
            dfgController.bindUnboundRTVals()
            dfgController.execute()

            tl_start = dfgExec.getMetadata("timeline_start")
            tl_end = dfgExec.getMetadata("timeline_end")
            tl_loopMode = dfgExec.getMetadata("timeline_loopMode")
            tl_simulationMode = dfgExec.getMetadata("timeline_simMode")
            tl_current = dfgExec.getMetadata("timeline_current")

            if len(tl_start) > 0 and len(tl_end) > 0:
                self.timeLine.setTimeRange(int(tl_start), int(tl_end))
            else:
                self.timeLine.setTimeRange(MainWindow.defaultFrameIn,
                                           MainWindow.defaultFrameOut)

            if len(tl_loopMode) > 0:
                self.timeLine.setLoopMode(int(tl_loopMode))
            else:
                self.timeLine.setLoopMode(1)

            if len(tl_simulationMode) > 0:
                self.timeLine.setSimulationMode(int(tl_simulationMode))
            else:
                self.timeLine.setSimulationMode(0)

            camera_mat44 = dfgExec.getMetadata("camera_mat44")
            camera_focalDistance = dfgExec.getMetadata("camera_focalDistance")
            if len(camera_mat44) > 0 and len(camera_focalDistance) > 0:
                try:
                    mat44 = self.client.RT.constructRTValFromJSON('Mat44', camera_mat44)
                    focalDistance = self.client.RT.constructRTValFromJSON('Float32',
                        camera_focalDistance)

                    camera = self.wrapRTVal(self.viewport.getCamera())

                    camera.setFromMat44('', mat44)
                    camera.setFocalDistance("", focalDistance)
                except Exception as e:
                    sys.stderr.write("Exception: " + str(e) + "\n")

            self.contentChanged.emit()
            self.onStructureChanged()

            self.onFileNameChanged(filePath)

            # then set it to the current value if we still have it.
            # this will ensure that sim mode scenes will play correctly.
            if len(tl_current) > 0:
                self.timeLine.updateTime(int(tl_current), True)
            else:
                self.timeLine.updateTime(MainWindow.defaultFrameIn, True)

            self.viewport.update()

        except Exception as e:
            sys.stderr.write("Exception: " + str(e) + "\n")

        self.lastFileName = filePath

    def onSaveGraph(self):
        self.saveGraph(False)

    def onSaveGraphAs(self):
        self.saveGraph(True)

    def closeEvent(self, event):
        if not self.checkUnsavedChanges():
            event.ignore()
            return

        self.viewport.setManipulationActive(False)
        self.settings.setValue("mainWindow/geometry", self.saveGeometry())
        self.settings.setValue("mainWindow/state", self.saveState())

        QtGui.QMainWindow.closeEvent(self, event)

        if os.path.exists(self.autosaveFilename):
            os.remove(self.autosaveFilename)

    def checkUnsavedChanges(self):
        binding = self.dfgWidget.getDFGController().getBinding()

        if binding.getVersion() != self.lastSavedBindingVersion:
            msgBox = QtGui.QMessageBox()
            msgBox.setText("Do you want to save your changes?")
            msgBox.setInformativeText(
                "Your changes will be lost if you don't save them.")
            msgBox.setStandardButtons(QtGui.QMessageBox.Save |
                                      QtGui.QMessageBox.Discard |
                                      QtGui.QMessageBox.Cancel)
            msgBox.setDefaultButton(QtGui.QMessageBox.Save)
            result = msgBox.exec_()
            if result == QtGui.QMessageBox.Discard:
                return True
            elif result == QtGui.QMessageBox.Cancel:
                return False
            else:
                return self.saveGraph(False)
        return True

    def onStructureChanged(self):
        if self.dfgWidget.getDFGController().isViewingRootGraph():
            self.timeLinePortPath = None
            try:
                graph = self.dfgWidget.getDFGController().getExec()
                portCount = graph.getExecPortCount()
                for i in range(0, portCount):
                    if graph.getExecPortType(
                        i) == self.client.DFG.PortTypes.Out:
                        continue
                    portName = graph.getExecPortName(i)
                    if portName != "timeline":
                        continue
                    if not graph.isExecPortResolvedType(
                        i, "SInt32") and not graph.isExecPortResolvedType(
                            i, "UInt32") and not graph.isExecPortResolvedType(
                                i,
                                "Float32") and not graph.isExecPortResolvedType(
                                    i, "Float64"):
                        continue
                    self.timeLinePortPath = "timeline"
                    self.timeLinePortIndex = i
                    break
            except Exception as e:
                self.dfgWidget.getDFGController().logError(str(e))

    def onNodeInspectRequested(self, node):
        if node.isBackDropNode():
            return

        dfgController = self.dfgWidget.getDFGController()
        self.dfgValueEditor.setNode(dfgController.getBinding(),
                                    dfgController.getExecPath(),
                                    dfgController.getExec(), node.name())

    def onNodeEditRequested(self, node):
        self.dfgWidget.maybeEditNode(node)

    def onFrameChanged(self, frame):
        try:
            self.evalContext.time = frame
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

        if not self.timeLinePortPath:
            return

        try:
            binding = self.dfgWidget.getDFGController().getBinding()
            dfgExec = binding.getExec()
            if dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                              "SInt32"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.SInt32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                "UInt32"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.UInt32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                "Float32"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.Float32(frame), False)
            elif dfgExec.isExecPortResolvedType(self.timeLinePortIndex,
                                                "Float64"):
                binding.setArgValue(self.timeLinePortPath,
                                    self.client.RT.types.Float64(frame), False)
        except Exception as e:
            self.dfgWidget.getDFGController().logError(str(e))

    def updateFPS(self):
        if not self.viewport:
            return

        caption = str(round(self.viewport.fps(), 2)) + " FPS"
        self.fpsLabel.setText(caption)

    def autosave(self):
        # [andrew 20150909] can happen if this triggers while the licensing
        # dialogs are up
        if not self.dfgWidget or not self.dfgWidget.getDFGController():
            return

        binding = self.dfgWidget.getDFGController().getBinding()
        if binding:
            bindingVersion = binding.getVersion()
            if bindingVersion != self.lastAutosaveBindingVersion:
                tmpAutosaveFilename = self.autosaveFilename
                tmpAutosaveFilename += ".tmp"

                if self.performSave(binding, tmpAutosaveFilename):
                    os.rename(tmpAutosaveFilename, self.autosaveFilename)
                    self.lastAutosaveBindingVersion = bindingVersion

    def onSidePanelInspectRequested(self):
        dfgController = self.dfgWidget.getDFGController()

        if dfgController.isViewingRootGraph():
            self.dfgValueEditor.setBinding(dfgController.getBinding())
        else:
            self.dfgValueEditor.clear()

    def onNewGraph(self):
        self.timeLine.pause()

        if not self.checkUnsavedChanges():
            return

        self.lastFileName = ""

        try:
            dfgController = self.dfgWidget.getDFGController()

            binding = dfgController.getBinding()
            binding.deallocValues()

            self.dfgValueEditor.clear()

            self.host.flushUndoRedo()
            self.qUndoStack.clear()
            self.viewport.clearInlineDrawing()
            QtCore.QCoreApplication.processEvents()

            # Note: the previous binding is no longer functional
            #             create the new one before resetting the timeline options

            binding = self.host.createBindingToNewGraph()
            self.lastSavedBindingVersion = binding.getVersion()
            dfgExec = binding.getExec()
            self.timeLinePortPath = None

            dfgController.setBindingExec(binding, '', dfgExec)

            self.timeLine.setTimeRange(MainWindow.defaultFrameIn,
                                       MainWindow.defaultFrameOut)
            self.timeLine.setLoopMode(1)
            self.timeLine.setSimulationMode(0)
            self.timeLine.updateTime(MainWindow.defaultFrameIn, True)

            QtCore.QCoreApplication.processEvents()
            self.qUndoView.setEmptyLabel("New Graph")

            self.onSidePanelInspectRequested()

            self.contentChanged.emit()
            self.onStructureChanged()

            self.onFileNameChanged('')

            self.viewport.update()

        except Exception as e:
            print 'Exception: ' + str(e)
            raise e

    def onLoadGraph(self):
        self.timeLine.pause()

        if not self.checkUnsavedChanges():
            return

        lastPresetFolder = self.settings.value(
            "mainWindow/lastPresetFolder").toString()
        filePath = QtGui.QFileDialog.getOpenFileName(
            self, "Load graph", lastPresetFolder, "*.canvas")
        if len(filePath.length):
            folder = QtCore.QDir(filePath)
            folder.cdUp()
            self.settings.setValue("mainWindow/lastPresetFolder",
                                   folder.path())
            self.loadGraph(filePath)

    # [andrew 20151027] FIXME Core.CAPI normally takes care of this
    def wrapRTVal(self, rtVal):
        return Core.Util.rtValToPyObject(self.client._client.getContext(), rtVal)

    def performSave(self, binding, filePath):
        graph = binding.getExec()

        graph.setMetadata("timeline_start", str(self.timeLine.getRangeStart()),
                          False)
        graph.setMetadata("timeline_end", str(self.timeLine.getRangeEnd()),
                          False)
        graph.setMetadata("timeline_current", str(self.timeLine.getTime()),
                          False)
        graph.setMetadata("timeline_loopMode", str(self.timeLine.loopMode()),
                          False)
        graph.setMetadata("timeline_simMode",
                          str(self.timeLine.simulationMode()), False)

        try:
            camera = self.wrapRTVal(self.viewport.getCamera())
            mat44 = camera.getMat44('Mat44')
            focalDistance = camera.getFocalDistance('Float32')

            graph.setMetadata("camera_mat44",
                              str(mat44.getJSON()), False)
            graph.setMetadata("camera_focalDistance",
                              str(focalDistance.getJSON()),
                              False)
        except Exception as e:
            print 'Exception: ' + str(e)
            raise e

        try:
            jsonVal = binding.exportJSON()
            jsonFile = open(filePath, "wb")
            if jsonFile:
                jsonFile.write(jsonVal)
                jsonFile.close()
        except Exception as e:
            print 'Exception: ' + str(e)
            return False

        return True

    def saveGraph(self, saveAs):
        self.timeLine.pause()

        filePath = self.lastFileName
        if len(filePath) == 0 or saveAs:
            lastPresetFolder = self.settings.value(
                "mainWindow/lastPresetFolder").toString()
            if len(self.lastFileName) > 0:
                filePath = self.lastFileName
                if filePath.lower().endswith('.canvas'):
                    filePath = filePath[0:-7]
            else:
                filePath = lastPresetFolder

            filePath = QtGui.QFileDialog.getSaveFileName(self, "Save graph",
                                                         filePath, "*.canvas")
            if len(filePath) == 0:
                return False
            if filePath.lower().endswith(".canvas.canvas"):
                filePath = filePath[0:-7]
            elif not filePath.lower().endswith(".canvas"):
                filePath += ".canvas"

        folder = QtCore.QDir(filePath)
        folder.cdUp()
        self.settings.setValue("mainWindow/lastPresetFolder", folder.path())

        binding = self.dfgWidget.getDFGController().getBinding()

        if self.performSave(binding, filePath):
            self.evalContext.setMember("currentFilePath",
                                       self.client.RT.types.String(filePath))

        self.lastFileName = filePath

        self.onFileNameChanged(filePath)

        self.lastSavedBindingVersion = binding.getVersion()

        return True

    def setBlockCompilations(self, blockCompilations):
        dfgController = self.dfgWidget.getDFGController()
        dfgController.setBlockCompilations(blockCompilations)

    def onFileNameChanged(self, fileName):
        if not fileName:
            self.setWindowTitle(self.windowTitle)
        else:
            self.setWindowTitle(self.windowTitle + " - " + fileName)

    def enableShortCuts(self, enabled):
        if self.newGraphAction:
            self.newGraphAction.blockSignals(enabled)
        if self.loadGraphAction:
            self.loadGraphAction.blockSignals(enabled)
        if self.saveGraphAction:
            self.saveGraphAction.blockSignals(enabled)
        if self.saveGraphAsAction:
            self.saveGraphAsAction.blockSignals(enabled)
        if self.quitAction:
            self.quitAction.blockSignals(enabled)
        if self.manipAction:
            self.manipAction.blockSignals(enabled)
        if self.setGridVisibleAction:
            self.setGridVisibleAction.blockSignals(enabled)
        if self.resetCameraAction:
            self.resetCameraAction.blockSignals(enabled)
        if self.clearLogAction:
            self.clearLogAction.blockSignals(enabled)
        if self.blockCompilationsAction:
            self.blockCompilationsAction.blockSignals(enabled)

    def onAdditionalMenuActionsRequested(self, name, menu, prefix):
        if name == 'File':
            if prefix:
                self.newGraphAction = QtGui.QAction('New Graph', menu)
                self.newGraphAction.setShortcut(QtGui.QKeySequence.New)
                self.loadGraphAction = QtGui.QAction('Load Graph...', menu)
                self.loadGraphAction.setShortcut(QtGui.QKeySequence.Open)
                self.saveGraphAction = QtGui.QAction('Save Graph', menu)
                self.saveGraphAction.setShortcut(QtGui.QKeySequence.Save)
                self.saveGraphAsAction = QtGui.QAction('Save Graph As...', menu)
                self.saveGraphAsAction.setShortcut(QtGui.QKeySequence.SaveAs)

                menu.addAction(self.newGraphAction)
                menu.addAction(self.loadGraphAction)
                menu.addAction(self.saveGraphAction)
                menu.addAction(self.saveGraphAsAction)

                self.newGraphAction.triggered.connect(self.onNewGraph)
                self.loadGraphAction.triggered.connect(self.onLoadGraph)
                self.saveGraphAction.triggered.connect(self.onSaveGraph)
                self.saveGraphAsAction.triggered.connect(self.onSaveGraphAs)
            else:
                menu.addSeparator()
                self.quitAction = QtGui.QAction('Quit', menu)
                self.quitAction.setShortcut(QtGui.QKeySequence.Quit)
                menu.addAction(self.quitAction)

                self.quitAction.triggered.connect(self.close)
        elif name == 'Edit':
            if prefix:
                undoAction = QtGui.QAction("Undo", self)
                def onUndo():
                    self.scriptEditor.eval("undo()")
                undoAction.triggered.connect(onUndo)
                undoAction.setEnabled(self.qUndoStack.canUndo())
                self.qUndoStack.canUndoChanged.connect(undoAction.setEnabled)
                undoAction.setShortcut(QtGui.QKeySequence.Undo)
                menu.addAction(undoAction)
                redoAction = QtGui.QAction("Redo", self)
                def onRedo():
                    self.scriptEditor.eval("redo()")
                redoAction.triggered.connect(onRedo)
                redoAction.setEnabled(self.qUndoStack.canRedo())
                self.qUndoStack.canRedoChanged.connect(redoAction.setEnabled)
                redoAction.setShortcut(QtGui.QKeySequence.Redo)
                menu.addAction(redoAction)
            else:
                menu.addSeparator()
                self.manipAction = QtGui.QAction(
                    DFG.DFGHotkeys.TOGGLE_MANIPULATION, self.viewport)
                self.manipAction.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_Q))
                self.manipAction.setShortcutContext(
                    QtCore.Qt.WidgetWithChildrenShortcut)
                self.manipAction.setCheckable(True)
                self.manipAction.setChecked(
                    self.viewport.isManipulationActive())
                self.manipAction.triggered.connect(
                    self.viewport.toggleManipulation)
                self.viewport.addAction(self.manipAction)
                menu.addAction(self.manipAction)
        elif name == 'View':
            if prefix:
                self.setGridVisibleAction = QtGui.QAction('&Display Grid', None)
                self.setGridVisibleAction.setShortcut(
                    QtCore.Qt.CTRL + QtCore.Qt.Key_G)
                self.setGridVisibleAction.setCheckable(True)
                self.setGridVisibleAction.setChecked(
                    self.viewport.isStageVisible())
                self.setGridVisibleAction.toggled.connect(
                    self.viewport.setStageVisible)

                self.resetCameraAction = QtGui.QAction('&Reset Camera',
                                                       self.viewport)
                self.resetCameraAction.setShortcut(QtGui.QKeySequence(QtCore.Qt.Key_R))
                self.resetCameraAction.setShortcutContext(
                    QtCore.Qt.WidgetWithChildrenShortcut)
                self.resetCameraAction.triggered.connect(
                    self.viewport.resetCamera)
                self.viewport.addAction(self.resetCameraAction)

                self.clearLogAction = QtGui.QAction('&Clear Log Messages', None)
                self.clearLogAction.triggered.connect(self.logWidget.clear)

                self.blockCompilationsAction = QtGui.QAction(
                    '&Block compilations', None)
                self.blockCompilationsAction.setCheckable(True)
                self.blockCompilationsAction.setChecked(False)
                self.blockCompilationsAction.triggered.connect(
                    self.setBlockCompilations)

                menu.addAction(self.setGridVisibleAction)
                menu.addSeparator()
                menu.addAction(self.resetCameraAction)
                menu.addSeparator()
                menu.addAction(self.clearLogAction)
                menu.addSeparator()
                menu.addAction(self.blockCompilationsAction)

    def onHotkeyPressed(self, key, modifiers, hotkey):
        if hotkey == DFG.DFGHotkeys.EXECUTE:
            self.onDirty()
        elif hotkey == DFG.DFGHotkeys.NEW_SCENE:
            self.onNewGraph()
        elif hotkey == DFG.DFGHotkeys.OPEN_SCENE:
            self.onLoadGraph()
        elif hotkey == DFG.DFGHotkeys.SAVE_SCENE:
            self.saveGraph(False)
        elif hotkey == DFG.DFGHotkeys.TOGGLE_MANIPULATION:
            # Make sure we use the Action path, so menu's "checked" state is updated
            if self.manipAction:
                self.manipAction.trigger()
        else:
            self.dfgWidget.onHotkeyPressed(key, modifiers, hotkey)

    def onGraphSet(self, graph):
        if graph != self.currentGraph:
            graph = self.dfgWidget.getUIGraph()
            graph.defineHotkey(QtCore.Qt.Key_Delete, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.DELETE_1)
            graph.defineHotkey(QtCore.Qt.Key_Backspace, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.DELETE_2)
            graph.defineHotkey(QtCore.Qt.Key_F5, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.EXECUTE)
            graph.defineHotkey(QtCore.Qt.Key_F, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.FRAME_SELECTED)
            graph.defineHotkey(QtCore.Qt.Key_A, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.FRAME_ALL)
            graph.defineHotkey(QtCore.Qt.Key_Tab, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.TAB_SEARCH)
            graph.defineHotkey(QtCore.Qt.Key_A, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.SELECT_ALL)
            graph.defineHotkey(QtCore.Qt.Key_C, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.COPY)
            graph.defineHotkey(QtCore.Qt.Key_V, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.PASTE)
            graph.defineHotkey(QtCore.Qt.Key_X, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.CUT)
            graph.defineHotkey(QtCore.Qt.Key_N, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.NEW_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_O, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.OPEN_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_S, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.SAVE_SCENE)
            graph.defineHotkey(QtCore.Qt.Key_F2, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.EDIT_PROPERTIES)
            graph.defineHotkey(QtCore.Qt.Key_R, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.RELAX_NODES)
            graph.defineHotkey(QtCore.Qt.Key_Q, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.TOGGLE_MANIPULATION)
            graph.defineHotkey(QtCore.Qt.Key_0, QtCore.Qt.ControlModifier,
                               DFG.DFGHotkeys.RESET_ZOOM)
            graph.defineHotkey(QtCore.Qt.Key_1, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.COLLAPSE_LEVEL_1)
            graph.defineHotkey(QtCore.Qt.Key_2, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.COLLAPSE_LEVEL_2)
            graph.defineHotkey(QtCore.Qt.Key_3, QtCore.Qt.NoModifier,
                               DFG.DFGHotkeys.COLLAPSE_LEVEL_3)

            graph.nodeInspectRequested.connect(self.onNodeInspectRequested)
            graph.nodeEditRequested.connect(self.onNodeEditRequested)
            graph.sidePanelInspectRequested.connect(
                self.onSidePanelInspectRequested)

            self.currentGraph = graph


app = QtGui.QApplication([])
app.setOrganizationName('Fabric Software Inc')
app.setApplicationName('Fabric Canvas Standalone')
app.setApplicationVersion('2.0.0')
app.setStyle( FabricStyle() )

fabricDir = os.environ.get('FABRIC_DIR', None)
if fabricDir:
    logoPath = os.path.join(fabricDir, 'Resources', 'fe_logo.png')
    app.setWindowIcon(QtGui.QIcon(logoPath))

opt_parser = optparse.OptionParser(usage='Usage: %prog [options] [graph]')
opt_parser.add_option('-u', '--unguarded',
                      action='store_true',
                      dest='unguarded',
                      help='compile KL code in unguarded mode')
opt_parser.add_option('-e', '--exec',
                      action='store',
                      dest='script',
                      help='execute Python script on startup')
(opts, args) = opt_parser.parse_args()

unguarded = opts.unguarded is True

settings = QtCore.QSettings()
mainWin = MainWindow(settings, unguarded)
mainWin.show()

for arg in args:
    mainWin.loadGraph(arg)

if opts.script:
    with open(opts.script, "r") as f:
        mainWin.scriptEditor.exec_(f.read())

app.exec_()
