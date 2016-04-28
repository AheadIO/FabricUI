// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "FabricApplication.h"
#include "FabricUI/Style/FabricStyle.h"

using namespace FabricUI::Application;

static int    s_argc = 0;
static char **s_argv = NULL;

FabricApplication::FabricApplication() : QApplication(s_argc, s_argv)
{
  setStyle(new FabricUI::Style::FabricStyle());
}
