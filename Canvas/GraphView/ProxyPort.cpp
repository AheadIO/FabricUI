// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/Canvas/GraphView/ProxyPort.h>
#include <FabricUI/Canvas/GraphView/SidePanel.h>
#include <FabricUI/Canvas/GraphView/Graph.h>
#include <FabricUI/Canvas/GraphView/Pin.h>
#include <FabricUI/Canvas/GraphView/GraphConfig.h>

#include <QtGui/QGraphicsLinearLayout>

using namespace FabricUI::GraphView;

ProxyPort::ProxyPort(SidePanel * parent, PortType portType)
: ConnectionTarget(parent->itemGroup())
{
  m_sidePanel = parent;
  m_portType = portType;

  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

  const GraphConfig & config = graph()->config();

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(config.sidePanelPortLabelSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  m_circle = new PinCircle(this, m_portType, color(), true);
  m_label = new TextContainer(this, "Expose", config.sidePanelFontColor, config.sidePanelFontHighlightColor, config.sidePanelFont);

  if(m_portType == PortType_Input)
  {
    layout->addItem(m_circle);
    layout->setAlignment(m_circle, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addItem(m_label);
    layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addStretch(1);
  }
  else if(m_portType == PortType_Output)
  {
    layout->addStretch(1);
    layout->addItem(m_label);
    layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);
    layout->addItem(m_circle);
    layout->setAlignment(m_circle, Qt::AlignHCenter | Qt::AlignVCenter);
  }
}


SidePanel * ProxyPort::sidePanel()
{
  return m_sidePanel;
}

const SidePanel * ProxyPort::sidePanel() const
{
  return m_sidePanel;
}

Graph * ProxyPort::graph()
{
  return sidePanel()->graph();
}

const Graph * ProxyPort::graph() const
{
  return sidePanel()->graph();
}

PinCircle * ProxyPort::circle()
{
  return m_circle;
}

const PinCircle * ProxyPort::circle() const
{
  return m_circle;
}

PortType ProxyPort::portType() const
{
  return m_portType;
}

QColor ProxyPort::color() const
{
  return QColor(0, 0, 0, 255);
}

bool ProxyPort::highlighted() const
{
  return m_highlighted;
}

void ProxyPort::setHighlighted(bool state)
{
  if(m_highlighted != state)
  {
    m_circle->setHighlighted(state);
    m_label->setHighlighted(state);
    m_highlighted = state;
  }
}

bool ProxyPort::canConnectTo(
  ConnectionTarget * other,
  std::string &failureReason
  ) const
{
  switch(other->targetType())
  {
    case TargetType_Pin:
    {
      Pin * otherPin = (Pin *)other;
      if ( this == other
        || portType() == PortType_Input
        || otherPin->portType() == PortType_Output )
        return false;
      return true;
    }
    case TargetType_NodeHeader:
    {
      return true;
    }
    default:
      return false;
  }
}

QPointF ProxyPort::connectionPos(PortType pType) const
{
  return m_circle->centerInSceneCoords();
}
