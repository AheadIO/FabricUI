// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/Pin.h>
#include <FabricUI/GraphView/PinCircle.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/GraphConfig.h>
#include <FabricUI/GraphView/ProxyPort.h>

#include <QtGui/QGraphicsLinearLayout>

using namespace FabricUI::GraphView;

Pin::Pin(
  Node * parent,
  FTL::StrRef name,
  PortType pType,
  QColor color,
  FTL::StrRef label
  )
  : ConnectionTarget(parent->pinsWidget())
  , m_node( parent )
  , m_name( name.data(), name.size() )
  , m_labelCaption( label.data(), label.size() )
{
  m_portType = pType;
  if ( m_labelCaption.empty() )
    m_labelCaption = m_name;
  m_color = color;
  m_index = 0;

  // [pzion 20160425] Force setDrawState to work
  m_drawState = false;
  setDrawState( true );

  const GraphConfig & config = node()->graph()->config();

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();

  if ( portType() == PortType_Output )
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

    QGraphicsItem *graphicsItem = m_inCircle;
    while ( graphicsItem )
    {
      if ( QGraphicsObject *graphicsObject = graphicsItem->toGraphicsObject() )
      {
        connect(
          graphicsObject, SIGNAL(xChanged()),
          this, SIGNAL(inCircleScenePositionChanged())
          );
        connect(
          graphicsObject, SIGNAL(yChanged()),
          this, SIGNAL(inCircleScenePositionChanged())
          );
      }
      if ( graphicsItem == m_node )
        break;
      graphicsItem = graphicsItem->parentItem();
    }
  }

  if(portType() != PortType_Input)
  {
    layout->addStretch(1);

    if(config.nodePinStretch > 0.0f && portType() != PortType_IO)
    {
      QGraphicsWidget * stretch = new QGraphicsWidget(this);
      stretch->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
      stretch->setMinimumWidth(portType() == PortType_IO ? config.nodePinStretch * 0.5 : config.nodePinStretch);
      stretch->setMaximumHeight(1.0f);
      layout->addItem(stretch);
    }
  }

  if(m_labelCaption.length() > 0)
  {
    m_label = new PinLabel(this, QSTRING_FROM_STL_UTF8(m_labelCaption), config.pinFontColor, config.pinFontHighlightColor, config.pinFont);

    layout->addItem(m_label);
    layout->setAlignment(m_label, Qt::AlignHCenter | Qt::AlignVCenter);
  }

  if(portType() != PortType_Output)
  {
    if(config.nodePinStretch > 0.0f && portType() != PortType_IO)
    {
      QGraphicsWidget * stretch = new QGraphicsWidget(this);
      stretch->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
      stretch->setMinimumWidth(portType() == PortType_IO ? config.nodePinStretch * 0.5 : config.nodePinStretch);
      stretch->setMaximumHeight(1.0f);
      layout->addItem(stretch);
    }

    layout->addStretch(1);
  }

  m_outCircle = new PinCircle(this, PortType_Output, m_color);
  layout->addItem(m_outCircle);
  layout->setAlignment(m_outCircle, Qt::AlignRight | Qt::AlignVCenter);
  if(portType() == PortType_Input)
  {
    m_outCircle->setClipping(true);
  }
  setDaisyChainCircleVisible(false);
  QGraphicsItem const *graphicsItem = m_outCircle;
  while ( graphicsItem )
  {
    if ( QGraphicsObject const *graphicsObject = graphicsItem->toGraphicsObject() )
    {
      connect(
        graphicsObject, SIGNAL(xChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
      connect(
        graphicsObject, SIGNAL(yChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
    }
    if ( graphicsItem == m_node )
      break;
    graphicsItem = graphicsItem->parentItem();
  }

  graphicsItem = m_node->header()->inCircle();
  while ( graphicsItem )
  {
    if ( QGraphicsObject const *graphicsObject = graphicsItem->toGraphicsObject() )
    {
      connect(
        graphicsObject, SIGNAL(xChanged()),
        this, SIGNAL(inCircleScenePositionChanged())
        );
      connect(
        graphicsObject, SIGNAL(yChanged()),
        this, SIGNAL(inCircleScenePositionChanged())
        );
    }
    if ( graphicsItem == m_node )
      break;
    graphicsItem = graphicsItem->parentItem();
  }

  graphicsItem = m_node->header()->outCircle();
  while ( graphicsItem )
  {
    if ( QGraphicsObject const *graphicsObject = graphicsItem->toGraphicsObject() )
    {
      connect(
        graphicsObject, SIGNAL(xChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
      connect(
        graphicsObject, SIGNAL(yChanged()),
        this, SIGNAL(outCircleScenePositionChanged())
        );
    }
    if ( graphicsItem == m_node )
      break;
    graphicsItem = graphicsItem->parentItem();
  }
}

Graph * Pin::graph()
{
  return node()->graph();
}

const Graph * Pin::graph() const
{
  return node()->graph();
}

std::string Pin::path() const
{
  std::string result = node()->name();
  result += graph()->config().pathSep;
  result += m_name;
  return result;
}

char const * Pin::label() const
{
  return m_labelCaption.c_str();
}

PortType Pin::portType() const
{
  return m_portType;
}

QColor Pin::color() const
{
  return m_color;
}

void Pin::setColor(QColor color, bool quiet, bool performUpdate)
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
  if(performUpdate)
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

void Pin::setDataType(FTL::CStrRef dataType)
{
  m_dataType = dataType;
  setToolTip(dataType.c_str());

  // automatically change the label for array pins
  if(m_label)
  {
    if(m_dataType.length() > 2)
    {
      if(m_dataType.substr(m_dataType.length()-2) == "[]" && m_labelSuffix != "[]")
      {
        m_labelSuffix = "[]";
        m_label->setText(QSTRING_FROM_STL_UTF8(m_labelCaption + m_labelSuffix));
        return;
      }
    }
    if(m_labelSuffix.length() > 0)
    {
      m_labelSuffix = "";
      m_label->setText(QSTRING_FROM_STL_UTF8(m_labelCaption + m_labelSuffix));
    }
  }
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
  std::string &failureReason
  ) const
{
  switch(other->targetType())
  {
    case TargetType_Pin:
    {
      Pin * otherPin = (Pin *)other;
      if ( this == otherPin
        || otherPin->node() == this->node()
        || otherPin->portType() == PortType_Output )
        return false;
      return m_node->graph()->controller()->canConnectTo(
        path().c_str(), otherPin->path().c_str(), failureReason
        );
    }
    case TargetType_Port:
    {
      Port * otherPort = (Port *)other;
      if ( this == other
        || otherPort->portType() == PortType_Output )
        return false;
      return m_node->graph()->controller()->canConnectTo(
        path().c_str(), otherPort->path().c_str(), failureReason
        );
    }
    case TargetType_ProxyPort:
    {
      ProxyPort * otherProxyPort = (ProxyPort *)other;
      if ( this == other
        || otherProxyPort->portType() == PortType_Output )
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

QPointF Pin::connectionPos(PortType pType) const
{
  PinCircle const *pinCircle;
  if ( !drawState() )
  {
    if( pType == PortType_Input )
      pinCircle = node()->header()->inCircle();
    else
      pinCircle = node()->header()->outCircle();
  }
  else
  {
    if( pType == PortType_Input )
      pinCircle = inCircle();
    else
      pinCircle = outCircle();
  }

  if ( pinCircle )
    return pinCircle->centerInSceneCoords();
  else
    return QPointF();
}

void Pin::setDrawState(bool flag)
{
  if ( m_drawState != flag )
  {
    m_drawState = flag;
    setVisible( m_drawState );

    if ( m_drawState )
    {
      setMaximumHeight( 1000 );
      setSizePolicy(
        QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed )
        );
    }
    else
    {
      setMaximumHeight( 0 );
      setSizePolicy(
        QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed )
        );
    }

    emit drawStateChanged();
  }
}

void Pin::setDaisyChainCircleVisible(bool flag)
{
  if( portType() == PortType_Input )
    m_outCircle->setDaisyChainCircleVisible( flag );
}

void Pin::setName( FTL::StrRef newName )
{
  if ( newName != m_name )
  {
    bool labelIsName = m_labelCaption == m_name;
    m_name = newName;
    if ( labelIsName )
      m_labelCaption = newName;
    m_label->setText( QSTRING_FROM_STL_UTF8(m_labelCaption + m_labelSuffix) );
  }
}
