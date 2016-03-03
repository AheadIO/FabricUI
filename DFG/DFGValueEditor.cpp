// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGValueEditor.h"
#include "VariablePathValueItem.h"
#include "VariablePathValueWidget.h"
#include <QtGui/QMessageBox>
#include <FTL/AutoSet.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGValueEditor::DFGValueEditor(
  DFGController * controller,
  const DFGConfig & config
  )
  : ValueEditor_Legacy::ValueEditorWidget(
    controller->getClient()
    )
  , m_config( config )
  , m_controller( controller )
{
  // todo: really the value editor should be using a notificationrouter... 
  QObject::connect(m_controller, SIGNAL(argsChanged()), this, SLOT(onArgsChanged()));
  QObject::connect(m_controller, SIGNAL(execSplitChanged()), this, SLOT(onArgsChanged()));
  QObject::connect(
    m_controller, SIGNAL(bindingChanged(FabricCore::DFGBinding const &)),
    this, SLOT(setBinding(FabricCore::DFGBinding const &))
    );
  QObject::connect(
    m_controller, SIGNAL(nodeRemoved(FTL::CStrRef, FTL::CStrRef)),
    this, SLOT(onNodeRemoved(FTL::CStrRef, FTL::CStrRef))
    );

  QObject::connect(
    this, SIGNAL(valueItemDelta(ValueItem*)),
    m_controller, SLOT(onValueItemDelta(ValueItem *))
    );
  QObject::connect(
    this, SIGNAL(valueItemInteractionEnter(ValueItem*)),
    m_controller, SLOT(onValueItemInteractionEnter(ValueItem *))
    );
  QObject::connect(
    this, SIGNAL(valueItemInteractionDelta(ValueItem*)),
    m_controller, SLOT(onValueItemInteractionDelta(ValueItem *))
    );
  QObject::connect(
    this, SIGNAL(valueItemInteractionLeave(ValueItem*)),
    m_controller, SLOT(onValueItemInteractionLeave(ValueItem *))
    );

  // register the extra widgets
  m_factory->registerEditor(&VariablePathValueWidget::creator, &VariablePathValueWidget::canDisplay);
}

DFGValueEditor::~DFGValueEditor()
{
}

void DFGValueEditor::clear()
{
  ValueEditor_Legacy::ValueEditorWidget::clear();

  m_binding.invalidate();
  m_execPath.clear();
  m_exec.invalidate();
  m_nodeName.clear();
}

void DFGValueEditor::setBinding(
  FabricCore::DFGBinding const &binding
  )
{
  m_binding = binding;
  m_execPath.clear();
  m_exec.invalidate();
  m_nodeName.clear();
  
  onArgsChanged();
}

void DFGValueEditor::setNode(
  FabricCore::DFGBinding const &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec const &exec,
  FTL::StrRef nodeName
  )
{
  m_binding = binding;
  m_execPath = execPath;
  m_exec = exec;
  m_nodeName = nodeName;
  
  onArgsChanged();
}

