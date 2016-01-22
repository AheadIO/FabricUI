//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "DFGExecNotifier.h"

#include <FTL/JSONValue.h>
#include <stdio.h>

namespace FabricUI {
namespace DFG {

DFGExecNotifier::DFGExecNotifier( FabricCore::DFGExec exec )
  : m_view( exec.createView( &ViewCallback, this ) )
  , m_depth( 0 )
{
}

DFGExecNotifier::~DFGExecNotifier()
{
}

void DFGExecNotifier::handle( FTL::CStrRef jsonStr )
{
  try
  {
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );

    FTL::CStrRef descStr = jsonObject->getString( FTL_STR("desc") );
    if ( descStr == FTL_STR("nodePortRenamed") )
    {
      FTL::CStrRef nodeName = jsonObject->getString( FTL_STR("nodeName") );
      unsigned portIndex = unsigned( jsonObject->getSInt32( FTL_STR("portIndex") ) );
      FTL::CStrRef oldPortName = jsonObject->getString( FTL_STR("oldPortName") );
      FTL::CStrRef newPortName = jsonObject->getString( FTL_STR("newPortName") );

      emit nodePortRenamed( nodeName, portIndex, oldPortName, newPortName );
    }
    else
    {
      fprintf(
        stderr,
        "DFGExecNotifier::viewCallback: warning: unhandled notification:\n%s\n",
        jsonStr.c_str()
        );
    }
  }
  catch ( FabricCore::Exception e )
  {
    printf(
      "DFGExecNotifier::viewCallback: caught Core exception: %s\n",
      e.getDesc_cstr()
      );
  }
  catch ( FTL::JSONException e )
  {
    printf(
      "DFGExecNotifier::viewCallback: caught FTL::JSONException: %s\n",
      e.getDescCStr()
      );
  }
}

void DFGExecNotifier::viewCallback( FTL::CStrRef jsonStr )
{
  if ( m_depth > 0 )
  {
    // Process later as we are already processing notifications
    m_queued.push_back( jsonStr );
    return;
  }

  DepthBracket _( this );

  handle( jsonStr );

  while ( !m_queued.empty() )
  {
    std::vector<std::string> queued;
    m_queued.swap( queued );
    for ( std::vector<std::string>::const_iterator it = queued.begin();
      it != queued.end(); ++it )
    {
      FTL::CStrRef queuedJSONStr = *it;
      handle( queuedJSONStr );
    }
  }
}

void DFGExecNotifier::ViewCallback(
  void * userData,
  char const *jsonCString,
  uint32_t jsonLength
  )
{
  static_cast<DFGExecNotifier *>(
    userData
    )->viewCallback(
      FTL::CStrRef( jsonCString, jsonLength )
      );
}

} // namespace DFG
} // namespace FabricUI
