// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGGraphViewWidget.h"
#include "DFGController.h"
#include <FabricCore.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGGraphViewWidget::DFGGraphViewWidget(QWidget * parent, const GraphView::GraphConfig & config, GraphView::GraphFactory * factory, GraphView::Graph * graph)
: GraphView::GraphViewWidget(parent, config, factory, graph)
{
}

void DFGGraphViewWidget::dropEvent(QDropEvent *event)
{
  std::string json = event->mimeData()->text().toUtf8().constData();

  graph()->controller()->beginInteraction();

  QPointF pos(event->pos().x(), event->pos().y());
  pos = graph()->itemGroup()->mapFromScene(pos);

  try
  {
    FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(json.c_str());
    if(jsonVar.isArray())
    {
      for(uint32_t i=0;i<jsonVar.getArraySize();i++)
      {
        const FabricCore::Variant * dictVar = jsonVar.getArrayElement(i);
        if(dictVar->isDict())
        {
          const FabricCore::Variant * typeVar = dictVar->getDictValue("type");
          if(typeVar->isString())
          {
            if(std::string(typeVar->getStringData()) == "DFGPreset")
            {
              const FabricCore::Variant * pathVar = dictVar->getDictValue("path");
              if(pathVar->isString())
              {
                ((DFGController*)graph()->controller())->addNodeFromPreset(graph()->path(), pathVar->getStringData(), pos);
                pos += QPointF(30, 30);
              }
            }
          }
        }
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    ((DFGController*)graph()->controller())->logError(e.getDesc_cstr());
  }

  graph()->controller()->endInteraction();
  event->acceptProposedAction();
}
