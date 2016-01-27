// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_NodeLabel__
#define __UI_GraphView_NodeLabel__

#include "TextContainer.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward declarations
    class NodeHeader;

    class NodeLabel : public TextContainer
    {
      Q_OBJECT

    public:

      NodeLabel(
        NodeHeader * parent,
        QString const &text,
        QColor color,
        QColor highlightColor,
        QFont font
        );

    };

  };

};

#endif // __UI_GraphView_NodeLabel__
