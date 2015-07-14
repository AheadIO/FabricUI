// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGGraphViewWidget.h"
#include "DFGController.h"
#include "Dialogs/DFGGetStringDialog.h"
#include "Dialogs/DFGNewVariableDialog.h"
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
                ((DFGController*)graph()->controller())->addDFGNodeFromPreset(
                  pathVar->getStringData(), pos
                  );
                pos += QPointF(30, 30);
              }
            }
            else if(std::string(typeVar->getStringData()) == "DFGVariable")
            {
              DFGController* controller = (DFGController*)graph()->controller();
              FabricCore::Client client = controller->getClient();
              FabricCore::DFGBinding binding = controller->getCoreDFGBinding();

              const FabricCore::Variant * pathVar = dictVar->getDictValue("path");
              std::string path = pathVar->getStringData();
              std::string execPath = controller->getCoreDFGExecPath();

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
                ((DFGController*)graph()->controller())->addDFGSet(
                  "", 
                  path.c_str(),
                  pos
                  );
                  pos += QPointF(30, 30);
              }
              else
              {
                ((DFGController*)graph()->controller())->addDFGGet(
                  "", 
                  path.c_str(),
                  pos
                  );
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
