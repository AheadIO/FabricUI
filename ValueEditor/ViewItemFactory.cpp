//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "BaseModelItem.h"
#include "BaseViewItem.h"
#include "ColorViewItem.h"
#include "DefaultViewItem.h"
#include "FloatSliderViewItem.h"
#include "FloatViewItem.h"
#include "QVariantRTVal.h"
#include "RTValViewItem.h"
#include "Vec3ViewItem.h"
#include "ViewItemFactory.h"

ViewItemFactory::ViewItemFactory()
{

}

ViewItemFactory::~ViewItemFactory()
{
}

ViewItemFactory* ViewItemFactory::GetInstance()
{
  static ViewItemFactory instance;
  return &instance;
}


BaseViewItem *ViewItemFactory::BuildView( BaseModelItem *modelItem )
{
  static bool initialized = false;
  if ( !initialized )
  {
    // Register the built-in view items
    RegisterCreator( ColorViewItem::CreateItem, ColorViewItem::Priority );
    RegisterCreator( DefaultViewItem::CreateItem, DefaultViewItem::Priority );
    RegisterCreator( FloatViewItem::CreateItem, FloatViewItem::Priority );
    RegisterCreator( FloatSliderViewItem::CreateItem, FloatSliderViewItem::Priority );
    RegisterCreator( RTValViewItem::CreateItem, RTValViewItem::Priority );
    RegisterCreator( Vec3ViewItem::CreateItem, Vec3ViewItem::Priority );

    // We put the QVariant-RTVal bridge injection
    // code here, as before we build a view it won't
    // be needed
    RTVariant::injectRTHandler();

    initialized = true;
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

bool ViewItemFactory::RegisterCreator(
  CreateItemFn createItemFn,
  int priority
  )
{
  for ( CreatorIT itr = creatorBegin(); itr != creatorEnd(); itr++ )
  {
    if ( itr->createItemFn == createItemFn )
      return false;
  }
  m_creators.push_back( ItemCreator( createItemFn, priority ) );
  // Sort creators in order of priority
  std::sort( m_creators.begin(), m_creators.end(), ItemCreator::Sort() );
  return true;
}

void ViewItemFactory::DeRegisterCreator( CreateItemFn createItemFn )
{
  for ( CreatorIT itr = creatorBegin(); itr != creatorEnd(); itr++ )
  {
    if ( itr->createItemFn == createItemFn )
    {
      m_creators.erase( itr );
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
    if ( BaseViewItem* viewItem = itr->createItemFn( name, value, metaData ) )
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
