// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGValueEditor__
#define __UI_DFG_DFGValueEditor__

#include <FabricUI/ValueEditor/ValueEditorWidget.h>

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

      DFGValueEditor(
        QWidget * parent,
        DFGController * controller,
        const DFGConfig & config = DFGConfig()
        );
      virtual ~DFGValueEditor();

      FTL::CStrRef getNodeName()
        { return m_nodeName; }
      void setNodeName( FTL::StrRef nodeName );

    public slots:

      void updateOutputs();
      void onArgsChanged();

    private:

      std::string unAliasType(const std::string & type);

      std::string m_nodeName;
      DFGConfig m_config;
      DFGController * m_controller;
    };

  };

};

#endif // __UI_DFG_DFGValueEditor__
