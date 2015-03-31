// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGValueEditor__
#define __UI_DFG_DFGValueEditor__

#include <ValueEditor/ValueEditorWidget.h>

#include "DFGWrapper/Node.h"
#include "DFGConfig.h"
#include "DFGController.h"

namespace FabricUI
{

  namespace DFG
  {

    class DFGValueEditor : public ValueEditor::ValueEditorWidget
    {
      Q_OBJECT

    public:

      DFGValueEditor(QWidget * parent, DFGController * controller, const DFGConfig & config = DFGConfig());
      virtual ~DFGValueEditor();

      FabricServices::DFGWrapper::NodePtr getNode();
      void setNode(FabricServices::DFGWrapper::NodePtr node);

    public slots:

      void updateOutputs();
      void onArgsChanged();

    private:

      FabricServices::DFGWrapper::NodePtr m_node;
      DFGConfig m_config;
      DFGController * m_controller;
    };

  };

};

#endif // __UI_DFG_DFGValueEditor__
