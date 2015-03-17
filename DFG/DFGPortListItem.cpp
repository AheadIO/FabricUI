// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGPortListItem.h"

using namespace FabricUI::DFG;

DFGPortListItem::DFGPortListItem(QListWidget * parent, QString portType, QString dataType, QString name)
: QListWidgetItem(parent)
{
  m_portType = portType;
  m_dataType = dataType;
  m_name = name;

  QString text = m_portType + " " + m_name;
  if(m_dataType.length() > 0)
    text += " ( " + m_dataType + " )";
  setText(text);
}

DFGPortListItem::~DFGPortListItem()
{
}

QString DFGPortListItem::portType() const
{
  return m_portType;
}

QString DFGPortListItem::dataType() const
{
  return m_dataType;
}

QString DFGPortListItem::name() const
{
  return m_name;
}

