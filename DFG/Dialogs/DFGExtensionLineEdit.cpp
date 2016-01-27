// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGExtensionLineEdit.h"
#include <FTL/CStrRef.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGExtensionLineEdit::DFGExtensionLineEdit(QWidget * parent, FabricCore::Client & client, QString text)
: DFGAutoCompleteLineEdit(parent, text)
{
  // todo: gather information about all loaded extensions
}
