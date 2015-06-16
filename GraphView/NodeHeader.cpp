// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/GraphView/NodeHeader.h>
#include <FabricUI/GraphView/Node.h>
#include <FabricUI/GraphView/NodeLabel.h>
#include <FabricUI/GraphView/Graph.h>

#include <QtGui/QGraphicsLinearLayout>
#include <QtGui/QPushButton>
#include <QtGui/QGraphicsProxyWidget>

using namespace FabricUI::GraphView;

NodeHeader::NodeHeader(
  Node * parent,
  QString const &text
  )
: QGraphicsWidget(parent->mainWidget())
{
  m_node = parent;
  Graph * graph = m_node->graph();

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

  QGraphicsLinearLayout * layout = new QGraphicsLinearLayout();
  float contentsMargins = graph->config().nodeHeaderContentMargins;
  float nodeWidthReduction = graph->config().nodeWidthReduction * 0.5;

  layout->setContentsMargins(contentsMargins + nodeWidthReduction, contentsMargins + graph->config().nodeHeaderSpaceTop, contentsMargins + nodeWidthReduction, contentsMargins + graph->config().nodeHeaderSpaceBottom);
  layout->setSpacing(graph->config().nodeHeaderSpacing);
  layout->setOrientation(Qt::Horizontal);
  setLayout(layout);

  m_title = new NodeLabel(this, text, graph->config().nodeFontColor, graph->config().nodeFontHighlightColor, graph->config().nodeFont);

  layout->addStretch(1);
  layout->addItem(m_title);
  layout->setAlignment(m_title, Qt::AlignHCenter | Qt::AlignTop);
  layout->addStretch(1);
}

Node * NodeHeader::node()
{
  return m_node;
}

const Node * NodeHeader::node() const
{
  return m_node;
}

void NodeHeader::setTitle(QString const &title)
{
  m_title->setText(title);
}

bool NodeHeader::highlighted() const
{
  return m_title->highlighted();
}

void NodeHeader::setHighlighted(bool state)
{
  m_title->setHighlighted(state);
}
