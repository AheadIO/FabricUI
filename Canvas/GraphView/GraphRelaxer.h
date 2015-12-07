// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphRelaxer__
#define __UI_GraphView_GraphRelaxer__

#include <QtCore/QPointF>

#include "GraphConfig.h"
#include <FTL/CStrRef.h>
#include <string>
#include <vector>
#include <map>

namespace FabricUI
{

  namespace GraphView
  {

    class GraphRelaxer : public QObject
    {
      Q_OBJECT

    public:

      struct RelaxSpring
      {
        int a;
        int b;
        float force;

        RelaxSpring(int in_a, int in_b, float in_force)
        {
          a = in_a;
          b = in_b;
          force = in_force;
        }
      };

      struct RelaxNode
      {
        int index;
        int col;
        int row;
        float width;
        float height;
        std::string name;
        QPointF curr;
        QPointF prev;
        QPointF vel;
        std::vector<RelaxSpring> springs;
      };

      GraphRelaxer(const GraphConfig & config = GraphConfig());
      unsigned int numNodes() const;
      FTL::CStrRef getName(int index);
      QPointF getPos(int index);
      int addNode(
        FTL::StrRef name,
        QPointF pos,
        int col,
        int row,
        float width,
        float height
        );
      void addSpring(FTL::StrRef a, FTL::StrRef b, float force = 1.0);
      void relax(int steps);

    private:

      typedef std::vector<RelaxNode> RelaxNodeList;
      typedef std::map<std::string, int> RelaxNodeLookup;
      typedef RelaxNodeLookup::iterator RelaxNodeLookupIt;
      typedef std::pair<std::string, int> RelaxNodeLookupPair;

      RelaxNodeList m_nodes;
      RelaxNodeLookup m_nodeLookup;
      GraphConfig m_config;

    };

  };

};

#endif // __UI_GraphView_GraphRelaxer__
