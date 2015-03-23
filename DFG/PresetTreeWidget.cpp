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
  m_presetDictsUpToDate = false;

  m_treeView->setDragEnabled(true);

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

  std::string search = m_searchEdit->text().toUtf8().constData();
  if(search.length() == 0)
  {
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
  else
  {
    // create a splitsearch with the content, in fact, keep it around
    // then get all matching namespaces, and allow to provide filters to the 
    // namespace items... that way they can start unfolding in a subsection
    // we should somehow also keep around the expansion information.  }
    updatePresetPathDB();
  }
}

void PresetTreeWidget::searchChanged(const QString & text)
{ 
  refresh();
}

void PresetTreeWidget::updatePresetPathDB()
{
  if(m_presetDictsUpToDate)
    return;

  std::string search = m_searchEdit->text().toUtf8().constData();
  if(search.length() == 0)
    return;

  m_presetDictsUpToDate = true;

  m_presetNameSpaceDict.clear();
  m_presetPathDict.clear();
  m_presetNameSpaceDictSTL.clear();
  m_presetPathDictSTL.clear();

  std::vector<NameSpace> namespaces;
  namespaces.push_back(m_host->getRootNameSpace());

  for(size_t i=0;i<namespaces.size();i++)
  {
    m_presetNameSpaceDictSTL.push_back(namespaces[i].getPath());

    std::vector<NameSpace> childNameSpaces = namespaces[i].getNameSpaces();
    namespaces.insert(namespaces.end(), childNameSpaces.begin(), childNameSpaces.end());

    std::vector<Object> presets = namespaces[i].getPresets();
    for(size_t j=0;j<presets.size();j++)
      m_presetPathDictSTL.push_back(presets[j].getPath());
  }

  for(size_t i=0;i<m_presetNameSpaceDictSTL.size();i++)
    m_presetNameSpaceDict.add(m_presetNameSpaceDictSTL[i].c_str(), '.', m_presetNameSpaceDictSTL[i].c_str());
  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
}
