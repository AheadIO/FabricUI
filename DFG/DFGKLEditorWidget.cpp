// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QFontMetrics>
#include <QtGui/QSplitter>
#include <QtGui/QMessageBox>

#include <FabricUI/GraphView/Graph.h>
#include <FabricUI/GraphView/SidePanel.h>

#include "DFGKLEditorWidget.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGKLEditorWidget::DFGKLEditorWidget(QWidget * parent, DFGController * controller, ASTWrapper::KLASTManager * manager, const DFGConfig & config)
: QWidget(parent)
{
  m_controller = controller;
  m_config = config;
  m_unsavedChanges = false;

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

  QPushButton * compileButton = new QPushButton("Compile", buttonsWidget);
  QPushButton * reloadButton = new QPushButton("Reload", buttonsWidget);
  buttonsLayout->addStretch(2);
  buttonsLayout->addWidget(reloadButton);
  buttonsLayout->addWidget(compileButton);


  QPalette buttonPal(compileButton->palette());
  buttonPal.setColor(QPalette::ButtonText, config.graphConfig.headerFontColor);
  buttonPal.setColor(QPalette::Button, config.graphConfig.nodeDefaultColor);
  // compileButton->setAutoFillBackground(false);
  compileButton->setPalette(buttonPal);
  reloadButton->setPalette(buttonPal);

  QSplitter * splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);
  splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setChildrenCollapsible(false);

  m_ports = new DFGKLEditorPortTableWidget(splitter, controller, config);
  m_klEditor = new KLEditor::KLEditorWidget(splitter, manager, config.klEditorConfig);

  splitter->addWidget(m_ports);
  splitter->addWidget(m_klEditor);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 7);
  layout->addWidget(buttonsWidget);
  layout->addWidget(splitter);

  QObject::connect(compileButton, SIGNAL(clicked()), this, SLOT(compile()));
  QObject::connect(reloadButton, SIGNAL(clicked()), this, SLOT(reload()));
  QObject::connect(m_ports, SIGNAL(execPortsChanged()), this, SLOT(onExecPortsChanged()));
  QObject::connect(m_klEditor->sourceCodeWidget(), SIGNAL(newUnsavedChanged()), this, SLOT(onNewUnsavedChanges()));
}

DFGKLEditorWidget::~DFGKLEditorWidget()
{
}

void DFGKLEditorWidget::setFunc(FabricCore::DFGExec func, char const * execPath)
{
  m_func = func;
  m_execPath = execPath;

  try
  {
    m_klEditor->sourceCodeWidget()->setCode(m_func.getCode());
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }

  m_ports->setExec(func);
  m_unsavedChanges = false;
}

void DFGKLEditorWidget::onExecPortsChanged()
{
  std::vector<DFGKLEditorPortTableWidget::PortInfo> infos;
  for(unsigned int i=0;i<m_ports->nbPorts();i++)
    infos.push_back(m_ports->portInfo(i));

  bool modified = false;

  // we expect only one change, so either a new port,
  // portName / portType or dataType change
  if(m_func.getExecPortCount() == infos.size())
  {
    for(size_t i=0;i<infos.size();i++)
    {
      bool addRemovePort = false;
      bool setPortType = false;
      bool setDataType = false;
      if(infos[i].portName != m_func.getExecPortName(i))
      {
        if(infos[i].portType == m_func.getExecPortType(i) &&
          infos[i].dataType == m_func.getExecPortTypeSpec(i))
        {
          try
          {
            std::string path = m_execPath;
            path += ".";
            path += m_func.getExecPortName(i);
            m_controller->renamePort(path.c_str(), infos[i].portName.c_str());
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
      else if(infos[i].portType != m_func.getExecPortType(i))
      {
        setPortType = true;
      }
      else if(infos[i].dataType != m_func.getExecPortTypeSpec(i))
      {
        setDataType = true;
      }

      if(addRemovePort)
      {
        char const * name = m_func.getExecPortName(i);

        if(m_controller->removePort(name))
        {
          m_controller->addPort(
            m_controller->getExecPath(),
            name,
            infos[i].portType,
            infos[i].dataType.c_str(),
            false
            ); 
          modified = true;
        }
      }
      else if(setPortType)
      {
        try
        {
          char const * name = m_func.getExecPortName(i);
          m_func.setExecPortType(name, infos[i].portType);
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
          m_func.setExecPortTypeSpec(i, infos[i].dataType.c_str());
        }
        catch(FabricCore::Exception e)
        {
          m_controller->logError(e.getDesc_cstr());
          modified = true;
        }
      }
    }
  }
  else if(m_func.getExecPortCount() > infos.size())
  {
    int indexToRemove = m_func.getExecPortCount() - 1;
    for(size_t i=0;i<infos.size();i++)
    {
      if(m_func.getExecPortName(i) != infos[i].portName)
      {
        indexToRemove = i;
        break;
      }
    }

    char const * name = m_func.getExecPortName(indexToRemove);
    m_controller->removePort(name);
    modified = true;
  }
  else if(m_func.getExecPortCount() < infos.size())
  {
    int indexToAdd = infos.size() - 1;
    for(size_t i=0;i<m_func.getExecPortCount();i++)
    {
      if(m_func.getExecPortName(i) != infos[i].portName)
      {
        indexToAdd = i;
        break;
      }
    }

    m_controller->addPort(
      m_controller->getExecPath(),
      infos[indexToAdd].portName,
      infos[indexToAdd].portType,
      infos[indexToAdd].dataType,
      false
      );
    
    modified = true;
  }

  if(modified)
  {
    m_ports->setExec(m_func);
  }
}

void DFGKLEditorWidget::compile()
{
  if(m_controller->setCode(m_execPath.c_str(), m_klEditor->sourceCodeWidget()->code().toUtf8().constData()))
    m_unsavedChanges = false;
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

  std::string code = m_controller->reloadCode(m_execPath.c_str());
  if(code.length() > 0)
  {
    try
    {
      m_klEditor->sourceCodeWidget()->setCode(code.c_str());
    }
    catch(FabricCore::Exception e)
    {
      m_controller->logError(e.getDesc_cstr());
    }
    m_unsavedChanges = false;
  }
}

void DFGKLEditorWidget::onNewUnsavedChanges()
{
  m_unsavedChanges = true;
}

void DFGKLEditorWidget::closeEvent(QCloseEvent * event)
{

}
