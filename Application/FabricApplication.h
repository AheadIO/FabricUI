// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_Application_FabricApplication__
#define __UI_Application_FabricApplication__

#include <QtGui/QApplication>

namespace FabricUI
{

  namespace Application
  {

    class FabricApplication : public QApplication
    {
    public:
        FabricApplication(int &argc, char **argv);
    };

  };

};

#endif // __UI_Application_FabricApplication__
