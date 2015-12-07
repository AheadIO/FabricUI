// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_PinLabel__
#define __UI_GraphView_PinLabel__

#include "TextContainer.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class Pin;

    class PinLabel : public TextContainer
    {
      Q_OBJECT

    public:

      PinLabel(Pin * parent, QString text, QColor color, QColor highlightColor, QFont font);

    };

  };

};

#endif // __UI_GraphView_PinLabel__
