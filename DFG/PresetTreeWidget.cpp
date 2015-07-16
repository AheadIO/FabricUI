// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/NameSpaceTreeItem.h>
#include <FabricUI/DFG/PresetTreeItem.h>
#include <FabricUI/DFG/PresetTreeWidget.h>
#include <FabricUI/DFG/VariableListTreeItem.h>

#include <FTL/JSONValue.h>
#include <FTL/MapCharSingle.h>
#include <FTL/Str.h>
#include <FTL/FS.h>

#include <QtGui/QVBoxLayout>
#include <QtGui/QDesktopServices>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtCore/QUrl>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;

PresetTreeWidget::PresetTreeWidget(
  DFGController *dfgController,
  const DFGConfig & config,
  bool showsPresets,
  bool showSearch,
  bool setupContextMenu
  )
  : m_dfgController( dfgController )
  , m_showsPresets( showsPresets )
{
  if(showSearch)
    m_searchEdit = new QLineEdit(this);
  else
    m_searchEdit = NULL;
  m_treeView = new TreeView::TreeViewWidget(this);
  m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
  m_treeModel = new TreeView::TreeModel(this);
  m_treeView->setModel(m_treeModel);
  m_presetDictsUpToDate = false;

  m_treeView->setDragEnabled(true);

  setFont(config.defaultFont);
  m_treeView->setFont(config.defaultFont);

  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  if(m_searchEdit)
    layout->addWidget(m_searchEdit);
  layout->addWidget(m_treeView);

  refresh();

  if(m_searchEdit)
  {
    QObject::connect(m_searchEdit, SIGNAL(editingFinished()), this, SLOT(refresh()));
    QObject::connect(m_searchEdit, SIGNAL(textChanged(const QString&)), this, SLOT(refresh()));
  }

  QObject::connect(
    dfgController, SIGNAL(hostChanged()),
    this, SLOT(refresh())
    );
  QObject::connect(
    dfgController, SIGNAL(bindingChanged()),
    this, SLOT(refresh())
    );

  if(setupContextMenu)
  {
    QObject::connect(m_treeView, SIGNAL(customContextMenuRequested(QPoint, FabricUI::TreeView::TreeItem *)), 
      this, SLOT(onCustomContextMenuRequested(QPoint, FabricUI::TreeView::TreeItem *)));
  }
}

PresetTreeWidget::~PresetTreeWidget()
{
}

void PresetTreeWidget::refresh()
{
  std::string search;
  if(m_searchEdit)
    search = m_searchEdit->text().toUtf8().constData();

  // persist the state of the treeview
  if(search.length() != 0 && m_state.length() == 0)
    m_state = m_treeView->state();

  FabricCore::DFGHost &host = m_dfgController->getCoreDFGHost();
  if ( !host )
    return;

  FabricCore::DFGBinding &binding = m_dfgController->getCoreDFGBinding();
  if ( !binding )
    return;

  m_treeModel->clear();

  if(search.length() == 0)
  {
    std::map<std::string, std::string> nameSpaceLookup;
    std::map<std::string, std::string> presetLookup;

    FabricCore::DFGStringResult jsonStr = host.getPresetDesc("");
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
    m_treeModel->addItem( new VariableListTreeItem( binding ) );

    for(std::map<std::string, std::string>::iterator it=nameSpaceLookup.begin();it!=nameSpaceLookup.end();it++)
    {
      NameSpaceTreeItem * item =
        new NameSpaceTreeItem( host, it->first.c_str(), it->second.c_str() );
      item->setShowsPresets(m_showsPresets);
      m_treeModel->addItem(item);
    }

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

    FabricCore::DFGStringResult jsonStringResult = host.getPresetDesc("");

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

      if(objectType != FTL_STR("NameSpace"))
        continue;

      QStringList filters;
      for(size_t j=0;j<userDatas.size();j++)
      {
        QString match(userDatas[j]);
        if(match.startsWith(name.data()))
        {
          filters.append(match.right(match.length() - name.size() - 1));
        }
      }

      NameSpaceTreeItem * item = new NameSpaceTreeItem(host, name.c_str(), name.c_str(), filters);
      item->setShowsPresets(m_showsPresets);
      m_treeModel->addItem(item);
    }

    m_treeView->expandAll();
  }
}

void PresetTreeWidget::onCustomContextMenuRequested(QPoint globalPos, FabricUI::TreeView::TreeItem * item)
{
  m_contextPath = item->path();

  QMenu menu(NULL);
  if(item->type() == "NameSpace" || item->type() == "Preset")
    menu.addAction("Open Folder");
  if(item->type() == "Preset")
    menu.addAction("Open File");

  QObject::connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(onContextMenuAction(QAction*)));
  menu.exec(globalPos);  
}

void PresetTreeWidget::onContextMenuAction(QAction * action)
{
  if(action->text() == "Open Folder" || action->text() == "Open File")
  {
    try
    {
      FabricCore::DFGHost host = m_dfgController->getCoreDFGHost();
      FTL::StrRef path = host.getPresetImportPathname(m_contextPath.toUtf8().constData());
      if(action->text() == "Open Folder")
      {
        if(!FTL::FSIsDir(path))
        {
          path = FTL::PathSplit(path).first;
        }
      }

      std::string pathStr(path.data(), path.size());
      QDesktopServices::openUrl(QUrl::fromLocalFile(pathStr.c_str()));
    }
    catch(FabricCore::Exception e)
    {
    }
  }
}

void PresetTreeWidget::updatePresetPathDB()
{
  if(m_presetDictsUpToDate)
    return;

  if(!m_searchEdit)
    return;

  std::string search = m_searchEdit->text().toUtf8().constData();
  if(search.length() == 0)
    return;

  FabricCore::DFGHost &host = m_dfgController->getCoreDFGHost();
  if ( !host )
    return;

  FabricCore::DFGBinding &binding = m_dfgController->getCoreDFGBinding();
  if ( !binding )
    return;

  m_presetDictsUpToDate = true;

  m_presetPathDict.clear();
  m_presetPathDictSTL.clear();

  std::vector<std::string> paths;
  paths.push_back("");

  for(unsigned int i=0;i<paths.size();i++)
  {
    FabricCore::DFGStringResult jsonStringResult =
      host.getPresetDesc( paths[i].c_str() );
    
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

      std::string path = paths[i];
      if(path.length() > 0)
        path += ".";
      path += name;

      if(objectType == FTL_STR("Preset"))
      {
        m_presetPathDictSTL.push_back(path);
      }
      else if(objectType == FTL_STR("NameSpace"))
      {
        paths.push_back(path);
      }
    }
  }

  for(size_t i=0;i<m_presetPathDictSTL.size();i++)
  {
    m_presetPathDict.add(m_presetPathDictSTL[i].c_str(), '.', m_presetPathDictSTL[i].c_str());
  }
}
