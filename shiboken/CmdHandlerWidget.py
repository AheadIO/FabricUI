
from FabricEngine import Core, FabricUI
from FabricEngine.FabricUI import DFG
  
def InvokeCmd(cmd, qUndoStack):
   undoCmd = DFG.UndoCmd_Python(cmd)
   qUndoStack.push(undoCmd)

class CmdHandlerWidget(DFG.DFGUICmdHandler_Python):

  def __init__(self, client, scriptEditor):
    super(CmdHandlerWidget, self).__init__()
    self.client = client
    self.scriptEditor = scriptEditor

  @staticmethod
  def encodeStringChars(string):
    result = ""
    for ch in string:
      if ch == "\"": result += "\\\""
      elif ch == "\r": result += "\\r"
      elif ch == "\n": result += "\\n"
      elif ch == "\t": result += "\\t"
      elif ch == "\\": result += "\\\\"
      else: result += ch
    return result
  
  @staticmethod
  def encodeString(string):
    return "\"" + CmdHandlerWidget.encodeStringChars(string) + "\""

  @staticmethod
  def encodeStrings(strings):
    result = "\""
    for i in range(0, len(strings)):
      if i > 0:
        result += "|"
      result += CmdHandlerWidget.encodeStringChars(strings[i])
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
      result += CmdHandlerWidget.encodeInt(xs[i])
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
      result += CmdHandlerWidget.encodeFloat(poss[i].x())
    result += "\""
    return result

  @staticmethod
  def encodePosYs(poss):
    result = "\""
    for i in range(0, len(poss)):
      if i > 0:
        result += "|"
      result += CmdHandlerWidget.encodeFloat(poss[i].y())
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(presetPath),
        CmdHandlerWidget.encodeFloat(pos.x()),
        CmdHandlerWidget.encodeFloat(pos.y()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(title),
        CmdHandlerWidget.encodeFloat(pos.x()),
        CmdHandlerWidget.encodeFloat(pos.y()),
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
          CmdHandlerWidget.encodeString(execPath),
          CmdHandlerWidget.encodeString(title),
          CmdHandlerWidget.encodeString(initialCode),
          CmdHandlerWidget.encodeFloat(pos.x()),
          CmdHandlerWidget.encodeFloat(pos.y()),
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
          CmdHandlerWidget.encodeString(execPath),
          CmdHandlerWidget.encodeString(title),
          CmdHandlerWidget.encodeFloat(pos.x()),
          CmdHandlerWidget.encodeFloat(pos.y()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(desiredNodeName),
        CmdHandlerWidget.encodeString(dataType),
        CmdHandlerWidget.encodeString(extDep),
        CmdHandlerWidget.encodeFloat(pos.x()),
        CmdHandlerWidget.encodeFloat(pos.y()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(desiredNodeName),
        CmdHandlerWidget.encodeString(varPath),
        CmdHandlerWidget.encodeFloat(pos.x()),
        CmdHandlerWidget.encodeFloat(pos.y()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(desiredNodeName),
        CmdHandlerWidget.encodeString(varPath),
        CmdHandlerWidget.encodeFloat(pos.x()),
        CmdHandlerWidget.encodeFloat(pos.y()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeStrings(nodeNames),
        CmdHandlerWidget.encodePosXs(newTopLeftPoss),
        CmdHandlerWidget.encodePosYs(newTopLeftPoss),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(backDropNodeName),
        CmdHandlerWidget.encodeFloat(newTopLeftPos.x()),
        CmdHandlerWidget.encodeFloat(newTopLeftPos.y()),
        CmdHandlerWidget.encodeFloat(newSize.width()),
        CmdHandlerWidget.encodeFloat(newSize.height()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeStrings(nodeNames),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeStrings(nodeNames),
        CmdHandlerWidget.encodeString(desiredNodeName),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(nodeName),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(srcPort),
        CmdHandlerWidget.encodeString(dstPort),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(srcPort),
        CmdHandlerWidget.encodeString(dstPort),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(desiredPortName),
        CmdHandlerWidget.encodeString(portTypeStr),
        CmdHandlerWidget.encodeString(typeSpec),
        CmdHandlerWidget.encodeString(portToConnect),
        CmdHandlerWidget.encodeString(extDep),
        CmdHandlerWidget.encodeString(metaData),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(oldPortName),
        CmdHandlerWidget.encodeString(desiredNewPortName),
        CmdHandlerWidget.encodeString(typeSpec),
        CmdHandlerWidget.encodeString(extDep),
        CmdHandlerWidget.encodeString(metaData),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(portName),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(nodeName),
        CmdHandlerWidget.encodeString(presetDirPath),
        CmdHandlerWidget.encodeString(presetName),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(nodeName),
        CmdHandlerWidget.encodeString(comment),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(code),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(oldNodeName),
        CmdHandlerWidget.encodeString(desiredNewNodeName),
        CmdHandlerWidget.encodeString(nodeMetadata),
        CmdHandlerWidget.encodeString(execMetadata),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(oldPortName),
        CmdHandlerWidget.encodeString(desiredNewPortName),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(json),
        CmdHandlerWidget.encodeFloat(cursorPos.x()),
        CmdHandlerWidget.encodeFloat(cursorPos.y()),
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
        CmdHandlerWidget.encodeString(argName),
        CmdHandlerWidget.encodeString(value.getTypeNameStr()),
        CmdHandlerWidget.encodeString(value.getJSONStr()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(portPath),
        CmdHandlerWidget.encodeString(value.getTypeNameStr()),
        CmdHandlerWidget.encodeString(value.getJSONStr()),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeString(refName),
        CmdHandlerWidget.encodeString(varPath),
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
                CmdHandlerWidget.encodeString(execPath),
                CmdHandlerWidget.encodeInts(indices),
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
        CmdHandlerWidget.encodeString(execPath),
        CmdHandlerWidget.encodeStrings(extDeps),
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
        CmdHandlerWidget.encodeString(execPath),
        ]
      )
