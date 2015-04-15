// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "FabricStyle.h"

#include <QtGui/QStyleHintReturn>
#include <QtGui/QStyleOptionComplex>
#include <QtGui/QStyleOptionButton>
#include <QtGui/QStyleOption>

using namespace FabricUI::Style;

void FabricStyle::polish(QPalette &palette)
{
  QColor baseColor = QColor(40, 40, 40);
  QColor highlightColor = QColor(230, 230, 230);
  float brightnessSpread = 4.0f;
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

// void FabricStyle::polish(QWidget *widget)
// {
//   if (qobject_cast<QAbstractButton *>(widget) || qobject_cast<QAbstractSpinBox *>(widget))
//     widget->setAttribute(Qt::WA_Hover, true);
// }

// void FabricStyle::unpolish(QWidget *widget)
// {
//   if (qobject_cast<QAbstractButton *>(widget) || qobject_cast<QAbstractSpinBox *>(widget))
//     widget->setAttribute(Qt::WA_Hover, false);
// }

// int FabricStyle::styleHint(StyleHint which, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData) const
// {
//   switch (which) {
//   case SH_DialogButtonLayout:
//     return int(QDialogButtonBox::MacLayout);
//   case SH_EtchDisabledText:
//     return int(true);
//   case SH_DialogButtonBox_ButtonsHaveIcons:
//     return int(true);
//   case SH_UnderlineShortcut:
//     return int(false);
//   default:
//     return QWindowsStyle::styleHint(which, option, widget, returnData);
//   }
// }

// int FabricStyle::pixelMetric(PixelMetric which, const QStyleOption *option, const QWidget *widget) const
// {
//   switch (which) {
//     case PM_ButtonDefaultIndicator:
//       return 0;
//     case PM_IndicatorWidth:
//     case PM_IndicatorHeight:
//       return 16;
//     case PM_CheckBoxLabelSpacing:
//       return 8;
//     case PM_DefaultFrameWidth:
//       return 2;
//     default:
//       return QWindowsStyle::pixelMetric(which, option, widget);
//   }
// }

// QIcon FabricStyle::standardIconImplementation(StandardPixmap which,
//         const QStyleOption *option, const QWidget *widget) const
// {
//     QImage image = QWindowsStyle::standardPixmap(which, option, widget)
//                    .toImage();
//     if (image.isNull())
//         return QIcon();
//     QPalette palette;
//     if (option) {
//         palette = option->palette;
//     } else if (widget) {
//         palette = widget->palette();
//     }
//     QPainter painter(&image);
//     painter.setOpacity(0.25);
//     painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
//     painter.fillRect(image.rect(), palette.highlight());
//     painter.end();

//     return QIcon(QPixmap::fromImage(image));
// }

// void FabricStyle::drawPrimitive(PrimitiveElement which,
//                                 const QStyleOption *option,
//                                 QPainter *painter,
//                                 const QWidget *widget) const
// {
//     switch (which) {
//     case PE_IndicatorCheckBox:
//         drawFabricCheckBoxIndicator(option, painter);
//         break;
//     case PE_PanelButtonCommand:
//         drawFabricBevel(option, painter);
//         break;
//     case PE_Frame:
//         drawFabricFrame(option, painter);
//         break;
//     case PE_FrameDefaultButton:
//         break;
//     default:
//         QWindowsStyle::drawPrimitive(which, option, painter, widget);
//     }
// }

// void FabricStyle::drawComplexControl(ComplexControl which,
//                                      const QStyleOptionComplex *option,
//                                      QPainter *painter,
//                                      const QWidget *widget) const
// {
//     if (which == CC_SpinBox) {
//         drawFabricSpinBoxButton(SC_SpinBoxDown, option, painter);
//         drawFabricSpinBoxButton(SC_SpinBoxUp, option, painter);

//         QRect rect = subControlRect(CC_SpinBox, option,
//                                     SC_SpinBoxEditField)
//                      .adjusted(-1, 0, +1, 0);
//         painter->setPen(QPen(option->palette.mid(), 1.0));
//         painter->drawLine(rect.topLeft(), rect.bottomLeft());
//         painter->drawLine(rect.topRight(), rect.bottomRight());
//     } else {
//         return QWindowsStyle::drawComplexControl(which, option, painter,
//                                                  widget);
//     }
// }

// QRect FabricStyle::subControlRect(ComplexControl whichControl,
//                                   const QStyleOptionComplex *option,
//                                   SubControl whichSubControl,
//                                   const QWidget *widget) const
// {
//     if (whichControl == CC_SpinBox) {
//         int frameWidth = pixelMetric(PM_DefaultFrameWidth, option,
//                                      widget);

//         int buttonWidth = 16;

//         switch (whichSubControl) {
//         case SC_SpinBoxFrame:
//             return option->rect;
//         case SC_SpinBoxEditField:
//             return option->rect.adjusted(+buttonWidth, +frameWidth,
//                                          -buttonWidth, -frameWidth);
//         case SC_SpinBoxDown:
//             return visualRect(option->direction, option->rect,
//                               QRect(option->rect.x(), option->rect.y(),
//                                     buttonWidth,
//                                     option->rect.height()));
//         case SC_SpinBoxUp:
//             return visualRect(option->direction, option->rect,
//                               QRect(option->rect.right() - buttonWidth,
//                                     option->rect.y(),
//                                     buttonWidth,
//                                     option->rect.height()));
//         default:
//             return QRect();
//         }
//     } else {
//         return QWindowsStyle::subControlRect(whichControl, option,
//                                              whichSubControl, widget);
//     }
// }

// void FabricStyle::drawFabricFrame(const QStyleOption *option,
//                                   QPainter *painter) const
// {
//     painter->save();
//     painter->setRenderHint(QPainter::Antialiasing, true);
//     painter->setPen(QPen(option->palette.foreground(), 1.0));
//     painter->drawRect(option->rect.adjusted(+1, +1, -1, -1));
//     painter->restore();
// }

// void FabricStyle::drawFabricBevel(const QStyleOption *option,
//                                   QPainter *painter) const
// {
//     QColor buttonColor = option->palette.button().color();
//     int coeff = (option->state & State_MouseOver) ? 115 : 105;

//     QLinearGradient gradient(0, 0, 0, option->rect.height());
//     gradient.setColorAt(0.0, option->palette.light().color());
//     gradient.setColorAt(0.2, buttonColor.lighter(coeff));
//     gradient.setColorAt(0.8, buttonColor.darker(coeff));
//     gradient.setColorAt(1.0, option->palette.dark().color());

//     double penWidth = 1.0;
//     if (const QStyleOptionButton *buttonOpt =
//             qstyleoption_cast<const QStyleOptionButton *>(option)) {
//         if (buttonOpt->features & QStyleOptionButton::DefaultButton)
//             penWidth = 2.0;
//     }    

//     QRect roundRect = option->rect.adjusted(+1, +1, -1, -1);
//     if (!roundRect.isValid())
//         return;

//     int diameter = 12;
//     int cx = 100 * diameter / roundRect.width();
//     int cy = 100 * diameter / roundRect.height();

//     painter->save();
//     painter->setPen(Qt::NoPen);
//     painter->setBrush(gradient);
//     painter->drawRoundRect(roundRect, cx, cy);

//     if (option->state & (State_On | State_Sunken)) {
//         QColor slightlyOpaqueBlack(0, 0, 0, 63);
//         painter->setBrush(slightlyOpaqueBlack);
//         painter->drawRoundRect(roundRect, cx, cy);
//     }

//     painter->setRenderHint(QPainter::Antialiasing, true);
//     painter->setPen(QPen(option->palette.foreground(), penWidth));
//     painter->setBrush(Qt::NoBrush);
//     painter->drawRoundRect(roundRect, cx, cy);
// }

// void FabricStyle::drawFabricSpinBoxButton(SubControl which,
//         const QStyleOptionComplex *option, QPainter *painter) const
// {
//     PrimitiveElement arrow = PE_IndicatorArrowLeft;
//     QRect buttonRect = option->rect;
//     if ((which == SC_SpinBoxUp)
//             != (option->direction == Qt::RightToLeft)) {
//         arrow = PE_IndicatorArrowRight;
//         buttonRect.translate(buttonRect.width() / 2, 0);
//     }
//     buttonRect.setWidth((buttonRect.width() + 1) / 2);

//     QStyleOption buttonOpt(*option);

//     painter->save();
//     painter->setClipRect(buttonRect, Qt::IntersectClip);
//     if (!(option->activeSubControls & which))
//         buttonOpt.state &= ~(State_MouseOver | State_On | State_Sunken);
//     drawFabricBevel(&buttonOpt, painter);

//     QStyleOption arrowOpt(buttonOpt);
//     arrowOpt.rect = subControlRect(CC_SpinBox, option, which)
//                     .adjusted(+3, +3, -3, -3);
//     if (arrowOpt.rect.isValid())
//         drawPrimitive(arrow, &arrowOpt, painter);
//     painter->restore();
// }

// void FabricStyle::drawFabricCheckBoxIndicator(
//         const QStyleOption *option, QPainter *painter) const
// {
//     painter->save();
//     painter->setRenderHint(QPainter::Antialiasing, true);

//     if (option->state & State_MouseOver) {
//         painter->setBrush(option->palette.alternateBase());
//     } else {
//         painter->setBrush(option->palette.base());
//     }
//     painter->drawRoundRect(option->rect.adjusted(+1, +1, -1, -1));

//     if (option->state & (State_On | State_NoChange)) {
//         QPixmap pixmap;
//         if (!(option->state & State_Enabled)) {
//             pixmap.load(":/images/checkmark-disabled.png");
//         } else if (option->state & State_NoChange) {
//             pixmap.load(":/images/checkmark-partial.png");
//         } else {
//             pixmap.load(":/images/checkmark.png");
//         }

//         QRect pixmapRect = pixmap.rect()
//                                  .translated(option->rect.topLeft())
//                                  .translated(+2, -6);
//         QRect painterRect = visualRect(option->direction, option->rect,
//                                        pixmapRect);
//         if (option->direction == Qt::RightToLeft) {
//             painter->scale(-1.0, +1.0);
//             painterRect.moveLeft(-painterRect.right() - 1);
//         }
//         painter->drawPixmap(painterRect, pixmap);
//     }
//     painter->restore();
// }
