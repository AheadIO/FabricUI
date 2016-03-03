// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGConfig__
#define __UI_DFG_DFGConfig__

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QTextCharFormat>
#include <string>
#include <map>
#include <FabricUI/KLEditor/EditorConfig.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FTL/StrRef.h>
#include <FabricCore.h>

namespace FabricUI
{

  namespace DFG
  {

    struct DFGConfig
    {
      QColor defaultBackgroundColor;
      QColor defaultWindowColor;
      QFont defaultFont;
      QColor defaultFontColor;
      QFont fixedFont;
      QFont logFont;
      QColor searchBackgroundColor;
      QColor searchHighlightColor;
      QColor searchCursorColor;
      QColor searchFontColor;
      QFont searchQueryFont;
      QFont searchResultsFont;
      QFont searchHelpFont;

      std::map<std::string, QColor> colorForDataType;

      QColor varNodeDefaultColor;
      QColor varLabelDefaultColor;

      KLEditor::EditorConfig klEditorConfig;
      GraphView::GraphConfig graphConfig;

      DFGConfig();

      void registerDataTypeColor(FTL::StrRef dataType, QColor color);
      QColor getColorForDataType(FTL::StrRef dataType, FabricCore::DFGExec * exec = NULL, char const * portName = NULL);
    };

  };

};

#endif // __UI_DFG_DFGConfig__
