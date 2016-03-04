// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGVariablePathLineEdit.h"
#include "../DFGController.h"
#include <FTL/CStrRef.h>
#include <FTL/JSONValue.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGVariablePathLineEdit::DFGVariablePathLineEdit(
  QWidget * parent,
  FabricCore::DFGBinding const &binding,
  FTL::CStrRef currentExecPath,
  QString text
  )
  : DFGAutoCompleteLineEdit(parent, text)
{
  FabricCore::DFGBinding localBinding = binding;
  QStringList words = DFGController::getVariableWordsFromBinding(localBinding, currentExecPath);
  setWords(words);
}
