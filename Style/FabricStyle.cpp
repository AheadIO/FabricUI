// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "FabricStyle.h"

#include <QtGui/QStyleHintReturn>
#include <QtGui/QStyleOptionComplex>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QStyleOption>

using namespace FabricUI::Style;

void FabricStyle::polish(QPalette &palette)
{
  QColor baseColor = QColor(60, 60, 60);
  QColor highlightColor = QColor(240, 240, 240);
  float brightnessSpread = 4.5f;
  float spread = brightnessSpread;

  if(baseColor.toHsv().valueF() > 0.5)
    spread = 100.0f / brightnessSpread;
  else
    spread = 100.0f * brightnessSpread;
  
  QColor highlightedTextColor;
  if(highlightColor.toHsv().valueF() > 0.6)
    highlightedTextColor= baseColor.darker(spread*2);
  else
    highlightedTextColor= baseColor.lighter(spread*2);
  
  palette.setBrush(QPalette::Background, QBrush(baseColor));
  palette.setBrush(QPalette::Window, QBrush(baseColor));
  palette.setBrush(QPalette::Foreground, baseColor.lighter(spread));
  palette.setBrush(QPalette::WindowText, baseColor.lighter(spread));
  palette.setBrush(QPalette::Base, baseColor);
  palette.setBrush(QPalette::AlternateBase, baseColor.darker(spread));
  palette.setBrush(QPalette::ToolTipBase, baseColor);
  palette.setBrush(QPalette::ToolTipText, baseColor.lighter(spread));
  palette.setBrush(QPalette::Text, baseColor.lighter(spread*1.2));
  palette.setBrush(QPalette::Button, baseColor);
  palette.setBrush(QPalette::ButtonText, baseColor.lighter(spread));
  palette.setBrush(QPalette::BrightText, QColor(240, 240, 240));
  palette.setBrush(QPalette::Light, baseColor.lighter(spread));
  palette.setBrush(QPalette::Midlight, baseColor.lighter(spread/2));
  palette.setBrush(QPalette::Dark, baseColor.darker(spread));
  palette.setBrush(QPalette::Mid, baseColor);
  palette.setBrush(QPalette::Shadow, baseColor.darker(spread*2));
  palette.setBrush(QPalette::Highlight, highlightColor);
  palette.setBrush(QPalette::HighlightedText, highlightedTextColor);
}
