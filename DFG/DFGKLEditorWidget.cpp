// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QFontMetrics>
#include <QtGui/QSplitter>
#include <QtGui/QMessageBox>

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/SidePanel.h>

#include "DFGErrorsWidget.h"
#include "DFGKLEditorWidget.h"
#include "DFGWidget.h"

#include <FTL/AutoSet.h>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGKLEditorWidget::DFGKLEditorWidget(
  QWidget * parent,
  DFGController * controller,
  ASTWrapper::KLASTManager * manager,
  const DFGConfig & config
  )
  : QWidget( parent )
  , m_controller( controller )
  , m_config( config )
  , m_unsavedChanges( false )
  , m_isSettingPorts( false )
{
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setMinimumSize(QSize(300, 250));

  setFont(config.fixedFont);

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  QWidget * buttonsWidget = new QWidget(this);
  buttonsWidget->setContentsMargins(0, 0, 0, 0);
  buttonsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
  QHBoxLayout * buttonsLayout = new QHBoxLayout();
  buttonsLayout->setContentsMargins(0, 0, 0, 0);
  buttonsWidget->setLayout(buttonsLayout);

  QPalette buttonsPal(buttonsWidget->palette());
  buttonsPal.setColor(QPalette::Background, config.graphConfig.headerBackgroundColor);
  buttonsWidget->setAutoFillBackground(true);
  buttonsWidget->setPalette(buttonsPal);
  buttonsWidget->setContentsMargins(config.graphConfig.headerMargins, config.graphConfig.headerMargins, config.graphConfig.headerMargins, config.graphConfig.headerMargins);
  buttonsWidget->setFont(config.graphConfig.nodeFont);

  QPushButton * saveButton = new QPushButton("Save", buttonsWidget);
  QPushButton * reloadButton = new QPushButton("Reload", buttonsWidget);
  buttonsLayout->addStretch(2);
  buttonsLayout->addWidget(reloadButton);
  buttonsLayout->addWidget(saveButton);


  QPalette buttonPal(saveButton->palette());
  buttonPal.setColor(QPalette::ButtonText, config.graphConfig.headerFontColor);
  buttonPal.setColor(QPalette::Button, config.graphConfig.nodeDefaultColor);
  // saveButton->setAutoFillBackground(false);
  saveButton->setPalette(buttonPal);
  reloadButton->setPalette(buttonPal);

  QSplitter * splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);
  splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setChildrenCollapsible(false);

  // [Julien] FE-5276 Validate the names in port creation and avoid illegal characters
  m_ports = new DFGKLEditorPortTableWidget( splitter, controller, config );
  QObject::connect(
    this, SIGNAL(execChanged()),
    m_ports, SLOT(refresh())
    );
  QObject::connect(
    m_ports, SIGNAL(execPortsChanged()),
    this, SLOT(onExecPortsChanged())
    );

  m_klEditor = new KLEditor::KLEditorWidget(splitter, manager, config.klEditorConfig);

  splitter->addWidget(m_ports);
  splitter->setStretchFactor(0, 1);
  splitter->addWidget(m_klEditor);
  splitter->setStretchFactor(1, 7);

  layout->addWidget(buttonsWidget);
  layout->addWidget(splitter);

  QObject::connect(
    controller, SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
  QObject::connect(
    controller, SIGNAL(execSplitChanged()),
    this, SLOT(onExecSplitChanged())
    );
  QObject::connect(saveButton, SIGNAL(clicked()), this, SLOT(save()));
  QObject::connect(reloadButton, SIGNAL(clicked()), this, SLOT(reload()));
  QObject::connect(m_klEditor->sourceCodeWidget(), SIGNAL(newUnsavedChanged()), this, SLOT(onNewUnsavedChanges()));

  updateDiags();
}

DFGKLEditorWidget::~DFGKLEditorWidget()
{
}

void DFGKLEditorWidget::onExecChanged()
{
  if(m_isSettingPorts)
    return;
  
  FabricCore::DFGExec &exec = m_controller->getExec();
  bool isEditable = m_controller->validPresetSplit();
  setEnabled(isEditable);

  if ( exec.getType() == FabricCore::DFGExecType_Func )
  {
    show();      
    m_klEditor->sourceCodeWidget()->setFocus();

    try
    {
      QString code = getExec().getCode();
      if ( code.length() == 0 )
        code = "dfgEntry {\n  //result = lhs + rhs;\n}\n";
      m_klEditor->sourceCodeWidget()->setCodeAndExec( code, &exec );
    }
    catch ( FabricCore::Exception e )
    {
      m_controller->logError(e.getDesc_cstr());
    }

    m_unsavedChanges = false;

    updateDiags();

    emit execChanged();
  }
  else
  {
    hide();
  }
}

