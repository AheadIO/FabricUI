// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGPortListItem__
#define __UI_DFG_DFGPortListItem__

#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>

namespace FabricUI
{

  namespace DFG
  {

    class DFGPortListItem : public QListWidgetItem 
    {
    public:

      DFGPortListItem(QListWidget * parent, QString portType, QString dataType, QString name);
      virtual ~DFGPortListItem();

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

#endif // __UI_DFG_DFGPortListItem__