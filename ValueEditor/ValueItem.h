// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueItem__
#define __UI_ValueEditor_ValueItem__

#include <TreeView/WidgetTreeItem.h>

#include <FabricCore.h>

namespace FabricUI
{

  namespace ValueEditor
  {
    class ValueItem : public TreeView::WidgetTreeItem
    {
      Q_OBJECT

    public:

      ValueItem(QString name, TreeView::TreeEditorFactory * factory, FabricCore::Client * client, FabricCore::RTVal value, QString label = "", bool enabled = true);
      virtual ~ValueItem();

      FabricCore::RTVal value() const;
      void setValue(FabricCore::RTVal v);
      QString valueTypeName() const;
      bool enabled() const;

      FabricCore::Client * client() const;

      void onUIChanged();
      void onDataChanged();

    signals:

      void beginInteraction(ValueItem * item);
      void valueChanged(ValueItem * item);
      void endInteraction(ValueItem * item);

    private:

      FabricCore::Client * m_client;
      FabricCore::RTVal m_value;
      QString m_valueTypeName;
      bool m_enabled;
      bool m_isSettingValue;
    };

  };

};

#endif // __UI_ValueEditor_ValueItem__
