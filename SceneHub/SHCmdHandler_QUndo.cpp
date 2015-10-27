/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/SceneHub/SHCmd.h>
#include <FabricUI/SceneHub/SHCmdHandler_QUndo.h>
#include <FabricCore.h>
#include <FabricUI/SceneHub/macros.h>
#include <FTL/OwnedPtr.h>
#include <iostream>
#include <vector>

using namespace FabricUI;
using namespace FabricUI::SceneHub;


class SHCmdHandler_QUndo::WrappedCmd : public QUndoCommand {

  public:
    WrappedCmd( SHCmd *shCmd ) : QUndoCommand(), 
      m_shCmd( shCmd ),
      m_didit( false ) {
        m_shCmd->getDesc().print();
      };

  protected:
    virtual void redo() {
      try {
        if(m_didit) m_shCmd->redo();
        
        else {
          m_didit = true;
          m_shCmd->doit();
          QUndoCommand::setText( m_shCmd->getDesc().asString().c_str() );
        }
      }
      catch (FabricCore::Exception e) {
        printf("Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

    virtual void undo() {
      try {
        assert(m_didit);
        m_shCmd->undo();
      }
      catch (FabricCore::Exception e) {
        printf( "Caught FabricCore::Exception: %s\n", e.getDesc_cstr() );
      }
    }

  private:
    FTL::OwnedPtr<SHCmd> m_shCmd;
    bool m_didit;
};
 
 void SHCmdHandler_QUndo::addSGObject(std::string name, bool isGlobalObject) {

  FABRIC_TRY("SHCmdHandler_QUndo::addSGObject", 

    std::vector<FabricCore::RTVal> params(2);
    params[0] = FabricCore::RTVal::ConstructString(*m_client, name.c_str());
    params[1] = FabricCore::RTVal::ConstructBoolean(*m_client, isGlobalObject);
    FabricCore::RTVal sceneGraph = m_shObject.callMethod("SceneGraph", "getScene", 0, 0);
    sceneGraph.callMethod("", "addObjectCmd", 2, &params[0]);

    FabricCore::RTVal cmdManager = sceneGraph.callMethod("SGCmdManager", "getOrCreateHierarchyCmdManager", 0, 0);
    SHCmd *cmd = new SHCmd(cmdManager);

    SHCmdDescription desc;
    desc.name = "addObjectCmd";
    desc.params.resize(2);
    desc.params[0].name = "name";
    desc.params[0].value = name;
    desc.params[1].name = "isGlobalObject";
    desc.params[1].value = std::to_string(isGlobalObject);

    cmd->setDesc(desc);
    m_qUndoStack->push( new WrappedCmd( cmd ) );
  );
}