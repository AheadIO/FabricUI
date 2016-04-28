// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "FabricApplication.h"
#include "FabricUI/Style/FabricStyle.h"

using namespace FabricUI::Application;

FabricApplication::FabricApplication(int &argc, char **argv) : QApplication(argc, argv)
{
  setStyle(new FabricUI::Style::FabricStyle());
}
