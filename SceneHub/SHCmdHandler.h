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
#include <FabricUI/SceneHub/SHCmd.h>
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
        virtual ~SHCmdHandler() {};

        static std::string encodeRTValToJSON(FabricCore::Context const& context, FabricCore::RTVal const& rtVal);

        static void decodeRTValFromJSON(FabricCore::Context const& context, FabricCore::RTVal & rtVal, FTL::CStrRef json); 
    };
  };
};

#endif // __FABRICUI_SceneHub_CmdHandler__

