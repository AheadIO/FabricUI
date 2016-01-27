// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "ValueItem.h"
#include "ValueWidget.h"
#include "ColorValueWidget.h"
#include "ValueEditorWidget.h"
#include <assert.h>
#include <FabricUI/Util/QString_Conversion.h>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor_Legacy;

ValueItem::ValueItem(
  FTL::StrRef name,
  TreeEditorFactory * factory,
  FabricCore::Client * client,
  QWidget * parent,
  FabricCore::RTVal value,
  FTL::StrRef label,
  bool enabled
  )
  : WidgetTreeItem(
    name,
    factory,
    label,
    parent
    )
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

  if ( !!m_valueAtInteractionEnter )
    emit valueItemInteractionDelta( this );
  else emit valueItemDelta( this );

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
  
  m_valueAtInteractionEnter = m_value.copy();
}

void ValueItem::onEndInteraction( ValueItem * item, bool cancel )
{
  assert( !!m_valueAtInteractionEnter );

  if(cancel)
    m_value = m_valueAtInteractionEnter;

  emit valueItemInteractionLeave( this );

  m_valueAtInteractionEnter.invalidate();
}

void ValueItem::onDialogAccepted()
{
  onEndInteraction(this);

  updatePixmap();
}

void ValueItem::onDialogCanceled()
{
  onEndInteraction(this, true /* cancel */);

  updatePixmap();
}

void ValueItem::onFilePathChosen(const QString & filePath)
{
  if(filePath.length() == 0)
    return;

  try
  {
    FabricCore::RTVal stringVal = FabricCore::RTVal::ConstructString(*m_client, QSTRING_TO_CONST_CHAR_VALUE(filePath));
    FabricCore::RTVal filePathVal = FabricCore::RTVal::Create(*m_client, "FilePath", 1, &stringVal);
    setValue(filePathVal);
  }
  catch(FabricCore::Exception e)
  {
    printf("%s\n", e.getDesc_cstr());
  }

  updatePixmap();
}

void ValueItem::onColorChosen(const QColor & color)
{
  try
  {
    float r = color.redF();
    float g = color.greenF();
    float b = color.blueF();
    float a = color.alphaF();
    setValue(ColorValueWidget::genRtVal(this, m_valueTypeName.toUtf8().constData(), r, g, b, a));
  }
  catch(FabricCore::Exception e)
  {
    printf("%s\n", e.getDesc_cstr());
  }

  ValueWidget * widget = (ValueWidget *)editor();
  if(widget)
    widget->onValueItemDelta(this);

  updatePixmap();
}
