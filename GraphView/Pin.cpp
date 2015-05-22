// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "Pin.h"
#include "Node.h"
#include "Graph.h"
#include "GraphConfig.h"
#include "ProxyPort.h"

#include <QtGui/QGraphicsLinearLayout>

using namespace FabricUI::GraphView;

Pin::Pin(Node * parent, QString name, PortType pType, QColor color, QString label)
: ConnectionTarget(parent->pinsWidget())
{
  m_node = parent;
  m_name = name;
  m_portType = pType;
  m_labelCaption = label;
  if(m_labelCaption.length() == 0)
    m_labelCaption = m_name;
  m_color = color;
  m_index = 0;
  m_drawState = true;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  const GraphConfig & config = node()->graph()->config();

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();

  if(portType() == PortType_Input)
    layout->setContentsMargins(0, 0, config.nodeWidthReduction * 0.5, 0);
  else if(portType() == PortType_Output)
    layout->setContentsMargins(config.nodeWidthReduction * 0.5, 0, 0, 0);
  else
    layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(config.pinLabelSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  m_inCircle = NULL;
  m_outCircle = NULL;

  if(portType() == PortType_Input || portType() == PortType_IO)
  {
    m_inCircle = new PinCircle(this, PortType_Input, m_color);
    layout->addItem(m_inCircle);
    layout->setAlignment(m_inCircle, Qt::AlignLeft | Qt::AlignVCenter);
  }

  if(portType() != PortType_Input)
    layout->addStretch(2);

  if(m_labelCaption.length() > 0)
  {
    m_label = new PinLabel(this, m_labelCaption, config.pinFontColor, config.pinFontHighlightColor, config.pinFont);

    layout->addItem(m_label);
    layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);
  }

  if(portType() != PortType_Output)
    layout->addStretch(2);

  if(portType() == PortType_Output || portType() == PortType_IO)
  {
    m_outCircle = new PinCircle(this, PortType_Output, m_color);
    layout->addItem(m_outCircle);
    layout->setAlignment(m_outCircle, Qt::AlignRight | Qt::AlignVCenter);
  }

}

Node * Pin::node()
{
  return m_node;
}

const Node * Pin::node() const
{
  return m_node;
}

Graph * Pin::graph()
{
  return node()->graph();
}

const Graph * Pin::graph() const
{
  return node()->graph();
}

QString Pin::name() const
{
  return m_name;
}

QString Pin::path() const
{
  return node()->path() + graph()->config().pathSep + name();
}

QString Pin::label() const
{
  return m_labelCaption;
}

PortType Pin::portType() const
{
  return m_portType;
}

QColor Pin::color() const
{
  return m_color;
}

void Pin::setColor(QColor color, bool quiet)
{
  if(inCircle())
    inCircle()->setColor(color);
  if(outCircle())
    outCircle()->setColor(color);
  if(m_color != color)
  {
    m_color = color;
    if(!quiet)
      emit colorChanged(this, color);
  }
  update();
}

int Pin::index() const
{
  return m_index;
}

void Pin::setIndex(int i)
{
  m_index = i;
}

bool Pin::highlighted() const
{
  return m_highlighted;
}

void Pin::setHighlighted(bool state)
{
  m_highlighted = state;
  if(inCircle())
    inCircle()->setHighlighted(state);
  if(outCircle())
    outCircle()->setHighlighted(state);
  if(m_label)
    m_label->setHighlighted(state);
  setColor(m_color, true);
}

QString Pin::dataType() const
{
  return m_dataType;
}

void Pin::setDataType(QString type)
{
  m_dataType = type;
  setToolTip(type);
}

PinCircle * Pin::inCircle()
{
  return m_inCircle;
}

const PinCircle * Pin::inCircle() const
{
  return m_inCircle;
}

PinCircle * Pin::outCircle()
{
  return m_outCircle;
}

const PinCircle * Pin::outCircle() const
{
  return m_outCircle;
}

bool Pin::canConnectTo(
  ConnectionTarget * other,
  QString &failureReason
  ) const
{
  switch(other->targetType())
  {
    case TargetType_Pin:
    {
      Pin * otherPin = (Pin *)other;
      if ( this == otherPin
        || otherPin->node() == this->node()
        || portType() == PortType_Input
        || otherPin->portType() == PortType_Output )
        return false;
      return m_node->graph()->controller()->canConnectTo(
        path(), otherPin->path(), failureReason
        );
    }
    case TargetType_Port:
    {
      Port * otherPort = (Port *)other;
      if ( this == other
        || portType() == PortType_Input
        || otherPort->portType() == PortType_Output )
        return false;
      return m_node->graph()->controller()->canConnectTo(
        path(), otherPort->path(), failureReason
        );
    }
    case TargetType_ProxyPort:
    {
      ProxyPort * otherProxyPort = (ProxyPort *)other;
      if ( this == other
        || portType() == PortType_Input
        || otherProxyPort->portType() == PortType_Output )
        return false;
      return true;
    }
    default:
      return false;
  }
}

QPointF Pin::connectionPos(PortType pType) const
{
  if(!drawState())
  {
    QPointF p;
    if(pType == PortType_Input)
    {
      p = node()->header()->boundingRect().topLeft();
      p += node()->header()->boundingRect().bottomLeft();
      p *= 0.5f;
      p += QPointF(node()->graph()->config().nodeWidthReduction * 0.5f, 0.0f);
    }
    else
    {
      p = node()->header()->boundingRect().topRight();
      p += node()->header()->boundingRect().bottomRight();
      p *= 0.5f;
      p -= QPointF(node()->graph()->config().nodeWidthReduction * 0.5f, 0.0f);
    }
    return node()->header()->mapToScene(p);
  }
  if(pType == PortType_Input)
  {
    if(inCircle())
      return inCircle()->centerInSceneCoords();
  }
  else
  {
    if(outCircle())
      return outCircle()->centerInSceneCoords();
  }
  return QPointF();
}

void Pin::setDrawState(bool flag)
{
  m_drawState = flag;
  setVisible(m_drawState);

  if(m_drawState)
  {
    setMaximumHeight(1000);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  }
  else
  {
    setMaximumHeight(0);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  }

  update();
}

bool Pin::drawState() const
{
  return m_drawState;
}
