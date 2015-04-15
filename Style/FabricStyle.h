// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_Style_FabricStyle__
#define __UI_Style_FabricStyle__

#include <QtGui/QWindowsStyle>

namespace FabricUI
{

  namespace Style
  {

    class FabricStyle : public QWindowsStyle
    {
        Q_OBJECT

    public:
        FabricStyle() {}

        void polish(QPalette &palette);
    //     void polish(QWidget *widget);
    //     void unpolish(QWidget *widget);
    //     int styleHint(StyleHint which, const QStyleOption *option,
    //                   const QWidget *widget = 0,
    //                   QStyleHintReturn *returnData = 0) const;
    //     int pixelMetric(PixelMetric which, const QStyleOption *option,
    //                     const QWidget *widget = 0) const;
    //     void drawPrimitive(PrimitiveElement which,
    //                        const QStyleOption *option, QPainter *painter,
    //                        const QWidget *widget = 0) const;
    //     void drawComplexControl(ComplexControl which,
    //                             const QStyleOptionComplex *option,
    //                             QPainter *painter,
    //                             const QWidget *widget = 0) const;
    //     QRect subControlRect(ComplexControl whichControl,
    //                          const QStyleOptionComplex *option,
    //                          SubControl whichSubControl,
    //                          const QWidget *widget = 0) const;
    // public slots:
    //     QIcon standardIconImplementation(StandardPixmap which,
    //                                      const QStyleOption *option,
    //                                      const QWidget *widget = 0) const;
    // private:
    //     void drawFabricFrame(const QStyleOption *option,
    //                          QPainter *painter) const;
    //     void drawFabricBevel(const QStyleOption *option,
    //                          QPainter *painter) const;
    //     void drawFabricCheckBoxIndicator(const QStyleOption *option,
    //                                      QPainter *painter) const;
    //     void drawFabricSpinBoxButton(SubControl which,
    //                                  const QStyleOptionComplex *option,
    //                                  QPainter *painter) const;
    };

  };

};

#endif // __UI_Style_FabricStyle__