void DFGValueEditor::onArgsChanged()
{
  ValueEditor_Legacy::ValueEditorWidget::clear();

  try
  {
    if ( !m_exec )
    {
      FabricCore::DFGExec rootExec = m_binding.getExec();

      for(size_t i=0;i<rootExec.getExecPortCount();i++)
      {
        if(rootExec.getExecPortType(i) == FabricCore::DFGPortType_Out)
          continue;
        FTL::CStrRef portName = rootExec.getExecPortName(i);
        if(portName.size() == 0)
          continue;
        if(portName == "timeLine" || portName == "timeline")
          continue;

        FabricCore::RTVal value = m_binding.getArgValue(portName.c_str());
        if(!value.isValid())
          continue;
        value = value.copy();

        ValueItem * item = addValue( portName, value, portName, true);
        if(item)
        { 
          item->setMetaData("uiRange", rootExec.getExecPortMetadata(portName.c_str(), "uiRange"));
          item->setMetaData("uiCombo", rootExec.getExecPortMetadata(portName.c_str(), "uiCombo"));
        }
      }

      for(size_t i=0;i<rootExec.getExecPortCount();i++)
      {
        if(rootExec.getExecPortType(i) != FabricCore::DFGPortType_Out)
          continue;
        FTL::CStrRef portName = rootExec.getExecPortName(i);
        if(portName.size() == 0)
          continue;
        FabricCore::RTVal value = m_binding.getArgValue(portName.c_str());
        if(!value.isValid())
          continue;
        value = value.copy();
        addValue( portName, value, portName, false );
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
          false,
          true /* parent to root */
          );

      std::string nodePathFromRoot = m_execPath;
      if ( !nodePathFromRoot.empty() )
        nodePathFromRoot += '.';
      nodePathFromRoot += m_nodeName;

      bool isEditable = m_controller->validPresetSplit();

      if ( m_exec.getNodeType( m_nodeName.c_str() )
        == FabricCore::DFGNodeType_Inst )
      {
        FabricCore::DFGExec subExec = m_exec.getSubExec( m_nodeName.c_str() );

        for(size_t i=0;i<subExec.getExecPortCount();i++)
        {
          if(subExec.getExecPortType(i) == FabricCore::DFGPortType_Out)
            continue;

          FTL::CStrRef portName = subExec.getExecPortName(i);

          std::string portPath = m_nodeName;
          portPath += '.';
          portPath += portName;
          if ( m_exec.hasSrcPort( portPath.c_str() ) )
            continue;

          FTL::CStrRef dataType = 
            m_exec.getNodePortResolvedType( portPath.c_str() );
          if(dataType.empty() || dataType.find('$') != dataType.end())
            continue;

          FabricCore::RTVal value =
            m_exec.getInstPortResolvedDefaultValue(
              portPath.c_str(),
              dataType.c_str()
              );
          if ( value.isValid() )
            value = value.copy();
          else
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
          
          std::string portPathFromRoot = nodePathFromRoot;
          portPathFromRoot += '.';
          portPathFromRoot += portName;

          ValueItem * item =
            addValue(
              portPathFromRoot,
              value,
              portName,
              isEditable, // enabled
              true // parentToRoot
              );
          if(item)
          {
            item->setMetaData("uiRange", subExec.getExecPortMetadata(portName.data(), "uiRange"));
            item->setMetaData("uiCombo", subExec.getExecPortMetadata(portName.data(), "uiCombo"));
          }
        }
      }
      else if( m_exec.getNodeType( m_nodeName.c_str() ) == FabricCore::DFGNodeType_Var ||
        m_exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Set)
      {
        if ( m_exec.getNodeType( m_nodeName.c_str() )
          == FabricCore::DFGNodeType_Set )
        {
          std::string varPath = m_exec.getRefVarPath(m_nodeName.c_str());

          FabricCore::RTVal varPathVal = FabricCore::RTVal::ConstructString(m_controller->getClient(), varPath.c_str());

          VariablePathValueItem * item = new VariablePathValueItem(
            m_nodeName + ".variable",
            m_factory,
            &m_client,
            m_controller->getBinding(),
            m_controller->getExecPath(),
            m_treeView,
            varPathVal,
            "variable",
            isEditable
            );

          std::string portPathFromRoot = m_execPath;
          if ( !portPathFromRoot.empty() )
            portPathFromRoot += '.';
          portPathFromRoot += m_nodeName;
          portPathFromRoot += FTL_STR(".variable");

          addValue( portPathFromRoot, item, true );
        }

        std::string portPath = m_nodeName + ".value";
        FTL::CStrRef dataType =
          m_exec.getNodePortResolvedType( portPath.c_str() );

        if(!dataType.empty() && dataType.find('$') == dataType.end())
        {
          FabricCore::RTVal value =
            m_exec.getPortDefaultValue( portPath.c_str(), dataType.c_str() );

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

          if ( value.isValid() )
          {
            std::string portPathFromRoot = m_execPath;
            if ( !portPathFromRoot.empty() )
              portPathFromRoot += '.';
            portPathFromRoot += m_nodeName;
            portPathFromRoot += FTL_STR(".value");
            
            addValue( portPathFromRoot, value, "value", isEditable, true );
          }
        }
      }
      else if ( m_exec.getNodeType( m_nodeName.c_str() )
        == FabricCore::DFGNodeType_Get )
      {
        std::string varPath = m_exec.getRefVarPath( m_nodeName.c_str() );
        FabricCore::RTVal varPathVal =
          FabricCore::RTVal::ConstructString(
            m_controller->getClient(),
            varPath.c_str()
            );

        VariablePathValueItem * item =
          new VariablePathValueItem(
            m_nodeName + ".variable",
            m_factory,
            &m_client,
            m_controller->getBinding(),
            m_controller->getExecPath(),
            m_treeView,
            varPathVal,
            "variable",
            isEditable
            );

        std::string portPathFromRoot = m_execPath;
        if ( !portPathFromRoot.empty() )
          portPathFromRoot += '.';
        portPathFromRoot += m_nodeName;
        portPathFromRoot += FTL_STR(".variable");
        
        addValue( portPathFromRoot, item, true );
      }

      // expand the node level tree item
      m_treeView->setExpanded(
        m_treeModel->index(nodeItem->index(), 0),
        true
        );
    }
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}

void DFGValueEditor::onNodeRenamed(
  FTL::CStrRef execPath,
  FTL::CStrRef oldNodeName,
  FTL::CStrRef newNodeName 
  )
{
  if ( execPath == m_execPath && oldNodeName == m_nodeName )
    setNode(
      m_binding,
      m_execPath,
      m_exec,
      newNodeName
      );
}

void DFGValueEditor::onNodeRemoved(
  FTL::CStrRef execPath,
  FTL::CStrRef nodeName
  )
{
  if ( execPath == m_execPath && nodeName == m_nodeName )
    clear();
}

void DFGValueEditor::updateOutputs()
{
  FTL::AutoSet<bool> updatingOutputsAutoSet( m_updatingOutputs, true );

  if ( m_nodeName.empty() )
  {
    for ( unsigned i = 0; i < m_treeModel->numItems(); ++i )
    {
      ValueItem * item = (ValueItem*)m_treeModel->item(i);
      FabricCore::RTVal value =
        m_binding.getArgValue( item->name().c_str() );
      if ( !value.isExEQTo( item->value() ) )
      {
        item->setValue( value.copy() );
        m_treeModel->invalidateItem(item);
      }
    }
  }
  else
  {
    FabricCore::DFGExec rootExec = m_binding.getExec();

    // [pzion 20150722] Notice we start at 1 here to skip node name
    for ( unsigned i = 1; i < m_treeModel->numItems(); ++i )
    {
      ValueItem *valueItem =
        static_cast<ValueItem *>( m_treeModel->item( i ) );
      FabricCore::RTVal const &valueItemValue = valueItem->value();

      FTL::CStrRef portName = valueItem->name().rsplit('.').second;

      std::string portPath = m_nodeName;
      portPath += '.';
      portPath += portName;

      FabricCore::RTVal value =
        m_exec.getPortDefaultValue(
          portPath.c_str(),
          valueItemValue.getTypeNameCStr()
          );
      if ( !!value && !value.isExEQTo( valueItem->value() ) )
      {
        valueItem->setValue( value.copy() );
        m_treeModel->invalidateItem( valueItem );
      }
    }
  }
}
