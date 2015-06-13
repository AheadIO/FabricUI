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
            else if(std::string(typeVar->getStringData()) == "DFGVar")
            {
              DFGController* controller = (DFGController*)graph()->controller();
              FabricCore::Client client = controller->getClient();
              FabricCore::DFGBinding binding = controller->getCoreDFGBinding();

              DFGNewVariableDialog dialog(this, client, binding);
              if(dialog.exec() != QDialog::Accepted)
                return;

              QString name = dialog.name();
              if(name.length() == 0)
                return;
              QString dataType = dialog.dataType();
              QString extension = dialog.extension();

              ((DFGController*)graph()->controller())->addDFGVar(
                name.toUtf8().constData(), 
                dataType.toUtf8().constData(), 
                extension.toUtf8().constData(), 
                pos
                );
                pos += QPointF(30, 30);
            }
            else if(std::string(typeVar->getStringData()) == "DFGGet")
            {
              ((DFGController*)graph()->controller())->addDFGGet(
                "", "", pos
                );
                pos += QPointF(30, 30);
            }
            else if(std::string(typeVar->getStringData()) == "DFGSet")
            {
              ((DFGController*)graph()->controller())->addDFGSet(
                "", "", pos
                );
                pos += QPointF(30, 30);
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
