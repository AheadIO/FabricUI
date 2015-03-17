// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QFontMetrics>
#include <QtGui/QSplitter>

#include <GraphView/Graph.h>
#include <GraphView/SidePanel.h>

#include "DFGKLEditorWidget.h"
#include "Dialogs/DFGNewPortDialog.h"

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

DFGKLEditorWidget::DFGKLEditorWidget(QWidget * parent, DFGController * controller, ASTWrapper::KLASTManager * manager, const DFGConfig & config)
: QWidget(parent)
{
  m_controller = controller;
  m_config = config;
  m_func = NULL;

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  setMinimumSize(QSize(300, 600));

  QPalette p = palette();
  // p.setColor(QPalette::Window, config.defaultBackgroundColor);
  // p.setColor(QPalette::Base, config.defaultFontColor);
  // p.setColor(QPalette::Text, config.defaultBackgroundColor);
  // p.setColor(QPalette::Button, config.defaultFontColor);
  // p.setColor(QPalette::ButtonText, config.defaultBackgroundColor);
  // setPalette(p);  
  setFont(config.fixedFont);

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  QWidget * buttonsWidget = new QWidget(this);
  buttonsWidget->setContentsMargins(0, 0, 0, 0);
  buttonsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
  QHBoxLayout * buttonsLayout = new QHBoxLayout();
  buttonsWidget->setContentsMargins(0, 0, 0, 0);
  buttonsLayout->setContentsMargins(0, 0, 0, 0);
  buttonsWidget->setLayout(buttonsLayout);

  QPushButton * compileButton = new QPushButton("Compile", buttonsWidget);
  // QPushButton * addPortButton = new QPushButton("New Port", buttonsWidget);
  // QPushButton * removePortButton = new QPushButton("Remove Port", buttonsWidget);
  buttonsLayout->addWidget(compileButton);
  // buttonsLayout->addWidget(addPortButton);
  // buttonsLayout->addWidget(removePortButton);
  buttonsLayout->addStretch(2);

  KLEditor::EditorConfig editorConfig;

  editorConfig.codeBackgroundColor = config.defaultFontColor;
  editorConfig.codeFontColor = config.defaultBackgroundColor;

  QSplitter * splitter = new QSplitter(this);
  splitter->setOrientation(Qt::Vertical);
  splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  splitter->setContentsMargins(0, 0, 0, 0);
  splitter->setChildrenCollapsible(false);

  m_ports = new DFGKLEditorPortTableWidget(splitter, controller, config);
  m_klEditor = new KLEditor::KLEditorWidget(splitter, manager, editorConfig);

  splitter->addWidget(m_ports);
  splitter->addWidget(m_klEditor);
  splitter->setStretchFactor(0, 1);
  splitter->setStretchFactor(1, 7);
  layout->addWidget(splitter);
  layout->addWidget(buttonsWidget);

  QObject::connect(compileButton, SIGNAL(clicked()), this, SLOT(compile()));
  QObject::connect(m_ports, SIGNAL(portsChanged()), this, SLOT(onPortsChanged()));
  // QObject::connect(addPortButton, SIGNAL(clicked()), this, SLOT(addPort()));
  // QObject::connect(removePortButton, SIGNAL(clicked()), this, SLOT(removePort()));
}

DFGKLEditorWidget::~DFGKLEditorWidget()
{
  if(m_func)
    delete(m_func);
}

void DFGKLEditorWidget::setFunc(DFGWrapper::FuncExecutable func)
{
  if(m_func)
    delete(m_func);
  m_func = new DFGWrapper::FuncExecutable(func);

  try
  {
    m_klEditor->sourceCodeWidget()->setCode(m_func->getCode().c_str());
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }

  m_ports->setExec(func);
}

void DFGKLEditorWidget::onPortsChanged()
{
  std::vector<DFGKLEditorPortTableWidget::PortInfo> infos;
  for(unsigned int i=0;i<m_ports->nbPorts();i++)
    infos.push_back(m_ports->portInfo(i));

  std::vector<DFGWrapper::Port> ports = m_func->getPorts();

  bool modified = false;

  // we expect only one change, so either a new port,
  // portName / portType or dataType change
  if(ports.size() == infos.size())
  {
    for(size_t i=0;i<infos.size();i++)
    {
      bool addRemovePort = false;
      if(infos[i].portName != ports[i].getName())
      {
        if(infos[i].portType == ports[i].getPortType() &&
          infos[i].dataType == ports[i].getDataType())
        {
          m_controller->renamePort(ports[i].getPath().c_str(), infos[i].portName.c_str());
          modified = true;
        }
        else
        {
          addRemovePort = true;
        }
      }
      else if(infos[i].portType != ports[i].getPortType())
      {
        addRemovePort = true;
      }
      else if(infos[i].dataType != ports[i].getDataType())
      {
        addRemovePort = true;
      }

      if(addRemovePort)
      {
        QString path = m_func->getPath().c_str();
        QString name = ports[i].getName().c_str();

        if(m_controller->removePort(path, name))
        {
          m_controller->addPort(path, name, infos[i].portType, infos[i].dataType.c_str()); 
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
      if(ports[i].getName() != infos[i].portName)
      {
        indexToRemove = i;
        break;
      }
    }

    QString path = m_func->getPath().c_str();
    QString name = ports[indexToRemove].getName().c_str();
    m_controller->removePort(path, name);
    modified = true;
  }
  else if(ports.size() < infos.size())
  {
    int indexToAdd = infos.size() - 1;
    for(size_t i=0;i<ports.size();i++)
    {
      if(ports[i].getName() != infos[i].portName)
      {
        indexToAdd = i;
        break;
      }
    }

    QString path = m_func->getPath().c_str();
    QString name = infos[indexToAdd].portName.c_str();
    QString dataType = infos[indexToAdd].dataType.c_str();

    m_controller->addPort(path, name, infos[indexToAdd].portType, dataType);
    modified = true;
  }

  if(modified)
  {
    m_ports->setExec(*m_func);
  }
}

void DFGKLEditorWidget::compile()
{
  m_controller->setCode(m_func->getPath().c_str(), m_klEditor->sourceCodeWidget()->code());
}


// void DFGKLEditorWidget::addPort()
// {
//   DFGNewPortDialog dialog(this, true, m_config);
//   if(dialog.exec() != QDialog::Accepted)
//     return;

//   QString portTypeStr = dialog.portType();
//   QString title = dialog.title();
//   QString dataType = dialog.dataType();
//   QString extension = dialog.extension();

//   if(title.length() == 0)
//     return;

//   GraphView::PortType portType = GraphView::PortType_Input;
//   if(portTypeStr == "In")
//     portType = GraphView::PortType_Output;
//   else if(portTypeStr == "IO")
//     portType = GraphView::PortType_IO;

//   if(title.length() > 0)
//   {
//     if(extension.length() > 0)
//       m_controller->addExtensionDependency(extension, m_func->getPath().c_str());
//     if(m_controller->addPort(m_func->getPath().c_str(), title, portType, dataType).length() > 0)
//       m_ports->setExec(*m_func);
//   }
// }

// void DFGKLEditorWidget::removePort()
// {
//   QString portName = m_ports->selectedItem();
//   if(portName.length() == 0)
//     return;

//   try
//   {
//     if(m_controller->removePort(m_func->getPath().c_str(), portName))
//       m_ports->setExec(*m_func);
//   }
//   catch(FabricCore::Exception e)
//   {
//     m_controller->logError(e.getDesc_cstr());
//   }
// }

void DFGKLEditorWidget::closeEvent(QCloseEvent * event)
{

}
