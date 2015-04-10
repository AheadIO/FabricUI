// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Port.h"
#include "SidePanel.h"
#include "Graph.h"
#include "Pin.h"
#include "GraphConfig.h"

#include <QtGui/QGraphicsLinearLayout>

using namespace FabricUI::GraphView;

Port::Port(SidePanel * parent, QString name, PortType portType, QString dataType, QColor color, QString label)
: ConnectionTarget(parent)
{
  m_sidePanel = parent;
  m_name = parent->getUniqueName(name);
  m_path = m_name;
  // if(parent->graph()->path().length() > 0)
  //   m_path = parent->graph()->path() + parent->graph()->config().pathSep + m_path;
  m_portType = portType;
  m_dataType = dataType;
  m_color = color;
  m_labelCaption = label;
  if(m_labelCaption.length() == 0)
    m_labelCaption = m_name;

  init();
}

void Port::init()
{
  setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding));

  const GraphConfig & config = graph()->config();

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(config.sidePanelPortLabelSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  m_label = new TextContainer(this, m_labelCaption, config.sidePanelFontColor, config.sidePanelFontHighlightColor, config.sidePanelFont);
  m_circle = new PinCircle(this, m_portType, color());

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

SidePanel * Port::sidePanel()
{
  return m_sidePanel;
}

const SidePanel * Port::sidePanel() const
{
  return m_sidePanel;
}

Graph * Port::graph()
{
  return sidePanel()->graph();
}

const Graph * Port::graph() const
{
  return sidePanel()->graph();
}

QString Port::name() const
{
  return m_name;
}

void Port::setName(QString n)
{
  if(m_name == m_labelCaption)
  {
    m_labelCaption = m_sidePanel->getUniqueName(n, true /* isLabel */);
    m_label->setText(m_labelCaption);
  }
  m_name = n;
  // m_path = graph()->path() + graph()->config().pathSep + m_name;
  m_path = m_name;
  update();
}

QString Port::path() const
{
  return m_path;
}

QString Port::label() const
{
  return m_labelCaption;
}

void Port::setLabel(QString n)
{
  m_labelCaption = m_sidePanel->getUniqueName(n, true /* isLabel */);
  m_label->setText(m_labelCaption);
  update();
}

QColor Port::color() const
{
  return m_color;
}

PortType Port::portType() const
{
  return m_portType;
}

QString Port::dataType() const
{
  return m_dataType;
}

void Port::setDataType(QString dt)
{
  m_dataType = dt;
  setToolTip(dt);
}

void Port::setColor(QColor color)
{
  m_color = color;
  m_circle->setColor(m_color);
}

bool Port::highlighted() const
{
  return m_highlighted;
}

void Port::setHighlighted(bool state)
{
  if(m_highlighted != state)
  {
    m_circle->setHighlighted(state);
    m_label->setHighlighted(state);
    m_highlighted = state;
  }
}

bool Port::canConnectTo(
  ConnectionTarget * other,
  QString &failureReason
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
      return m_sidePanel->graph()->controller()->canConnectTo(path(), otherPin->path(), failureReason);
    }
    case TargetType_Port:
    {
      Port * otherPort = (Port *)other;
      if ( this == otherPort
        || portType() == PortType_Input
        || otherPort->portType() == PortType_Output )
        return false;
      if(path() == otherPort->path())
        return false;
      return m_sidePanel->graph()->controller()->canConnectTo(path(), otherPort->path(), failureReason);
    }
    default:
      return false;
  }
}

QPointF Port::connectionPos(PortType pType) const
{
  return m_circle->centerInSceneCoords();
}

