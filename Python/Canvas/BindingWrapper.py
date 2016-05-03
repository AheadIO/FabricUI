from PySide import QtCore, QtGui
from FabricEngine.FabricUI import DFG
from FabricEngine.Canvas.RTValEncoderDecoder import RTValEncoderDecoder

class UndoCmd(QtGui.QUndoCommand):
    def __init__(self, cmd):
        QtGui.QUndoCommand.__init__(self)
        self.cmd = cmd
        self.done = False
    def redo(self):
        if not self.done:
            self.done = True
            DFG.UndoCmd_Python.TryCatchDoit(self.cmd)
            self.setText(self.cmd.getDesc())
        else:
            DFG.UndoCmd_Python.TryCatchRedo(self.cmd)
    def undo(self):
        DFG.UndoCmd_Python.TryCatchUndo(self.cmd)

def InvokeCmd(cmd, qUndoStack):
    undoCmd = UndoCmd(cmd)
    qUndoStack.push(undoCmd)

class BindingWrapper:

    def __init__(self, client, binding, qUndoStack):
        self.client = client
        self.binding = binding
        self.qUndoStack = qUndoStack
        self.rtvalEncoderDecoder = RTValEncoderDecoder(self.client)

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
        cmd = DFG.DFGUICmd_ExplodeNode(
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
        packedSrcPorts,
        packedDstPorts,
        ):
        rootExec = self.binding.getExec()
        exec_ = rootExec.getSubExec(execPath)
        srcPorts = BindingWrapper.splitNames(packedSrcPorts)
        dstPorts = BindingWrapper.splitNames(packedDstPorts)
        cmd = DFG.DFGUICmd_Disconnect(
            self.binding,
            execPath,
            exec_,
            srcPorts,
            dstPorts
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
        value = self.rtvalEncoderDecoder.getFromString(typeName, valueJSON)
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
        value = self.rtvalEncoderDecoder.getFromString(typeName, valueJSON)
        cmd = DFG.DFGUICmd_SetPortDefaultValue(
            self.binding,
            execPath,
            exec_,
            portPath,
            value,
            )
        InvokeCmd(cmd, self.qUndoStack)

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

    def dismissLoadDiags(
        self,
        packedIndices,
        ):
        rootExec = self.binding.getExec()
        indices = BindingWrapper.splitInts(packedIndices)
        cmd = DFG.DFGUICmd_DismissLoadDiags(
            self.binding,
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
