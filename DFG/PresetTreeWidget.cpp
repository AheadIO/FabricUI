// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PresetTreeWidget.h"
#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"

#include <QtGui/QVBoxLayout>

using namespace FabricServices::DFGWrapper;
using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeWidget::PresetTreeWidget(QWidget * parent, Host * host, const DFGConfig & config)
: QWidget(parent)
, m_host(host)
{
  m_searchEdit = new QLineEdit(this);
  m_treeView = new TreeView::TreeViewWidget(this);
  m_treeModel = new TreeView::TreeModel(this);
  m_treeView->setModel(m_treeModel);

  m_treeView->setDragEnabled(true);

  // QPalette p = palette();
  // p.setColor(QPalette::Base, config.defaultBackgroundColor);
  // p.setColor(QPalette::Text, config.defaultFontColor);
  // setPalette(p);  
  setFont(config.defaultFont);

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(m_searchEdit);
  layout->addWidget(m_treeView);

  refresh();

  QObject::connect(m_searchEdit, SIGNAL(textChanged(const QString &)), this, SLOT(searchChanged(const QString &)));
}

PresetTreeWidget::~PresetTreeWidget()
{
}

void PresetTreeWidget::refresh()
{
  m_treeModel->clear();

  if(!m_host)
    return;

  std::vector<NameSpace> nameSpaces = m_host->getRootNameSpace().getNameSpaces();
  for(size_t i=0;i<nameSpaces.size();i++)
  {
    m_treeModel->addItem(new NameSpaceTreeItem(nameSpaces[i]));
  }
  std::vector<Graph> graphs = m_host->getRootNameSpace().getGraphs();
  for(size_t i=0;i<graphs.size();i++)
  {
    m_treeModel->addItem(new PresetTreeItem(graphs[i]));
  }
  std::vector<Func> functions = m_host->getRootNameSpace().getFuncs();
  for(size_t i=0;i<functions.size();i++)
  {
    m_treeModel->addItem(new PresetTreeItem(functions[i]));
  }
}

void PresetTreeWidget::searchChanged(const QString & text)
{ 
  printf("%s\n", text.toUtf8().constData());
}
