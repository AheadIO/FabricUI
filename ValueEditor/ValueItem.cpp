// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "ValueItem.h"
#include "ValueWidget.h"
#include <assert.h>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

ValueItem::ValueItem(QString name, TreeEditorFactory * factory, FabricCore::Client * client, QWidget * parent, FabricCore::RTVal value, QString label, bool enabled)
: WidgetTreeItem(name, factory, "ValueItem", label, parent)
{
  m_isSettingValue = false;
  m_value = value;
  if(m_value.isValid())
    m_valueTypeName = m_value.getTypeName().getStringCString();
  m_client = client;
  m_enabled = enabled;
}

ValueItem::~ValueItem()
{
}

void ValueItem::setValue(FabricCore::RTVal v)
{
  if(m_isSettingValue)
    return;

  m_isSettingValue = true;
  m_value = v;
  m_needsUpdate = true;
  onDataChanged();
  m_isSettingValue = false;
}

QString ValueItem::valueTypeName() const
{
  return m_valueTypeName;
}

bool ValueItem::enabled() const
{
  return m_enabled;
}

FabricCore::Client * ValueItem::client() const
{
  return m_client;
}

void ValueItem::onUIChanged()
{
  if(!editor())
    return;

  m_value = ((ValueWidget*)editor())->value();

  if ( !!m_valueAtInteractionEnter )
    emit valueItemInteractionDelta( this );
  else emit valueItemDelta( this );
}

void ValueItem::onDataChanged()
{
  if(!editor())
    return;
  ((ValueWidget*)editor())->setValue(m_value);
}

void ValueItem::setMetaData(const char * key, const char * value)
{
  if(key == NULL || value == NULL)
    return;
  if(strlen(key) == 0 || strlen(value) == 0)
    return;

  std::map<std::string, std::string>::iterator it = m_metaData.find(key);
  if(it == m_metaData.end())
    m_metaData.insert(std::pair<std::string, std::string>(key, value));
  else
    it->second = value;
}

const char * ValueItem::getMetaData(const char * key)
{
  std::map<std::string, std::string>::iterator it = m_metaData.find(key);
  if(it == m_metaData.end())
    return NULL;
  return it->second.c_str();
}

void ValueItem::onBeginInteraction( ValueItem * item )
{
  assert( !m_valueAtInteractionEnter );

  FabricCore::RTVal typeNameRTVal = m_value.getTypeName();
  char const *typeNameCStr = typeNameRTVal.getStringCString();

  m_valueAtInteractionEnter =
    m_value.callMethod(
      typeNameCStr,
      "clone",
      0, NULL
      );
}

void ValueItem::onEndInteraction( ValueItem * item )
{
  assert( !!m_valueAtInteractionEnter );

  emit valueItemInteractionLeave( this );

  m_valueAtInteractionEnter.invalidate();
}

