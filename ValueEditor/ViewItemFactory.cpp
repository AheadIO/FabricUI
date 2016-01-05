#include "stdafx.h"
#include "ViewItemFactory.h"
#include "BaseViewItemCreator.h"
#include "BaseModelItem.h"
#include "BaseViewItem.h"

ViewItemFactory::ViewItemFactory()
{

}

ViewItemFactory::~ViewItemFactory()
{
  // destruct all remaining creators.
  while (!m_creators.empty())
    (*m_creators.rbegin())->DeleteThis();
}

ViewItemFactory* ViewItemFactory::GetInstance()
{
  static ViewItemFactory instance;
  return &instance;
}


BaseViewItem *ViewItemFactory::BuildView( BaseModelItem *modelItem )
{
  // We put the QVariant-RTVal bridge injection
  // code here, as before we build a view it won't
  // be needed
  static bool doVarInjection = true;
  if (doVarInjection)
  {
    RTVariant::injectRTHandler();
    doVarInjection = false;
  }

  if ( !modelItem )
    return 0;

  BaseViewItem* viewItem = CreateViewItem( modelItem );
  if ( viewItem )
  {
    QObject::connect(
      viewItem, SIGNAL( viewValueChanged( QVariant const &, bool ) ),
      modelItem, SLOT( onViewValueChanged( QVariant const &, bool ) )
      );

    QObject::connect(
      modelItem, SIGNAL( modelValueChanged( QVariant const & ) ),
      viewItem, SLOT( onModelValueChanged( QVariant const & ) )
      );
  }
  return viewItem;
}

// sort using a custom function object
struct CreatorSort
{
  bool operator()( BaseViewItemCreator* a, BaseViewItemCreator* b )
  {
    return a->Priority() < b->Priority();
  }
};

bool ViewItemFactory::RegisterCreator(BaseViewItemCreator* creator)
{
  for ( CreatorIT itr = creatorBegin(); itr != creatorEnd(); itr++ )
  {
    if (*itr == creator)
      return false;
  }
  m_creators.push_back(creator);
  // Sort creators in order of priority
  std::sort( m_creators.begin(), m_creators.end(), CreatorSort() );
  return true;
}

void ViewItemFactory::DeRegisterCreator(BaseViewItemCreator* creator)
{
  for ( CreatorIT itr = creatorBegin(); itr != creatorEnd(); itr++ )
  {
    if (*itr == creator)
    {
      m_creators.erase(itr);
      break;
    }
  }
}

BaseViewItem *ViewItemFactory::CreateViewItem(
  BaseModelItem *modelItem,
  QString const &name,
  QVariant const &value,
  FTL::JSONObject* metaData
  )
{
  // iterate in reverse.  This ensures we test the most-specialized types
  // before testing the more generalized types
  for ( CreatorRIT itr = creatorRBegin(); itr != creatorREnd(); itr++ )
  {
    BaseViewItem* viewItem = (*itr)->CreateItem( name, value, metaData );
    if (viewItem)
    {
      viewItem->setBaseModelItem( modelItem );
      viewItem->updateMetadata( metaData );
      return viewItem;
    }
  }

  return 0;
}

BaseViewItem *ViewItemFactory::CreateViewItem(
  BaseModelItem *modelItem
  )
{
  return CreateViewItem(
    modelItem,
    modelItem->GetName(),
    modelItem->GetValue(),
    modelItem->GetMetadata()
    );
}

BaseViewItem *ViewItemFactory::CreateViewItem(
  QString const &name,
  QVariant const &value,
  FTL::JSONObject* metaData
  )
{
  return CreateViewItem(
    0,
    name,
    value,
    metaData
    );
}
