// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ValueItem__
#define __UI_ValueEditor_ValueItem__

#include <FabricUI/TreeView/WidgetTreeItem.h>
#include <map>

#include <FabricCore.h>

namespace FabricUI
{

  namespace ValueEditor
  {
    class ValueItem : public TreeView::WidgetTreeItem
    {
      Q_OBJECT

    public:

      ValueItem(
        FTL::StrRef name,
        TreeView::TreeEditorFactory * factory,
        FabricCore::Client * client,
        QWidget * parent,
        FabricCore::RTVal value,
        FTL::StrRef label = FTL::StrRef(),
        bool enabled = true
        );
      virtual ~ValueItem();

      virtual FTL::CStrRef type() const { return FTL_STR("ValueItem"); }

      FabricCore::RTVal &value()
        { return m_value; }
      FabricCore::RTVal &valueAtInteractionEnter()
        { return m_valueAtInteractionEnter; }
      void setValue(FabricCore::RTVal v);
      QString valueTypeName() const;
      bool enabled() const;

      FabricCore::Client * client() const;

      void setMetaData(const char * key, const char * value);
      const char * getMetaData(const char * key);

    signals:

      void valueItemDelta( ValueItem *valueItem );

      void valueItemInteractionEnter( ValueItem *valueItem );
      void valueItemInteractionDelta( ValueItem *valueItem );
      void valueItemInteractionLeave( ValueItem *valueItem );

    public slots:

      void onBeginInteraction(ValueItem * item);
      void onEndInteraction(ValueItem * item);
      void onDialogAccepted();
      void onDialogCanceled();
      void onFilePathChosen(const QString & filePath);
      void onColorChosen(const QColor & color);

    private:

      FabricCore::Client * m_client;
      FabricCore::RTVal m_value;
      QString m_valueTypeName;
      std::map<std::string, std::string> m_metaData;
      bool m_enabled;
      bool m_isSettingValue;
      FabricCore::RTVal m_valueAtInteractionEnter;
    };

  };

};

#endif // __UI_ValueEditor_ValueItem__
