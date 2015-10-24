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

    class DFGValueEditor : public FabricUI::ValueEditor::ValueEditorWidget
    {
      Q_OBJECT

    public:

      DFGValueEditor(
        DFGController * controller,
        const DFGConfig & config
        );
      virtual ~DFGValueEditor();

    public slots:

      void clear();

      void setBinding(
        FabricCore::DFGBinding const &binding
        );

      void setNode(
        FabricCore::DFGBinding const &binding,
        FTL::StrRef m_execPath,
        FabricCore::DFGExec const &exec,
        FTL::StrRef m_nodeName
        );

      void updateOutputs();
      void onArgsChanged();

      void onNodeRemoved( FTL::CStrRef nodePathFromRoot );

    private:

      DFGConfig m_config;
      DFGController *m_controller;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      std::string m_nodeName;
    };

  };

};

#endif // __UI_DFG_DFGValueEditor__
