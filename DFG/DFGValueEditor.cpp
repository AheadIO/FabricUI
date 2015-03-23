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

FabricServices::DFGWrapper::Node DFGValueEditor::getNode()
{
  return m_node;
}

void DFGValueEditor::setNode(FabricServices::DFGWrapper::Node node)
{
  m_node = node;
  onArgsChanged();
}

void DFGValueEditor::onArgsChanged()
{
  clear();

  try
  {
    if(!m_node.isValid())
    {
      DFGWrapper::Binding binding = m_controller->getView()->getGraph().getWrappedCoreBinding();
      std::vector<DFGWrapper::Port> ports = binding.getGraph().getPorts();
      for(uint32_t i=0;i<ports.size();i++)
      {
        if(ports[i].getPortType() == FabricCore::DFGPortType_Out)
          continue;
        if(ports[i].getName().length() == 0)
          continue;
        if(ports[i].getName() == "timeLine" || ports[i].getName() == "timeline")
          continue;
        std::string path = ports[i].getPath();
        FabricCore::RTVal value = binding.getArgValue(path.c_str());
        if(!value.isValid())
          continue;
        addValue(path.c_str(), value, ports[i].getName().c_str(), true);
      }
      for(uint32_t i=0;i<ports.size();i++)
      {
        if(ports[i].getPortType() != FabricCore::DFGPortType_Out)
          continue;
        std::string path = ports[i].getPath();
        FabricCore::RTVal value = binding.getArgValue(path.c_str());
        if(!value.isValid())
          continue;
        addValue(path.c_str(), value, ports[i].getName().c_str(), false);
      }
    }
    else
    {
      // add an item for the node
      ValueItem * nodeItem = addValue(m_node.getPath().c_str(), FabricCore::RTVal(), m_node.getPath().c_str(), false);

      std::vector<DFGWrapper::Pin> pins = m_node.getPins();
      for(size_t i=0;i<pins.size();i++)
      {
        if(pins[i].getPinType() == FabricCore::DFGPortType_Out)
          continue;
        if(pins[i].isConnected())
          continue;

        std::string dataType = pins[i].getDataType();
        if(dataType == "" || dataType.find('$') != std::string::npos)
          continue;

        FabricCore::RTVal value = pins[i].getDefaultValue(dataType.c_str());
        
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
            value = pins[i].getDefaultValue(aliasedDataType.c_str());
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
        addValue(pins[i].getPath().c_str(), value, pins[i].getName().c_str(), true);
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
  if(m_node.isValid())
    return;

  DFGWrapper::Binding binding = m_controller->getView()->getGraph().getWrappedCoreBinding();

  for(unsigned int i=0;i<m_treeModel->numItems();i++)
  {
    ValueItem * item = (ValueItem*)m_treeModel->item(i);
    if(item->enabled())
      continue;
    item->setValue(binding.getArgValue(item->name().toUtf8().constData()));
    m_treeModel->invalidateItem(item);
  }
}
