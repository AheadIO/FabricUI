// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGVariablePathLineEdit.h"
#include <FTL/CStrRef.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGVariablePathLineEdit::DFGVariablePathLineEdit(
  QWidget * parent,
  FabricCore::DFGBinding const &binding,
  QString text
  )
  : DFGAutoCompleteLineEdit(parent, text)
{
  QStringList words;
  // todo: variables API
  setWords(words);
}
