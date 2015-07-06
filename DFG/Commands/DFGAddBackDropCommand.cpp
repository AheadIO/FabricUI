// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGAddBackDropCommand.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAddBackDropCommand::DFGAddBackDropCommand(
  DFGController *controller,
  FTL::CStrRef title,
  QPointF pos
  )
  : DFGCommand(controller)
  , m_title(title)
  , m_pos(pos)
{
}

GraphView::Node * DFGAddBackDropCommand::getNode()
{
  DFGController * ctrl = static_cast<DFGController *>( controller() ); 
  if ( ctrl->graph() )
    return ctrl->graph()->nodeFromPath( m_nodePath.c_str() );
  return NULL;
}

bool DFGAddBackDropCommand::invoke()
{
  DFGController * ctrl = (DFGController*)controller();
  FabricCore::DFGExec graph = ctrl->getCoreDFGExec();
  
  m_nodePath = graph.addUser( m_title.c_str() );

  graph.setNodeMetadata(
    m_nodePath.c_str(),
    "uiTitle",
    m_title.c_str(),
    false
    );

  std::string posJSONString;
  {
    FTL::JSONEnc<> enc( posJSONString );
    FTL::JSONObjectEnc<> objEnc( enc );
    {
      FTL::JSONEnc<> xEnc( objEnc, FTL_STR("x") );
      FTL::JSONFloat64Enc<> xF64Enc( xEnc, m_pos.x() );
    }
    {
      FTL::JSONEnc<> yEnc( objEnc, FTL_STR("y") );
      FTL::JSONFloat64Enc<> yF64Enc( yEnc, m_pos.y() );
    }
  }
  graph.setNodeMetadata(
    m_nodePath.c_str(),
    "uiGraphPos",
    posJSONString.c_str(),
    false
    );

  return true;
}
