// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGValueEditor.h"
#include <QtGui/QMessageBox>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGValueEditor::DFGValueEditor(
  QWidget * parent,
  DFGController * controller,
  const DFGConfig & config
  )
  : ValueEditor::ValueEditorWidget(
    parent,
    controller->getClient()
    )
  , m_config( config )
  , m_controller( controller )
{
  // todo: really the value editor should be using a notificationrouter... 
  QObject::connect(m_controller, SIGNAL(argsChanged()), this, SLOT(onArgsChanged()));
  QObject::connect(this, SIGNAL(valueChanged(ValueItem*)), m_controller, SLOT( onValueChanged(ValueItem *)));
}

DFGValueEditor::~DFGValueEditor()
{
}

void DFGValueEditor::setNodeName( FTL::StrRef nodeName )
{
  m_nodeName = nodeName;
  onArgsChanged();
}

void DFGValueEditor::onArgsChanged()
{
  clear();

  try
  {
    if(m_nodeName.empty())
    {
      if(!m_controller->getRouter())
        return;

      FabricCore::DFGBinding binding = m_controller->getCoreDFGBinding();
      FabricCore::DFGExec exec = binding.getExec();

      for(size_t i=0;i<exec.getExecPortCount();i++)
      {
        if(exec.getExecPortType(i) == FabricCore::DFGPortType_Out)
          continue;
        FTL::StrRef portName = exec.getExecPortName(i);
        if(portName.size() == 0)
          continue;
        if(portName == "timeLine" || portName == "timeline")
          continue;
        FTL::StrRef uiHidden = exec.getExecPortMetadata(portName.data(), "uiHidden");
        if(uiHidden == "true")
          continue;

        FabricCore::RTVal value = binding.getArgValue(portName.data());
        if(!value.isValid())
          continue;

        ValueItem * item = addValue(portName.data(), value, portName.data(), true);
        if(item)
        {
          item->setMetaData("uiRange", exec.getExecPortMetadata(portName.data(), "uiRange"));
          item->setMetaData("uiCombo", exec.getExecPortMetadata(portName.data(), "uiCombo"));
        }
      }

      for(size_t i=0;i<exec.getExecPortCount();i++)
      {
        if(exec.getExecPortType(i) != FabricCore::DFGPortType_Out)
          continue;
        FTL::StrRef portName = exec.getExecPortName(i);
        if(portName.size() == 0)
          continue;
        FTL::StrRef uiHidden = exec.getExecPortMetadata(portName.data(), "uiHidden");
        if(uiHidden == "true")
          continue;
        FabricCore::RTVal value = binding.getArgValue(portName.data());
        if(!value.isValid())
          continue;
        addValue(portName.data(), value, portName.data(), false);
      }
    }
    else
    {
      // add an item for the node
      ValueItem * nodeItem =
        addValue(
          m_nodeName.c_str(),
          FabricCore::RTVal(),
          m_nodeName.c_str(),
          false
          );

      FabricCore::DFGExec exec = m_controller->getCoreDFGExec();

      if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
      {
        FabricCore::DFGExec subExec = exec.getSubExec(m_nodeName.c_str());

        std::string prefix = m_nodeName;
        prefix += ".";

        for(size_t i=0;i<subExec.getExecPortCount();i++)
        {
          if(subExec.getExecPortType(i) == FabricCore::DFGPortType_Out)
            continue;

          FTL::StrRef portName = subExec.getExecPortName(i);
          std::string portPath = prefix + portName.data();
          if(exec.isConnectedToAny(portPath.c_str()))
            continue;

          std::string dataType = exec.getNodePortResolvedType(portPath.c_str());
          if(dataType == "" || dataType.find('$') != std::string::npos)
            continue;

          FTL::StrRef hidden = subExec.getExecPortMetadata(portName.data(), "uiHidden");
          if(hidden == "true")
            continue;

          FabricCore::RTVal value = exec.getInstPortResolvedDefaultValue(portPath.c_str(), dataType.c_str());
          if(!value.isValid())
          {
            std::string aliasedDataType = unAliasType(dataType);
            if(aliasedDataType != dataType)
              value = exec.getInstPortResolvedDefaultValue(portPath.c_str(), aliasedDataType.c_str());
          }

          if(!value.isValid())
          {
            bool isObject = FabricCore::GetRegisteredTypeIsObject(m_controller->getClient(), dataType.c_str());
            if(isObject)
            {
              value = FabricCore::RTVal::Create(m_controller->getClient(), dataType.c_str(), 0, 0);
            }
            else
            {
              value = FabricCore::RTVal::Construct(m_controller->getClient(), dataType.c_str(), 0, 0);
            }
          }
          if(!value.isValid())
            continue;
          
          ValueItem * item = addValue(portPath.data(), value, portName.data(), true);
          if(item)
          {
            item->setMetaData("uiRange", subExec.getExecPortMetadata(portName.data(), "uiRange"));
            item->setMetaData("uiCombo", subExec.getExecPortMetadata(portName.data(), "uiCombo"));
          }
        }
      }
      else if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Var)
      {
        std::string portPath = m_nodeName + ".value";
        std::string dataType = exec.getNodePortResolvedType(portPath.c_str());
        FabricCore::RTVal value = exec.getPortDefaultValue(portPath.c_str(), dataType.c_str());
        if(!value.isValid())
        {
          std::string aliasedDataType = unAliasType(dataType);
          if(aliasedDataType != dataType)
            value = exec.getInstPortResolvedDefaultValue(portPath.c_str(), aliasedDataType.c_str());
        }

        if(!value.isValid())
        {
          bool isObject = FabricCore::GetRegisteredTypeIsObject(m_controller->getClient(), dataType.c_str());
          if(isObject)
          {
            value = FabricCore::RTVal::Create(m_controller->getClient(), dataType.c_str(), 0, 0);
          }
          else
          {
            value = FabricCore::RTVal::Construct(m_controller->getClient(), dataType.c_str(), 0, 0);
          }
        }

        if(value.isValid())
          addValue(portPath.c_str(), value, "value", true);
      }
      else if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Get)
      {
        QMessageBox msg(QMessageBox::Warning, "Fabric Code", "DFGValueEditor: To be implemented: DFGNodeType_Get");
        msg.addButton("Ok", QMessageBox::AcceptRole);
        msg.exec();
      }
      else if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Set)
      {
        QMessageBox msg(QMessageBox::Warning, "Fabric Code", "DFGValueEditor: To be implemented: DFGNodeType_Set");
        msg.addButton("Ok", QMessageBox::AcceptRole);
        msg.exec();
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
  if(!m_nodeName.empty())
    return;

  if(!m_controller->getRouter())
    return;

  FabricCore::DFGBinding binding = m_controller->getCoreDFGBinding();

  for(unsigned int i=0;i<m_treeModel->numItems();i++)
  {
    ValueItem * item = (ValueItem*)m_treeModel->item(i);
    if(item->enabled())
      continue;
    item->setValue(binding.getArgValue(item->name().toUtf8().constData()));
    m_treeModel->invalidateItem(item);
  }
}

std::string DFGValueEditor::unAliasType(const std::string & type)
{
  if(type == "Integer")
    return "SInt32";
  else if(type == "Byte")
    return "UInt8";
  else if(type == "Size" || type == "Count" || type == "Index")
    return "UInt32";
  else if(type == "Scalar")
    return "Float32";
  return type;
}
