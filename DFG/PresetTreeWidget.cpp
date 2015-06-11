// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "PresetTreeWidget.h"
#include "NameSpaceTreeItem.h"
#include "PresetTreeItem.h"
#include "VariableListTreeItem.h"

#include <FTL/JSONValue.h>
#include <FTL/MapCharSingle.h>
#include <FTL/Str.h>

#include <QtGui/QVBoxLayout>

using namespace FabricServices;
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
  refresh();
}

void PresetTreeWidget::setBinding( FabricCore::DFGBinding const &coreDFGBinding )
{
  m_coreDFGBinding = coreDFGBinding;
  refresh();
}

void PresetTreeWidget::refresh()
{
  std::string search = m_searchEdit->text().toUtf8().constData();

  // persist the state of the treeview
  if(search.length() != 0 && m_state.length() == 0)
    m_state = m_treeView->state();

  if(!m_coreDFGHost.isValid())
    return;

  m_treeModel->clear();

  if(search.length() == 0)
  {
    std::map<std::string, std::string> nameSpaceLookup;
    std::map<std::string, std::string> presetLookup;

    FabricCore::DFGStringResult jsonStr = m_coreDFGHost.getPresetDesc("");
    FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(jsonStr.getCString());
    const FabricCore::Variant * membersVar = jsonVar.getDictValue("members");

    for(FabricCore::Variant::DictIter memberIter(*membersVar); !memberIter.isDone(); memberIter.next())
    {
      std::string name = memberIter.getKey()->getStringData();
      const FabricCore::Variant * memberVar = memberIter.getValue();
      const FabricCore::Variant * objectTypeVar = memberVar->getDictValue("objectType");
      std::string objectType = objectTypeVar->getStringData();
      if(objectType == "Preset")
      {
        presetLookup.insert(std::pair<std::string, std::string>(name, name));
      }
      else if(objectType == "NameSpace")
      {
        nameSpaceLookup.insert(std::pair<std::string, std::string>(name, name));
      }
    }

    // also add the variable list item
    m_treeModel->addItem(new VariableListTreeItem(m_coreDFGHost, m_coreDFGBinding));

    for(std::map<std::string, std::string>::iterator it=nameSpaceLookup.begin();it!=nameSpaceLookup.end();it++)
      m_treeModel->addItem(new NameSpaceTreeItem(m_coreDFGHost, it->first.c_str(), it->second.c_str()));

    for(std::map<std::string, std::string>::iterator it=presetLookup.begin();it!=presetLookup.end();it++)
      m_treeModel->addItem(new PresetTreeItem(it->second.c_str(), it->first.c_str()));
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

    FabricCore::DFGStringResult jsonStr = m_coreDFGHost.getPresetDesc("");
    FabricCore::Variant jsonVar = FabricCore::Variant::CreateFromJSON(jsonStr.getCString());
    const FabricCore::Variant * membersVar = jsonVar.getDictValue("members");

    for(FabricCore::Variant::DictIter memberIter(*membersVar); !memberIter.isDone(); memberIter.next())
    {
      FTL::StrRef name = memberIter.getKey()->getStringData();

      QStringList filters;
      for(size_t j=0;j<userDatas.size();j++)
      {
        QString match(userDatas[j]);
        if(match.startsWith(name.data()))
          filters.append(match.right(match.length() - name.size()));
      }

      if(filters.length() == 0)
        continue;

      // also add the variable list item
      m_treeModel->addItem(new VariableListTreeItem(m_coreDFGHost, m_coreDFGBinding, filters));

      const FabricCore::Variant * memberVar = memberIter.getValue();
      const FabricCore::Variant * objectTypeVar = memberVar->getDictValue("objectType");
      std::string objectType = objectTypeVar->getStringData();
      if(objectType == "NameSpace")
      {
        m_treeModel->addItem(new NameSpaceTreeItem(m_coreDFGHost, name.data(), name.data(), filters));
      }
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

  std::vector<std::string> paths;
  paths.push_back("");

  unsigned oldSize = paths.size() + 1;
  for(unsigned int i=0;i<oldSize;i++)
  {
    FabricCore::DFGStringResult jsonStringResult = m_coreDFGHost.getPresetDesc("");
    char const *jsonCStr;
    uint32_t jsonSize;
    jsonStringResult.getStringDataAndLength( jsonCStr, jsonSize );
    FTL::CStrRef jsonStr( jsonCStr, jsonSize );
    FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );

    FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
      FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
      );
    FTL::JSONObject const *membersObject =
      jsonObject->get( FTL_STR("members") )->cast<FTL::JSONObject>();

    for ( FTL::JSONObject::const_iterator it = membersObject->begin();
      it != membersObject->end(); ++it )
    {
      FTL::CStrRef name = it->first;
      FTL::JSONObject const *memberObject =
        it->second->cast<FTL::JSONObject>();
      FTL::CStrRef objectType =
        memberObject->getString( FTL_STR("objectType") );
      if(objectType == FTL_STR("Preset"))
      {
        m_presetPathDictSTL.push_back(paths[i] + std::string(".") + name.c_str());
      }
      else if(objectType == FTL_STR("NameSpace"))
      {
        paths.push_back(paths[i] + std::string(".") + name.c_str());
      }
    }
  }

  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
  {
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
  }
}
