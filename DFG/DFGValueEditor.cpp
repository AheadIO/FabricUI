// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <DFGWrapper/Binding.h>

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
      DFGWrapper::Binding binding = m_controller->getView()->getGraph()->getWrappedCoreBinding();
      DFGWrapper::PortList ports = binding.getExecutable()->getPorts();
      for(uint32_t i=0;i<ports.size();i++)
      {
        if(ports[i]->getEndPointType() != FabricCore::DFGPortType_Out)
          continue;
        std::string portName = ports[i]->getName();
        if(portName.length() == 0)
          continue;
        if(portName == "timeLine" || portName == "timeline")
          continue;
        char const * path = ports[i]->getEndPointPath();
        FabricCore::RTVal value = binding.getArgValue(path);
        if(!value.isValid())
          continue;
        addValue(path, value, ports[i]->getName(), true);
      }
      for(uint32_t i=0;i<ports.size();i++)
      {
        if(ports[i]->getEndPointType() == FabricCore::DFGPortType_Out)
          continue;
        char const * path = ports[i]->getEndPointPath();
        FabricCore::RTVal value = binding.getArgValue(path);
        if(!value.isValid())
          continue;
        addValue(path, value, ports[i]->getName(), false);
      }
    }
    else
    {
      // add an item for the node
      ValueItem * nodeItem = addValue(m_node->getNodePath(), FabricCore::RTVal(), m_node->getNodePath(), false);

      DFGWrapper::PinList pins = m_node->getPins();
      for(size_t i=0;i<pins.size();i++)
      {
        if(pins[i]->getEndPointType() == FabricCore::DFGPortType_Out)
          continue;
        if(pins[i]->isConnectedToAny())
          continue;

        std::string dataType = pins[i]->getResolvedType();
        if(dataType == "" || dataType.find('$') != std::string::npos)
          continue;

        FabricCore::RTVal value = pins[i]->getDefaultValue(dataType.c_str());
        
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
            value = pins[i]->getDefaultValue(aliasedDataType.c_str());
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
        addValue(pins[i]->getEndPointPath(), value, pins[i]->getName(), true);
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

  DFGWrapper::Binding binding = m_controller->getView()->getGraph()->getWrappedCoreBinding();

  for(unsigned int i=0;i<m_treeModel->numItems();i++)
  {
    ValueItem * item = (ValueItem*)m_treeModel->item(i);
    if(item->enabled())
      continue;
    item->setValue(binding.getArgValue(item->name().toUtf8().constData()));
    m_treeModel->invalidateItem(item);
  }
}
