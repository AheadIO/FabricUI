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
    };

  };

};

#endif // __UI_Style_FabricStyle__
