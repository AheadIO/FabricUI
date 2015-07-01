// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGConfig.h"
#include <CodeCompletion/KLTypeDesc.h>

using namespace FabricServices;
using namespace FabricUI::DFG;

DFGConfig::DFGConfig()
: defaultFont("Lucida", 10),
  fixedFont("Courier", 12),
  logFont("Courier", 10)
  , searchQueryFont( QFont("Courier", 12, QFont::Bold) )
  , searchResultsFont( QFont("Courier", 12) )
  , searchHelpFont( QFont("Courier", 12, QFont::Bold) )
{
  defaultWindowColor = QColor(12, 7, 47);
  defaultBackgroundColor = QColor(156, 174, 187);
  defaultFont.setBold(true);
  defaultFontColor = QColor(31, 43, 55, 255);
  fixedFont.setStyleHint( QFont::TypeWriter );
  logFont.setStyleHint( QFont::TypeWriter );
  searchBackgroundColor = QColor(135, 135, 135);
  searchHighlightColor = QColor(137, 181, 202);
  searchCursorColor = QColor(220, 220, 220);
  searchFontColor = QColor(0, 0, 0);
  varNodeDefaultColor = QColor(214, 191, 103);
  varLabelDefaultColor = QColor(188, 129, 83);

  klEditorConfig.codeBackgroundColor = defaultFontColor;
  klEditorConfig.codeFontColor = defaultBackgroundColor;

  registerDataTypeColor("", QColor(40, 40, 40));
  registerDataTypeColor("Boolean", QColor(240, 78, 35));
  registerDataTypeColor("Scalar", QColor(128, 195, 66));
  registerDataTypeColor("Float32", QColor(128, 195, 66));
  registerDataTypeColor("Float64", QColor(128, 195, 66));
  registerDataTypeColor("Byte", QColor(0, 153, 100));
  registerDataTypeColor("Integer", QColor(0, 153, 100));
  registerDataTypeColor("SInt8", QColor(0, 153, 100));
  registerDataTypeColor("UInt8", QColor(0, 153, 100));
  registerDataTypeColor("SInt16", QColor(0, 153, 100));
  registerDataTypeColor("UInt16", QColor(0, 153, 100));
  registerDataTypeColor("SInt32", QColor(0, 153, 100));
  registerDataTypeColor("UInt32", QColor(0, 153, 100));
  registerDataTypeColor("SInt64", QColor(0, 153, 100));
  registerDataTypeColor("UInt64", QColor(0, 153, 100));
  registerDataTypeColor("Index", QColor(0, 153, 100));
  registerDataTypeColor("Size", QColor(0, 153, 100));
  registerDataTypeColor("String", QColor(76, 16, 0));

  // todo: to be removed
  registerDataTypeColor("Regex", QColor(134, 55, 41));
  registerDataTypeColor("Vec2", QColor(255, 242, 0));
  registerDataTypeColor("Vec3", QColor(255, 242, 0));
  registerDataTypeColor("Vec4", QColor(255, 242, 0));
  registerDataTypeColor("Mat22", QColor(249, 157, 28));
  registerDataTypeColor("Mat33", QColor(249, 157, 28));
  registerDataTypeColor("Mat44", QColor(249, 157, 28));
  registerDataTypeColor("Xfo", QColor(249, 157, 28));
  registerDataTypeColor("Quat", QColor(0, 191, 232));
  registerDataTypeColor("Euler", QColor(0, 191, 232));
  registerDataTypeColor("RotationOrder", QColor(0, 191, 232));
  registerDataTypeColor("Color", QColor(255, 0, 0));
  registerDataTypeColor("RGB", QColor(255, 0, 0));
  registerDataTypeColor("RGBA", QColor(255, 0, 0));
  registerDataTypeColor("ARGB", QColor(255, 0, 0));
  registerDataTypeColor("Geometry", QColor(51, 1, 106));
  registerDataTypeColor("Lines", QColor(51, 1, 106));
  registerDataTypeColor("Points", QColor(51, 1, 106));
  registerDataTypeColor("PolygonMesh", QColor(51, 1, 106));
}

void DFGConfig::registerDataTypeColor(FTL::StrRef dataType, QColor color)
{
  std::string baseType = CodeCompletion::KLTypeDesc(dataType).getBaseType();
  std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);
  if(it != colorForDataType.end())
    it->second = color;
  colorForDataType.insert(std::pair<std::string, QColor>(baseType, color));
}

QColor DFGConfig::getColorForDataType(FTL::StrRef dataType, FabricCore::DFGExec * exec, char const * portName)
{
  if(dataType.size() > 0)
  {
    if(dataType.data()[0] == '$')
      return QColor(0, 0, 0);
    std::string baseType = CodeCompletion::KLTypeDesc(dataType.data()).getBaseType();
    std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);

    if(it == colorForDataType.end() && exec != NULL && portName != NULL)
    {
      QString uiColor = exec->getExecPortMetadata(portName, "uiColor");
      if(uiColor.length() > 0)
      {
        FabricCore::Variant uiColorVar = FabricCore::Variant::CreateFromJSON(uiColor.toUtf8().constData());
        const FabricCore::Variant * rVar = uiColorVar.getDictValue("r");
        const FabricCore::Variant * gVar = uiColorVar.getDictValue("g");
        const FabricCore::Variant * bVar = uiColorVar.getDictValue("b");

        if(rVar && gVar && bVar)
        {
          QColor color(rVar->getSInt32(), gVar->getSInt32(), bVar->getSInt32());
          registerDataTypeColor(dataType, color);
          return color;
        }
      }
    }

    if(it != colorForDataType.end())
      return it->second;
    return QColor(150, 150, 150);
  }
  return QColor(0, 0, 0);
}
