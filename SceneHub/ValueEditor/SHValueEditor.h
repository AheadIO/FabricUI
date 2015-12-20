/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SHVALUEEDITOR_H__
#define __UI_SCENEHUB_SHVALUEEDITOR_H__

#include <FabricUI/Canvas/DFG/DFGConfig.h>
#include <FabricUI/Canvas/DFG/DFGValueEditor.h>
#include <FabricUI/Canvas/DFG/DFGController.h>
#include <FabricUI/SceneHub/TreeView/SHTreeView.h>
#include <FabricUI/SceneHub/TreeView/SHTreeItem.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHValueEditor : public DFG::DFGValueEditor
    {
      Q_OBJECT

      public:
        SHValueEditor(
          FabricUI::DFG::DFGController *controller, 
          const FabricUI::DFG::DFGConfig &config,
          FabricUI::SceneHub::SHTreeView *shTreeView);
          
        virtual ~SHValueEditor() {};

      public slots:
        virtual void clear();
         void onSHArgsChanged(FabricUI::SceneHub::SHTreeItem *shTreeItem);

      private:
        FabricUI::SceneHub::SHTreeView *m_shTreeView;
    };
  };
};

#endif // __UI_SCENEHUB_SHVALUEEDITOR_H__
