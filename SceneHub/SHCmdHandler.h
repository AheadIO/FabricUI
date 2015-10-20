//
// Copyright 2010-2015 Fabric Software Inc. All rights reserved.
//

#ifndef __FABRICUI_SceneHub_CmdHandler__
#define __FABRICUI_SceneHub_CmdHandler__

#include <QtCore/QAbstractItemModel>
#include <QtGui/QMenu>
#include <QtGui/QTreeView>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <FTL/OwnedPtr.h>
#include <FTL/SharedPtr.h>
#include <FTL/ArrayRef.h>
#include <FTL/CStrRef.h>
#include <FabricCore.h>
#include <vector>
#include <assert.h>
#include <iostream>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmdHandler
    {
      public:

        SHCmdHandler() {};
        virtual ~SHCmdHandler() {};

        static std::string encodeRTValToJSON(
          FabricCore::Context const& context,
          FabricCore::RTVal const& rtVal
          );

        static void decodeRTValFromJSON(
          FabricCore::Context const& context,
          FabricCore::RTVal & rtVal,
          FTL::CStrRef json
          );

        //virtual void dfgDoRemoveNodes(
        //  FabricCore::DFGBinding const &binding,
        //  FTL::CStrRef execPath,
        //  FabricCore::DFGExec const &exec,
        //  FTL::ArrayRef<FTL::StrRef> nodeNames
        //  ) = 0;

        //virtual void dfgDoConnect(
        //  FabricCore::DFGBinding const &binding,
        //  FTL::CStrRef execPath,
        //  FabricCore::DFGExec const &exec,
        //  FTL::CStrRef srcPath, 
        //  FTL::CStrRef dstPath
        //  ) = 0;
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler__