void DFGKLEditorWidget::onExecPortsChanged()
{
  FTL::AutoSet<bool> isSettingPorts(m_isSettingPorts, true);

  std::vector<DFGKLEditorPortTableWidget::PortInfo> infos;
  for(unsigned int i=0;i<m_ports->nbPorts();i++)
    infos.push_back(m_ports->portInfo(i));

  bool modified = false;

  FabricCore::DFGExec &exec = getExec();

  // we expect only one change, so either a new port,
  // portName / portType or dataType change
  if(exec.getExecPortCount() == infos.size())
  {
    for(size_t i=0;i<infos.size();i++)
    {
      bool addRemovePort = false;
      bool setPortType = false;
      bool setDataType = false;
      if(infos[i].portName != exec.getExecPortName(i))
      {
        if(infos[i].portType == exec.getExecPortType(i) &&
          infos[i].dataType == exec.getExecPortTypeSpec(i))
        {
          try
          {
            FTL::CStrRef oldName = exec.getExecPortName(i);
            FTL::CStrRef desiredNewName = infos[i].portName;
            m_controller->cmdRenameExecPort(
              QString::fromUtf8( oldName.data(), oldName.size() ),
              QString::fromUtf8( desiredNewName.data(), desiredNewName.size() )
              );
          }
          catch(FabricCore::Exception e)
          {
            m_controller->logError(e.getDesc_cstr());
          }
          modified = true;
        }
        else
        {
          addRemovePort = true;
        }
      }
      else if(infos[i].portType != exec.getExecPortType(i))
      {
        setPortType = true;
      }
      else if(infos[i].dataType != exec.getExecPortTypeSpec(i))
      {
        setDataType = true;
      }

      if(addRemovePort)
      {
        QString portName = QString::fromUtf8( exec.getExecPortName(i) );

        m_controller->cmdRemovePort( portName );
        m_controller->cmdAddPort(
          portName,
          infos[i].portType,
          QString::fromUtf8( infos[i].dataType.c_str() ),
          QString(), // portToConnectWith
          QString(), // extDep
          QString() // metaData
          ); 
        modified = true;
      }
      else if(setPortType)
      {
        try
        {
          char const * name = exec.getExecPortName(i);
          exec.setExecPortType(name, infos[i].portType);
        }
        catch(FabricCore::Exception e)
        {
          m_controller->logError(e.getDesc_cstr());
          modified = true;
        }
      }
      else if(setDataType)
      {
        try
        {
          exec.setExecPortTypeSpec(i, infos[i].dataType.c_str());
        }
        catch(FabricCore::Exception e)
        {
          m_controller->logError(e.getDesc_cstr());
          modified = true;
        }
      }
    }
  }
  else if(exec.getExecPortCount() > infos.size())
  {
    int indexToRemove = exec.getExecPortCount() - 1;
    for(size_t i=0;i<infos.size();i++)
    {
      if(exec.getExecPortName(i) != infos[i].portName)
      {
        indexToRemove = i;
        break;
      }
    }

    char const * name = exec.getExecPortName(indexToRemove);
    m_controller->cmdRemovePort( name );
    modified = true;
  }
  else if(exec.getExecPortCount() < infos.size())
  {
    int indexToAdd = infos.size() - 1;
    for(size_t i=0;i<exec.getExecPortCount();i++)
    {
      if(exec.getExecPortName(i) != infos[i].portName)
      {
        indexToAdd = i;
        break;
      }
    }

    m_controller->cmdAddPort(
      QString::fromUtf8( infos[indexToAdd].portName.c_str() ),
      infos[indexToAdd].portType,
      QString::fromUtf8( infos[indexToAdd].dataType.c_str() ),
      QString(), // portToConnectWith
      QString(), // extDep
      QString() // metaData
      );
    
    modified = true;
  }

  if ( modified )
    m_ports->refresh();
}

void DFGKLEditorWidget::save()
{
  m_controller->cmdSetCode(
    m_klEditor->sourceCodeWidget()->code()
    );

  m_unsavedChanges = false;

  updateDiags( true );
}

void DFGKLEditorWidget::updateDiags( bool saving )
{
  bool haveErrors = false;
  if ( FabricCore::DFGExec exec = m_controller->getExec() )
  {
    DFGWidget *dfgWidget = m_controller->getDFGWidget();
    DFGErrorsWidget *dfgErrorsWidget = dfgWidget->getErrorsWidget();
    dfgErrorsWidget->onErrorsMayHaveChanged();
    haveErrors = dfgErrorsWidget->haveErrors();
  }

  if ( saving && !haveErrors )
    m_controller->log("Save successful.");
}

void DFGKLEditorWidget::reload()
{
  if(m_unsavedChanges)
  {
    QMessageBox msg(QMessageBox::Warning, "Fabric Warning", 
      "You have unsaved changes in the source code window, are you sure?");

    msg.addButton("Ok", QMessageBox::AcceptRole);
    msg.addButton("Cancel", QMessageBox::RejectRole);
    msg.exec();

    QString clicked = msg.clickedButton()->text();
    if(clicked == "Cancel")
      return;
  }

  QString code = m_controller->reloadCode();
  if(code.length() > 0)
  {
    try
    {
      FabricCore::DFGExec &exec = m_controller->getExec();
      m_klEditor->sourceCodeWidget()->setCodeAndExec( code, &exec );
    }
    catch(FabricCore::Exception e)
    {
      m_controller->logError(e.getDesc_cstr());
    }

    m_unsavedChanges = false;

    updateDiags();
  }
}

void DFGKLEditorWidget::onNewUnsavedChanges()
{
  m_unsavedChanges = true;
}

void DFGKLEditorWidget::onExecSplitChanged()
{
  if(!isVisible())
    return;

  FabricCore::DFGExec exec = m_controller->getExec();
  if(exec.getType() == FabricCore::DFGExecType_Func)
    onExecChanged();
}

void DFGKLEditorWidget::closeEvent(QCloseEvent * event)
{

}
