from FabricEngine.FabricUI import DFG

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
        srcPorts,
        dstPorts,
        ):
        return self.evalCmdWithArgs(
            "disconnect",
            [
                UICmdHandler.encodeString(execPath),
                UICmdHandler.encodeStrings(srcPorts),
                UICmdHandler.encodeStrings(dstPorts),
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
