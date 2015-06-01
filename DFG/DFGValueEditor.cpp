// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <DFGWrapper/Binding.h>
#include <DFGWrapper/InstPort.h>

#include "DFGValueEditor.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGValueEditor::DFGValueEditor(QWidget * parent, DFGController * controller, const DFGConfig & config)
: ValueEditor::ValueEditorWidget(parent, controller->getClient())
{
  m_config = config;
  m_controller = controller;

  // todo: really the value editor should be using a dfgview... 
  QObject::connect(m_controller, SIGNAL(argsChanged()), this, SLOT(onArgsChanged()));
  QObject::connect(this, SIGNAL(valueChanged(ValueItem*)), m_controller, SLOT( onValueChanged(ValueItem *)));
}

DFGValueEditor::~DFGValueEditor()
{
}

FabricServices::DFGWrapper::NodePtr DFGValueEditor::getNode()
{
  return m_node;
}

void DFGValueEditor::setNode(FabricServices::DFGWrapper::NodePtr node)
{
  m_node = node;
  onArgsChanged();
}

void DFGValueEditor::onArgsChanged()
{
  clear();

  try
  {
    if(!m_node)
    {
      if(!m_controller->getView())
        return;

      DFGWrapper::Binding binding = m_controller->getView()->getGraph()->getDFGBinding();
      DFGWrapper::ExecPortList ports = binding.getExecutable()->getExecPorts();
      for(uint32_t i=0;i<ports.size();i++)
      {
        if(ports[i]->getNodePortType() != FabricCore::DFGPortType_Out)
          continue;
        std::string portName = ports[i]->getPortName();
        if(portName.length() == 0)
          continue;
        if(portName == "timeLine" || portName == "timeline")
          continue;
        QString hidden = ports[i]->getMetadata("uiHidden");
        if(hidden == "true")
          continue;
        char const * path = ports[i]->getPortPath();
        FabricCore::RTVal value = binding.getArgValue(path);
        if(!value.isValid())
          continue;
        ValueItem * item = addValue(path, value, ports[i]->getPortName(), true);
        if(item)
        {
          item->setMetaData("uiRange", ports[i]->getMetadata("uiRange"));
          item->setMetaData("uiCombo", ports[i]->getMetadata("uiCombo"));
        }
      }
      for(uint32_t i=0;i<ports.size();i++)
      {
        if(ports[i]->getNodePortType() == FabricCore::DFGPortType_Out)
          continue;
        QString hidden = ports[i]->getMetadata("uiHidden");
        if(hidden == "true")
          continue;
        char const * path = ports[i]->getPortPath();
        FabricCore::RTVal value = binding.getArgValue(path);
        if(!value.isValid())
          continue;
        addValue(path, value, ports[i]->getPortName(), false);
      }
    }
    else
    {
      // add an item for the node
      ValueItem * nodeItem = addValue(m_node->getNodeName(), FabricCore::RTVal(), m_node->getNodeName(), false);

      DFGWrapper::NodePortList nodePorts = m_node->getNodePorts();
      for(size_t i=0;i<nodePorts.size();i++)
      {
        if ( !nodePorts[i]->isInstPort() )
          continue;
        DFGWrapper::InstPortPtr instPort =
          DFGWrapper::InstPortPtr::StaticCast( nodePorts[i] );

        if(instPort->getNodePortType() == FabricCore::DFGPortType_Out)
          continue;
        if(instPort->isConnectedToAny())
          continue;

        std::string dataType = instPort->getResolvedType();
        if(dataType == "" || dataType.find('$') != std::string::npos)
          continue;

        QString hidden = instPort->getExecPort()->getMetadata("uiHidden");
        if(hidden == "true")
          continue;

        FabricCore::RTVal value = instPort->getDefaultValue(dataType.c_str());
        if(!value.isValid())
          value = instPort->getExecPort()->getDefaultValue(dataType.c_str());
        
        if(!value.isValid())
        {
          std::string aliasedDataType;
          if(dataType == "Integer")
            aliasedDataType = "SInt32";
          else if(dataType == "Byte")
            aliasedDataType = "UInt8";
          else if(dataType == "Size" || dataType == "Count" || dataType == "Index")
            aliasedDataType = "UInt32";
          else if(dataType == "Scalar")
            aliasedDataType = "Float32";

          if(aliasedDataType.length() > 0)
            value = instPort->getDefaultValue(aliasedDataType.c_str());
        }

        if(!value.isValid())
        {
          bool isObject = FabricCore::GetRegisteredTypeIsObject(*m_controller->getClient(), dataType.c_str());
          if(isObject)
          {
            value = FabricCore::RTVal::Create(*m_controller->getClient(), dataType.c_str(), 0, 0);
          }
          else
          {
            value = FabricCore::RTVal::Construct(*m_controller->getClient(), dataType.c_str(), 0, 0);
          }
        }
        if(!value.isValid())
          continue;
        
        ValueItem * item = addValue(instPort->getPortPath(), value, instPort->getPortName(), true);
        if(item)
        {
          item->setMetaData("uiRange", instPort->getExecPort()->getMetadata("uiRange"));
          item->setMetaData("uiCombo", instPort->getExecPort()->getMetadata("uiCombo"));
        }
      }

      // expand the node level tree item
      m_treeView->setExpanded(m_treeModel->index(nodeItem->index(), 0), true);
    }
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}

void DFGValueEditor::updateOutputs()
{
  if(m_node)
    return;

  if(!m_controller->getView())
    return;

  DFGWrapper::Binding binding = m_controller->getView()->getGraph()->getDFGBinding();

  for(unsigned int i=0;i<m_treeModel->numItems();i++)
  {
    ValueItem * item = (ValueItem*)m_treeModel->item(i);
    if(item->enabled())
      continue;
    item->setValue(binding.getArgValue(item->name().toUtf8().constData()));
    m_treeModel->invalidateItem(item);
  }
}
