// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGGraphViewWidget.h"
#include "DFGController.h"
#include "Dialogs/DFGGetStringDialog.h"
#include "Dialogs/DFGNewVariableDialog.h"
#include "DFGUICmdHandler.h"
#include <FabricCore.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGGraphViewWidget::DFGGraphViewWidget(
  QWidget * parent,
  const GraphView::GraphConfig & config,
  GraphView::Graph * graph
  )
  : GraphView::GraphViewWidget(
    parent,
    config,
    graph
    )
{
}

void DFGGraphViewWidget::setGraph(GraphView::Graph * graph)
{
  GraphView::GraphViewWidget::setGraph(graph);
  if(graph)
    graph->setCentralOverlayText("Press TAB to insert nodes.");
}

void DFGGraphViewWidget::dropEvent(QDropEvent *event)
{
  DFGController *controller =
    static_cast<DFGController *>( graph()->controller() );

  std::string json = event->mimeData()->text().toUtf8().constData();

  graph()->controller()->beginInteraction();

  QPointF pos(event->pos().x(), event->pos().y());
  pos = graph()->itemGroup()->mapFromScene(pos);

  try
  {
    FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(json.c_str());
    std::vector <std::string> droppedNodes;
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
            std::string node = "";

            if(std::string(typeVar->getStringData()) == "DFGPreset")
            {
              const FabricCore::Variant * pathVar = dictVar->getDictValue("path");
              if(pathVar->isString())
              {
                node = controller->cmdAddInstFromPreset(
                        pathVar->getStringData(),
                        pos
                        );
                pos += QPointF(30, 30);
              }
            }

            else if(std::string(typeVar->getStringData()) == "DFGVariable")
            {
              DFGController* controller = (DFGController*)graph()->controller();
              FabricCore::Client client = controller->getClient();
              FabricCore::DFGBinding binding = controller->getBinding();

              const FabricCore::Variant * pathVar = dictVar->getDictValue("path");
              std::string path = pathVar->getStringData();
              std::string execPath = controller->getExecPath();

              while(execPath.length() > 0)
              {
                size_t delimPos = execPath.find('.');
                if(delimPos != std::string::npos)
                {
                  std::string prefix = execPath.substr(0, delimPos+1);
                  execPath = execPath.substr(delimPos+1);
                  if(path.substr(0, prefix.length()) == prefix)
                  {
                    path = path.substr(delimPos+1);
                  }
                  else
                    break;
                }
                else if(execPath + "." == path.substr(0, execPath.length() + 1))
                {
                  path = path.substr(execPath.length() + 1);
                  execPath = "";
                }
                else
                  break;
              }

              if(event->keyboardModifiers().testFlag(Qt::ShiftModifier) || 
                event->mouseButtons().testFlag(Qt::RightButton))
              {
                node = controller->cmdAddSet(
                        "",
                        path.c_str(),
                        pos
                        );
              }
              else
              {
                node = controller->cmdAddGet(
                        "",
                        path.c_str(),
                        pos
                        );
              }
            }

            if (!node.empty())
              droppedNodes.push_back(node);
          }
        }
      }
      if (droppedNodes.size())
      {
        graph()->clearSelection();
        for (unsigned i=0;i<droppedNodes.size();i++)
        {
          if (GraphView::Node *uiNode = graph()->node(droppedNodes[i]))
            uiNode->setSelected(true);
        }
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    controller->logError(e.getDesc_cstr());
  }

  graph()->controller()->endInteraction();
  event->acceptProposedAction();
}
