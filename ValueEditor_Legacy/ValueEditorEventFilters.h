// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueEditorEventFilter__
#define __UI_ValueEditor_ValueEditorEventFilter__

#include <QtCore/QEvent> 

#include "ValueEditorWidget.h"
#include "ValueWidget.h"

namespace FabricUI
{

  namespace ValueEditor_Legacy
  {
    class EventFilter : public QObject
    {
      Q_OBJECT

      public:

        EventFilter(ValueWidget * itemWidget);
        virtual bool eventFilter(QObject *object, QEvent *event);

        ValueWidget * itemWidget() { return m_itemWidget; }

      protected:

        ValueWidget * m_itemWidget;
    };

    class TabEventFilter : public EventFilter
    {
      Q_OBJECT

      public:

        TabEventFilter(ValueWidget * itemWidget);
        virtual bool eventFilter(QObject *object, QEvent *event);
    };

    class BackspaceDeleteEventFilter : public EventFilter
    {
      Q_OBJECT

      public:

        BackspaceDeleteEventFilter(ValueWidget * itemWidget);
        virtual bool eventFilter(QObject *object, QEvent *event);
    };

  };

};

#endif // __UI_ValueEditor_ValueEditorEventFilter__
