// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QFontMetrics>
#include <QtGui/QSplitter>

#include <GraphView/Graph.h>
#include <GraphView/SidePanel.h>

#include "DFGKLEditorWidget.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGKLEditorWidget::DFGKLEditorWidget(QWidget * parent, DFGController * controller, ASTWrapper::KLASTManager * manager, const DFGConfig & config)
: QWidget(parent)
{
  m_controller = controller;
  m_config = config;
  m_func = NULL;
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
  buttonsLayout->addStretch(2);
  buttonsLayout->addWidget(compileButton);

  QPalette buttonPal(compileButton->palette());
  buttonPal.setColor(QPalette::ButtonText, config.graphConfig.headerFontColor);
  buttonPal.setColor(QPalette::Button, config.graphConfig.nodeDefaultColor);
  // compileButton->setAutoFillBackground(false);
  compileButton->setPalette(buttonPal);

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
  QObject::connect(m_ports, SIGNAL(portsChanged()), this, SLOT(onPortsChanged()));
  QObject::connect(m_klEditor->sourceCodeWidget(), SIGNAL(newUnsavedChanged()), this, SLOT(onNewUnsavedChanges()));
}

DFGKLEditorWidget::~DFGKLEditorWidget()
{
}

void DFGKLEditorWidget::setFunc(DFGWrapper::FuncExecutablePtr func)
{
  m_func = func;

  try
  {
    m_klEditor->sourceCodeWidget()->setCode(m_func->getCode());
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }

  m_ports->setExec(func);
  m_unsavedChanges = false;
}

void DFGKLEditorWidget::onPortsChanged()
{
  std::vector<DFGKLEditorPortTableWidget::PortInfo> infos;
  for(unsigned int i=0;i<m_ports->nbPorts();i++)
    infos.push_back(m_ports->portInfo(i));

  DFGWrapper::PortList ports = m_func->getPorts();

  bool modified = false;

  // we expect only one change, so either a new port,
  // portName / portType or dataType change
  if(ports.size() == infos.size())
  {
    for(size_t i=0;i<infos.size();i++)
    {
      bool addRemovePort = false;
      bool setPortType = false;
      bool setDataType = false;
      if(infos[i].portName != ports[i]->getName())
      {
        if(infos[i].portType == ports[i]->getPortType() &&
          infos[i].dataType == ports[i]->getTypeSpec())
        {
          try
          {
            m_controller->renamePort(ports[i]->getEndPointPath(), infos[i].portName.c_str());
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
      else if(infos[i].portType != ports[i]->getPortType())
      {
        setPortType = true;
      }
      else if(infos[i].dataType != ports[i]->getTypeSpec())
      {
        setDataType = true;
      }

      if(addRemovePort)
      {
        QString path = m_func->getFuncPath();
        QString name = ports[i]->getName();

        if(m_controller->removePort(path, name))
        {
          m_controller->addPort(path, name, infos[i].portType, infos[i].dataType.c_str(), false); 
          modified = true;
        }
      }
      else if(setPortType)
      {
        try
        {
          ports[i]->setPortType(infos[i].portType);
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
          ports[i]->setTypeSpec(infos[i].dataType.c_str());
        }
        catch(FabricCore::Exception e)
        {
          m_controller->logError(e.getDesc_cstr());
          modified = true;
        }
      }
    }
  }
  else if(ports.size() > infos.size())
  {
    int indexToRemove = ports.size() - 1;
    for(size_t i=0;i<infos.size();i++)
    {
      if(ports[i]->getName() != infos[i].portName)
      {
        indexToRemove = i;
        break;
      }
    }

    QString path = m_func->getFuncPath();
    QString name = ports[indexToRemove]->getName();
    m_controller->removePort(path, name);
    modified = true;
  }
  else if(ports.size() < infos.size())
  {
    int indexToAdd = infos.size() - 1;
    for(size_t i=0;i<ports.size();i++)
    {
      if(ports[i]->getName() != infos[i].portName)
      {
        indexToAdd = i;
        break;
      }
    }

    QString path = m_func->getFuncPath();
    QString name = infos[indexToAdd].portName.c_str();
    QString dataType = infos[indexToAdd].dataType.c_str();

    m_controller->addPort(path, name, infos[indexToAdd].portType, dataType, false);
    modified = true;
  }

  if(modified)
  {
    m_ports->setExec(m_func);
  }
}

void DFGKLEditorWidget::compile()
{
  if(m_controller->setCode(m_func->getFuncPath(), m_klEditor->sourceCodeWidget()->code()))
    m_unsavedChanges = false;
}

void DFGKLEditorWidget::onNewUnsavedChanges()
{
  m_unsavedChanges = true;
}

void DFGKLEditorWidget::closeEvent(QCloseEvent * event)
{

}
