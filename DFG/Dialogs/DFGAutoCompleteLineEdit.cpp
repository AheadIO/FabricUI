// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QCompleter>
#include "DFGAutoCompleteLineEdit.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGAutoCompleteLineEdit::DFGAutoCompleteLineEdit(QWidget * parent, QString text)
: QLineEdit(text, parent)
{
}

DFGAutoCompleteLineEdit::~DFGAutoCompleteLineEdit()
{
}

void DFGAutoCompleteLineEdit::setWords(const QStringList & words)
{
  if(words.length() == 0)
    return;
  QCompleter* completer = new QCompleter(words);
  setCompleter(completer); 
}
