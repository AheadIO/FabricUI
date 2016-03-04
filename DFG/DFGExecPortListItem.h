// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

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

      DFGExecPortListItem(
        QListWidget * parent, 
        char const * portType, 
        char const * dataType, 
        char const * name);
      virtual ~DFGExecPortListItem();

      char const * portType() const;
      char const * dataType() const;
      char const * name() const;

    private:

      std::string m_portType;
      std::string m_dataType;
      std::string m_name;
    };

  };

};

#endif // __UI_DFG_DFGExecPortListItem__
