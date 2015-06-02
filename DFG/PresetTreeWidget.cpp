// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PresetTreeWidget.h"
#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"
#include <FTL/Str.h>
#include <FTL/MapCharSingle.h>

#include <QtGui/QVBoxLayout>

using namespace FabricServices;
using namespace FabricServices::DFGWrapper;
using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeWidget::PresetTreeWidget(
  QWidget * parent,
  FabricCore::DFGHost const &coreDFGHost,
  const DFGConfig & config
  )
  : QWidget( parent )
  , m_coreDFGHost( coreDFGHost )
{
  m_searchEdit = new QLineEdit(this);
  m_treeView = new TreeView::TreeViewWidget(this);
  m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
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

  QObject::connect(m_searchEdit, SIGNAL(editingFinished()), this, SLOT(refresh()));
  QObject::connect(m_searchEdit, SIGNAL(textChanged(const QString&)), this, SLOT(refresh()));
}

PresetTreeWidget::~PresetTreeWidget()
{
}

void PresetTreeWidget::setHost( FabricCore::DFGHost const &coreDFGHost )
{
  m_coreDFGHost = coreDFGHost;
}

void PresetTreeWidget::refresh()
{
  std::string search = m_searchEdit->text().toUtf8().constData();

  // persist the state of the treeview
  if(search.length() != 0 && m_state.length() == 0)
    m_state = m_treeView->state();

  if(!m_host)
    return;

  m_treeModel->clear();

  if(search.length() == 0)
  {
    std::vector<NameSpace> & nameSpaces = m_host->getRootNameSpace().getNameSpaces();
    for(size_t i=0;i<nameSpaces.size();i++)
    {
      m_treeModel->addItem(new NameSpaceTreeItem(nameSpaces[i]));
    }
    std::vector<Graph> & graphs = m_host->getRootNameSpace().getGraphs();
    for(size_t i=0;i<graphs.size();i++)
    {
      m_treeModel->addItem(new PresetTreeItem(graphs[i]));
    }
    std::vector<Func> & functions = m_host->getRootNameSpace().getFuncs();
    for(size_t i=0;i<functions.size();i++)
    {
      m_treeModel->addItem(new PresetTreeItem(functions[i]));
    }

    // todo: this should really be working.
    //m_treeView->setState(m_state);
    //m_state = "";
  }
  else
  {
    // create a splitsearch with the content, in fact, keep it around
    // then get all matching namespaces, and allow to provide filters to the 
    // namespace items... that way they can start unfolding in a subsection
    // we should somehow also keep around the expansion information.  }
    updatePresetPathDB();

    // [pzion 20150305] This is a little evil but avoids lots of copying
    std::vector<FTL::StrRef> searchSplit;
    FTL::StrSplit<'.'>( search, searchSplit, true /*strict*/ );

    FTL::StrRemap< FTL::MapCharSingle<'.', '\0'> >( search );

    char const **cStrs = reinterpret_cast<char const **>(
      alloca( searchSplit.size() * sizeof( char const * ) )
      );
    for ( size_t i = 0; i < searchSplit.size(); ++i )
      cStrs[i] = searchSplit[i].data();    

    SplitSearch::Matches matches = m_presetPathDict.search( searchSplit.size(), cStrs );

    if(matches.getSize() == 0)
      return;

    std::vector<const char *> userDatas;
    userDatas.resize(matches.getSize());
    matches.getUserdatas(matches.getSize(), (const void**)&userDatas[0]);

    std::vector<NameSpace> & nameSpaces = m_host->getRootNameSpace().getNameSpaces();
    for(size_t i=0;i<nameSpaces.size();i++)
    {
      QString name = QString(nameSpaces[i].getName().c_str()) + ".";
      QStringList filters;
      for(size_t j=0;j<userDatas.size();j++)
      {
        QString match(userDatas[j]);
        if(match.startsWith(name))
          filters.append(match.right(match.length() - name.length()));
      }

      if(filters.length() > 0)
        m_treeModel->addItem(new NameSpaceTreeItem(nameSpaces[i], filters));
    }

    m_treeView->expandAll();
  }
}

void PresetTreeWidget::updatePresetPathDB()
{
  if(m_presetDictsUpToDate)
    return;

  std::string search = m_searchEdit->text().toUtf8().constData();
  if(search.length() == 0)
    return;

  m_presetDictsUpToDate = true;

  m_presetPathDict.clear();
  m_presetPathDictSTL.clear();

  std::vector<Object> & presets = m_host->getRootNameSpace().getPresets(true /*recursive*/);
  m_presetPathDictSTL.resize(presets.size());
  for(size_t i=0;i<presets.size();i++)
  {
    m_presetPathDictSTL[i] = presets[i].getPath();
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
  }
}
