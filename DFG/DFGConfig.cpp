// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGConfig.h"
#include <CodeCompletion/KLTypeDesc.h>

using namespace FabricServices;
using namespace FabricUI::DFG;

DFGConfig::DFGConfig()
: defaultFont("Lucida", 10),
  fixedFont("Courier", 12),
  logFont("Courier", 10)
{
  defaultWindowColor = QColor(80, 98, 110);
  defaultBackgroundColor = QColor(156, 174, 187);
  defaultFont.setBold(true);
  defaultFontColor = QColor(31, 43, 55, 255);
  fixedFont.setStyleHint( QFont::TypeWriter );
  logFont.setStyleHint( QFont::TypeWriter );
  searchBackgroundColor = QColor(135, 135, 135);
  searchHighlightColor = QColor(137, 181, 202);
  searchFontColor = QColor(0, 0, 0);

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
  registerDataTypeColor("String", QColor(134, 55, 41));

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
  registerDataTypeColor("Geometry", QColor(92, 46, 145));
  registerDataTypeColor("Lines", QColor(92, 46, 145));
  registerDataTypeColor("Points", QColor(92, 46, 145));
  registerDataTypeColor("PolygonMesh", QColor(92, 46, 145));
}

void DFGConfig::registerDataTypeColor(const std::string & dataType, QColor color)
{
  std::string baseType = CodeCompletion::KLTypeDesc(dataType).getBaseType();
  std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);
  if(it != colorForDataType.end())
    it->second = color;
  colorForDataType.insert(std::pair<std::string, QColor>(baseType, color));
}

QColor DFGConfig::getColorForDataType(const std::string & dataType, DFGWrapper::PortPtr port)
{
  if(dataType.length() > 0)
  {
    if(dataType[0] == '$')
      return QColor(0, 0, 0);
    std::string baseType = CodeCompletion::KLTypeDesc(dataType).getBaseType();
    std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);

    if(it == colorForDataType.end() && port)
    {
      QString uiColor = port->getMetadata("uiColor");
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
