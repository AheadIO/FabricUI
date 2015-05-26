// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGExecPortListItem.h"

using namespace FabricUI::DFG;

DFGExecPortListItem::DFGExecPortListItem(QListWidget * parent, QString portType, QString dataType, QString name)
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

DFGExecPortListItem::~DFGExecPortListItem()
{
}

QString DFGExecPortListItem::portType() const
{
  return m_portType;
}

QString DFGExecPortListItem::dataType() const
{
  return m_dataType;
}

QString DFGExecPortListItem::name() const
{
  return m_name;
}

