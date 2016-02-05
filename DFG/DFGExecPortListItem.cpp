// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGExecPortListItem.h"

using namespace FabricUI::DFG;

DFGExecPortListItem::DFGExecPortListItem(QListWidget * parent, char const * portType, char const * dataType, char const * name)
: QListWidgetItem(parent)
{
  m_portType = portType;
  m_dataType = dataType;
  m_name = name;

  std::string text = m_portType + " " + m_name;
  if(m_dataType.length() > 0)
    text += " ( " + m_dataType + " )";
  setText(text.c_str());
}

DFGExecPortListItem::~DFGExecPortListItem()
{
}

char const * DFGExecPortListItem::portType() const
{
  return m_portType.c_str();
}

char const * DFGExecPortListItem::dataType() const
{
  return m_dataType.c_str();
}

char const * DFGExecPortListItem::name() const
{
  return m_name.c_str();
}

