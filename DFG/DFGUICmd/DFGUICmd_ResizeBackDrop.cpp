//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#include <FabricUI/DFG/DFGUICmd/DFGUICmd_ResizeBackDrop.h>
#include <FTL/JSONEnc.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

void DFGUICmd_ResizeBackDrop::appendDesc( QString &desc )
{
  desc += "Resize backdrop ";
  appendDesc_NodeName( m_backDropName, desc );
}

void DFGUICmd_ResizeBackDrop::invoke( unsigned &coreUndoCount )
{
  invoke(
    m_backDropName.toUtf8().constData(),
    coreUndoCount
    );
}

void DFGUICmd_ResizeBackDrop::invoke(
  FTL::CStrRef backDropName,
  unsigned &coreUndoCount
  )
{
  {
    std::string json;
    {
      FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
      FTL::JSONObjectEnc<std::string> joe( je );
      {
        FTL::JSONEnc<std::string> xJE( joe, FTL_STR("x") );
        FTL::JSONFloat64Enc<std::string> xJFE( xJE, m_newTopLeftPos.x() );
      }
      {
        FTL::JSONEnc<std::string> yJE( joe, FTL_STR("y") );
        FTL::JSONFloat64Enc<std::string> yJFE( yJE, m_newTopLeftPos.y() );
      }
    }
    getExec().setNodeMetadata(
      backDropName.c_str(), "uiGraphPos", json.c_str(), true, false
      );
    ++coreUndoCount;
  }

  {
    std::string json;
    {
      FTL::JSONEnc<std::string> je( json, FTL::JSONFormat::Packed() );
      FTL::JSONObjectEnc<std::string> joe( je );
      {
        FTL::JSONEnc<std::string> wJE( joe, FTL_STR("w") );
        FTL::JSONFloat64Enc<std::string> wJFE( wJE, m_newSize.width() );
      }
      {
        FTL::JSONEnc<std::string> hJE( joe, FTL_STR("h") );
        FTL::JSONFloat64Enc<std::string> hJFE( hJE, m_newSize.height() );
      }
    }
    getExec().setNodeMetadata(
      backDropName.c_str(), "uiGraphSize", json.c_str(), true, false
      );
    ++coreUndoCount;
  }
}

FABRIC_UI_DFG_NAMESPACE_END
