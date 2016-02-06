// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGAutoCompleteLineEdit__
#define __UI_DFG_DFGAutoCompleteLineEdit__

#include <QtGui/QLineEdit>

namespace FabricUI
{
  namespace DFG
  {

    class DFGAutoCompleteLineEdit : public QLineEdit
    {
      Q_OBJECT

    public:

      DFGAutoCompleteLineEdit(QWidget * parent, QString text = "");
      virtual ~DFGAutoCompleteLineEdit();

      virtual void setWords(const QStringList & words);
    };

  };

};

#endif // __UI_DFG_DFGAutoCompleteLineEdit__
