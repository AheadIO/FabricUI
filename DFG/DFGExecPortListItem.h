// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGExecPortListItem__
#define __UI_DFG_DFGExecPortListItem__

#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

namespace FabricUI
{

  namespace DFG
  {

    class DFGExecPortListItem : public QListWidgetItem 
    {
    public:

      DFGExecPortListItem(QListWidget * parent, QString portType, QString dataType, QString name);
      virtual ~DFGExecPortListItem();

      QString portType() const;
      QString dataType() const;
      QString name() const;

    private:

      QString m_portType;
      QString m_dataType;
      QString m_name;
    };

  };

};

#endif // __UI_DFG_DFGExecPortListItem__
