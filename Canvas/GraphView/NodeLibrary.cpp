// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FabricUI/Canvas/GraphView/NodeLibrary.h>

#include <QtGui/QVBoxLayout>
#include <QtCore/QRegExp>

#include <map>

using namespace FabricUI::GraphView;

NodeLibrary::NodeLibrary(QWidget * parent)
: QWidget(parent)
{
  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(0, 0, 0, 0);

  m_searchBox = new QLineEdit(this);
  m_searchBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  m_searchBox->setContentsMargins(0, 0, 0, 0);
  layout->addWidget(m_searchBox);
  QObject::connect(m_searchBox, SIGNAL(editingFinished()), this, SLOT(search()));

  m_scrollArea = new QScrollArea(this);
  m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_scrollArea->setContentsMargins(0, 0, 0, 0);
  m_scrollArea->setWidgetResizable(true);
  m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  layout->addWidget(m_scrollArea);

  m_listWidget = new QWidget(m_scrollArea);
  m_scrollArea->setWidget(m_listWidget);

  QVBoxLayout * listLayout = new QVBoxLayout();
  listLayout->setContentsMargins(2, 2, 2, 2);
  listLayout->setSpacing(2);
  m_listWidget->setLayout(listLayout);

  m_prevSearchPattern = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
}

void NodeLibrary::init()
{
  search();
}

QWidget * NodeLibrary::itemParent()
{
  return m_listWidget;
}

NodeLibraryItem * NodeLibrary::constructItem(unsigned int index)
{
  return new NodeLibraryItem(itemParent(), itemPath(index), itemPath(index), itemColor(index));
}

QString NodeLibrary::searchPattern() const
{
  return m_searchBox->text();
}

void NodeLibrary::search()
{
  QString pattern = searchPattern();
  if(pattern == m_prevSearchPattern)
    return;
  m_prevSearchPattern = pattern;

  // clear all previous items
  QVBoxLayout * vbox = (QVBoxLayout *)m_listWidget->layout();
  QLayoutItem* item;
  while ( ( item = vbox->takeAt( 0 ) ) != NULL )
  {
    delete(item->widget());
    delete(item);
  }
  vbox->update();

  QRegExp expression("*"+pattern.replace(".", "*")+"*", Qt::CaseInsensitive, QRegExp::Wildcard);

  std::map<QString, NodeLibraryItem*> matches;

  for(unsigned int i=0;i<itemCount();i++)
  {
    QString path = itemPath(i);
    if(!expression.exactMatch(path))
      continue;
    if(matches.find(path) != matches.end())
      continue;
    NodeLibraryItem * item = constructItem(i);
    matches.insert(std::pair<QString, NodeLibraryItem*>(path, item));
  }

  for(std::map<QString, NodeLibraryItem*>::iterator it = matches.begin(); it != matches.end(); it++)
    vbox->addWidget(it->second);

  vbox->addStretch(1);
  m_scrollArea->updateGeometry();
}

void NodeLibrary::clearSearch()
{
  m_searchBox->setText("");
  search();
}

