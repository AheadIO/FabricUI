// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "GraphRelaxer.h"

using namespace FabricUI::GraphView;

GraphRelaxer::GraphRelaxer(const GraphConfig & config)
: m_config(config)
{
}

unsigned int GraphRelaxer::numNodes() const
{
  return (unsigned int)m_nodes.size(); 
}

QString GraphRelaxer::getName(int index)
{
  return m_nodes[index].name;
}

QPointF GraphRelaxer::getPos(int index)
{
  return m_nodes[index].curr;
}

int GraphRelaxer::addNode(QString name, QPointF pos, int col, int row, float width, float height)
{
  RelaxNode node;
  node.index = (int)m_nodes.size();
  node.col = col;
  node.row = row;
  node.width = width;
  node.height = height;
  node.name = name;
  node.prev = node.curr = pos;
  node.vel = QPointF(0.0f, 0.0f);

  m_nodeLookup.insert(RelaxNodeLookupPair(node.name, node.index));
  m_nodes.push_back(node);

  return node.index;
}

void GraphRelaxer::addSpring(QString a, QString b, float force)
{
  if(a == b)
    return;

  RelaxNodeLookupIt itA = m_nodeLookup.find(a);
  if(itA == m_nodeLookup.end())
    return;
  RelaxNodeLookupIt itB = m_nodeLookup.find(b);
  if(itB == m_nodeLookup.end())
    return;

  RelaxNode & nodeA = m_nodes[itA->second];
  RelaxNode & nodeB = m_nodes[itB->second];

  nodeA.springs.push_back(RelaxSpring(nodeA.index, nodeB.index, force));
  nodeB.springs.push_back(RelaxSpring(nodeB.index, nodeA.index, force));
}

void GraphRelaxer::relax(int steps)
{
  // todo: base this off config
  float gravity = -30.0f;
  float drag = 0.98f;
  float blend = 0.25f;
  float restLength = 120.0f;
  float rowHeightInc = 5.0f;
  float colWidthInc = 2.0f;
  float idealForce = 1.0f;

  // prepare the row counters
  std::vector<int> colCount;
  std::vector<float> width;
  std::vector<float> height;
  for(unsigned int i=0;i<m_nodes.size();i++)
  {
    int col = m_nodes[i].col;
    while(col >= colCount.size())
    {
      colCount.push_back(0);
      width.push_back(0.0f);
      height.push_back(0.0f);
    }
    colCount[col]++;
    if(width[col] < m_nodes[i].width)
      width[col] = m_nodes[i].width;
    if(height[col] < m_nodes[i].height)
      height[col] = m_nodes[i].height;
  }

  // sort each row by prev row and prev pins


  for(int stepI = 0; stepI < steps; stepI++)
  {
    for(int nodeI = 0; nodeI < m_nodes.size(); nodeI++)
    {
      RelaxNode & node = m_nodes[nodeI];
      if(node.col == 0)
        continue;
      
      float totalColWidth = 0.0;
      for(int colI = 0; colI < node.col; colI++)
        totalColWidth += width[colI+1] + colWidthInc;

      float rowHeight = height[node.col] + rowHeightInc;

      // QPointF force(0.0f, 0.0f);
      // for(int springI = 0; springI < node.springs.size(); springI++)
      // {
      //   RelaxSpring & spring = node.springs[springI];

      //   QPointF a = m_nodes[spring.a].prev;
      //   QPointF b = m_nodes[spring.b].prev;
      //   QPointF delta = a - b;
      //   float length = sqrt(delta.x() * delta.x() + delta.y() * delta.y());
      //   delta *= (restLength - length) / length;
      //   force += delta * spring.force;
      // }

      QPointF idealPos(m_nodes[0].prev.x() - totalColWidth,
        m_nodes[0].prev.y() + float(node.row) * rowHeight - float(colCount[node.col]-1) * 0.5 * rowHeight);

      // force += (idealPos - node.prev) * idealForce;
      // force += QPointF(gravity, 0.0);
      // force *= drag;
      // node.vel = node.vel * (1.0 - blend) + force * blend;
      // node.curr += node.vel;

      node.curr = idealPos;
    }

    for(int nodeI = 0; nodeI < m_nodes.size(); nodeI++)
    {
      RelaxNode & node = m_nodes[nodeI];
      node.prev = node.curr;
    }

    // we aren't using any sim anymore
    break;
  }
}
