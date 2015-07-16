// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "DFGValueEditor.h"
#include "VariablePathValueItem.h"
#include "VariablePathValueWidget.h"
#include <QtGui/QMessageBox>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGValueEditor::DFGValueEditor(
  DFGController * controller,
  const DFGConfig & config
  )
  : ValueEditor::ValueEditorWidget(
    controller->getClient()
    )
  , m_config( config )
  , m_controller( controller )
{
  // todo: really the value editor should be using a notificationrouter... 
  QObject::connect(m_controller, SIGNAL(argsChanged()), this, SLOT(onArgsChanged()));
  QObject::connect(m_controller, SIGNAL(bindingChanged()), this, SLOT(onBindingChanged()));
  QObject::connect(m_controller, SIGNAL(nodeDeleted(QString)), this, SLOT(onNodeDeleted(QString)));
  QObject::connect(this, SIGNAL(valueChanged(ValueItem*)), m_controller, SLOT( onValueChanged(ValueItem *)));

  // register the extra widgets
  m_factory->registerEditor(&VariablePathValueWidget::creator, &VariablePathValueWidget::canDisplay);
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

      FabricCore::DFGBinding binding = m_controller->getBinding();
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
      std::string prefix = m_nodeName;
      std::string globalPrefix = m_controller->getExecPath();
      if(globalPrefix.length() > 0)
        globalPrefix += ".";
      globalPrefix += prefix;

      // add an item for the node
      ValueItem * nodeItem =
        addValue(
          globalPrefix.c_str(),
          FabricCore::RTVal(),
          m_nodeName.c_str(),
          false,
          true /* parent to root */
          );

      FabricCore::DFGExec exec = m_controller->getExec();

      if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
      {
        FabricCore::DFGExec subExec = exec.getSubExec(m_nodeName.c_str());

        for(size_t i=0;i<subExec.getExecPortCount();i++)
        {
          if(subExec.getExecPortType(i) == FabricCore::DFGPortType_Out)
            continue;

          FTL::CStrRef portName = subExec.getExecPortName(i);
          std::string portPath = prefix + "." + portName.data();
          if(exec.hasSrcPort(portPath.c_str()))
            continue;

          FTL::CStrRef dataType = exec.getNodePortResolvedType(portPath.c_str());
          if(dataType.empty() || dataType.find('$') != dataType.end())
            continue;

          FTL::CStrRef hidden = subExec.getExecPortMetadata(portName.data(), "uiHidden");
          if(hidden == "true")
            continue;

          FabricCore::RTVal value = exec.getInstPortResolvedDefaultValue(portPath.c_str(), dataType.c_str());
          if(!value.isValid())
          {
            FTL::CStrRef aliasedDataType = unAliasType(dataType);
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
          
          ValueItem * item = addValue((globalPrefix + "." + portName.data()).c_str(), value, portName.data(), true);
          if(item)
          {
            item->setMetaData("uiRange", subExec.getExecPortMetadata(portName.data(), "uiRange"));
            item->setMetaData("uiCombo", subExec.getExecPortMetadata(portName.data(), "uiCombo"));
          }
        }
      }
      else if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Var ||
        exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Set)
      {
        if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Set)
        {
          std::string varPath = exec.getRefVarPath(m_nodeName.c_str());

          FabricCore::RTVal varPathVal = FabricCore::RTVal::ConstructString(m_controller->getClient(), varPath.c_str());

          VariablePathValueItem * item = new VariablePathValueItem(
            "variable",
            m_factory,
            &m_client,
            m_controller->getBinding(),
            m_controller->getExecPath(),
            m_treeView,
            varPathVal
            );
          addValue((globalPrefix+".variable").c_str(), item, true);
        }
        else
        {

        }

        std::string portPath = prefix + ".value";
        FTL::CStrRef dataType = exec.getNodePortResolvedType(portPath.c_str());

        if(!dataType.empty() && dataType.find('$') == dataType.end())
        {
          FabricCore::RTVal value = exec.getPortDefaultValue(portPath.c_str(), dataType.c_str());
          if(!value.isValid())
          {
            FTL::CStrRef aliasedDataType = unAliasType(dataType);

            if(aliasedDataType != dataType)
              value = exec.getPortDefaultValue(portPath.c_str(), aliasedDataType.c_str());
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
            addValue((globalPrefix + ".value").c_str(), value, "value", true);
        }
      }
      else if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Get)
      {
        std::string varPath = exec.getRefVarPath(m_nodeName.c_str());
        FabricCore::RTVal varPathVal = FabricCore::RTVal::ConstructString(m_controller->getClient(), varPath.c_str());

        VariablePathValueItem * item = new VariablePathValueItem(
          "variable",
          m_factory,
          &m_client,
          m_controller->getBinding(),
          m_controller->getExecPath(),
          m_treeView,
          varPathVal
          );
        addValue((globalPrefix+".variable").c_str(), item, true);
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

void DFGValueEditor::onBindingChanged()
{
  m_nodeName = "";
  onArgsChanged();
}

void DFGValueEditor::onNodeDeleted(QString nodePath)
{
  if(nodePath == m_nodeName.c_str())
  {
    setNodeName("");   
  }
}

void DFGValueEditor::updateOutputs()
{
  if(!m_nodeName.empty())
    return;

  if(!m_controller->getRouter())
    return;

  FabricCore::DFGBinding &binding = m_controller->getBinding();

  for(unsigned int i=0;i<m_treeModel->numItems();i++)
  {
    ValueItem * item = (ValueItem*)m_treeModel->item(i);
    if(item->enabled())
      continue;
    item->setValue(binding.getArgValue(item->name().toUtf8().constData()));
    m_treeModel->invalidateItem(item);
  }
}

FTL::CStrRef DFGValueEditor::unAliasType(FTL::CStrRef dataType)
{
  if(dataType == FTL_STR("Integer"))
    return FTL_STR("SInt32");
  else if(dataType == FTL_STR("Byte"))
    return FTL_STR("UInt8");
  else if(dataType == FTL_STR("Size") || dataType == FTL_STR("Count") || dataType == FTL_STR("Index"))
    return FTL_STR("UInt32");
  else if(dataType == FTL_STR("Scalar"))
    return FTL_STR("Float32");
  else return dataType;
}
