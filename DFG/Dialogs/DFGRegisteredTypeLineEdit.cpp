// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGRegisteredTypeLineEdit.h"
#include <FTL/CStrRef.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGRegisteredTypeLineEdit::DFGRegisteredTypeLineEdit(QWidget * parent, FabricCore::Client & client, QString text)
: DFGAutoCompleteLineEdit(parent, text)
{
  QStringList words;
  FabricCore::Variant registeredTypesVar = FabricCore::GetRegisteredTypes_Variant(client);
  for(FabricCore::Variant::DictIter keyIter(registeredTypesVar); !keyIter.isDone(); keyIter.next())
  {
    FTL::CStrRef key = keyIter.getKey()->getStringData();
    if(key.find('<') != key.end())
      continue;
    if(key.find('[') != key.end())
      continue;
    words.append(key.c_str());
  }
  setWords(words);
}
