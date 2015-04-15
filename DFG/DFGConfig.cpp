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
}

void DFGConfig::registerDataTypeColor(const std::string & dataType, QColor color)
{
  std::string baseType = CodeCompletion::KLTypeDesc(dataType).getBaseType();
  std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);
  if(it != colorForDataType.end())
    it->second = color;
  colorForDataType.insert(std::pair<std::string, QColor>(baseType, color));
}

std::string filterTypeColor(const std::string & typeColor)
{
  std::string filteredTypeColor;
  int commaCount = 0;
  for(unsigned int i=0;i<typeColor.length();i++)
  {
    char c = typeColor[i];
    if(c == ',')
      commaCount++;
    if(isalnum(c) || c == '(' || c == ')' || c == ',') // don't use '.'' since it is all integers
      filteredTypeColor += tolower(c);
  }

  if(filteredTypeColor.length() < 12) // color(0,0,0)
  {
    printf("Invalid dfgTypeColor token found: '%s'\n", typeColor.c_str());
    return "";
  }
  if(commaCount != 2)
  {
    printf("Invalid dfgTypeColor token found: '%s'\n", typeColor.c_str());
    return "";
  }
  if(filteredTypeColor.substr(0, 6) != "color(")
  {
    printf("Invalid dfgTypeColor token found: '%s'\n", typeColor.c_str());
    return "";
  }
  if(filteredTypeColor[filteredTypeColor.length()-1] != ')')
  {
    printf("Invalid dfgTypeColor token found: '%s'\n", typeColor.c_str());
    return "";
  }

  return filteredTypeColor;
}

QColor DFGConfig::getColorForDataType(const std::string & dataType, ASTWrapper::KLASTManager * manager)
{
  if(dataType.length() > 0)
  {
    if(dataType[0] == '$')
      return QColor(0, 0, 0);
    std::string baseType = CodeCompletion::KLTypeDesc(dataType).getBaseType();
    std::map<std::string, QColor>::iterator it = colorForDataType.find(baseType);

    // get it from the AST manager
    if(it == colorForDataType.end() && manager)
    {
      const ASTWrapper::KLType * klType = manager->getKLTypeByName(baseType.c_str());
      if(klType)
      {
        std::string typeColor = klType->getComments()->getSingleQualifier("dfgTypeColor");
        typeColor = filterTypeColor(typeColor);
        if(typeColor.length() > 0)
        {
          std::string parts = typeColor.substr(6, typeColor.length() - 7);

          int firstComma = parts.find(',');
          int secondComma = parts.find(',', firstComma + 1);

          std::string rStr = parts.substr(0, firstComma);
          std::string gStr = parts.substr(firstComma + 1, secondComma - firstComma - 1);
          std::string bStr = parts.substr(secondComma + 1, parts.length() - secondComma);

          int r, g, b;
          r = atoi(rStr.c_str());
          g = atoi(gStr.c_str());
          b = atoi(bStr.c_str());

          QColor color(r, g, b);
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
