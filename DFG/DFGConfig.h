// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGConfig__
#define __UI_DFG_DFGConfig__

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QTextCharFormat>
#include <string>
#include <map>

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
      QColor searchBackgroundColor;
      QColor searchHighlightColor;
      QColor searchFontColor;

      std::map<std::string, QColor> colorForDataType;

      DFGConfig();

      void registerDataTypeColor(const std::string & dataType, QColor color);
      QColor getColorForDataType(const std::string & dataType);
    };

  };

};

#endif // __UI_DFG_DFGConfig__