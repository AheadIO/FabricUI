// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifdef _WIN32
// [andrew 20150630] warning caused by Qt headers
# pragma warning( disable : 4267 )
#endif

#include <QtGui/QCompleter>
#include "DFGAutoCompleteLineEdit.h"

using namespace FabricUI;
using namespace FabricUI::DFG;
 
bool DFGAutoCompleteLineEdit_compareWords(const QString& s1,const QString& s2)
{
  if(s1.length() < s2.length())
    return true;
  if(s1.length() > s2.length())
    return false;
  return s1 < s2;
}

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
  QStringList list = words;
  qSort(list.begin(),list.end(), DFGAutoCompleteLineEdit_compareWords);
  QCompleter* completer = new QCompleter(list);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  setCompleter(completer); 
}
