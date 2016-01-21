//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#include "BaseModelItem.h"
#include "BaseViewItem.h"
#include "BooleanCheckBoxViewItem.h"
#include "ColorViewItem.h"
#include "DefaultViewItem.h"
#include "FilepathViewItem.h"
#include "FloatSliderViewItem.h"
#include "FloatViewItem.h"
#include "IntSliderViewItem.h"
#include "QVariantRTVal.h"
#include "RTValViewItem.h"
#include "SIntViewItem.h"
#include "StringViewItem.h"
#include "UIntViewItem.h"
#include "Vec2ViewItem.h"
#include "Vec3ViewItem.h"
#include "Vec4ViewItem.h"
#include "ComboBoxViewItem.h"
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
  ItemMetadata* metaData
  )
{
  static bool initialized = false;
  if ( !initialized )
  {
    // Register the builtin view items
    RegisterCreator( BooleanCheckBoxViewItem::CreateItem, BooleanCheckBoxViewItem::Priority );
    RegisterCreator( ColorViewItem::CreateItem, ColorViewItem::Priority );
    RegisterCreator( DefaultViewItem::CreateItem, DefaultViewItem::Priority );
    RegisterCreator( FilepathViewItem::CreateItem, FilepathViewItem::Priority );
    RegisterCreator( FloatSliderViewItem::CreateItem, FloatSliderViewItem::Priority );
    RegisterCreator( FloatViewItem::CreateItem, FloatViewItem::Priority );
    RegisterCreator( IntSliderViewItem::CreateItem, IntSliderViewItem::Priority );
    RegisterCreator( RTValViewItem::CreateItem, RTValViewItem::Priority );
    RegisterCreator( SIntViewItem::CreateItem, SIntViewItem::Priority );
    RegisterCreator( StringViewItem::CreateItem, StringViewItem::Priority );
    RegisterCreator( UIntViewItem::CreateItem, UIntViewItem::Priority );
    RegisterCreator( Vec2ViewItem::CreateItem, Vec2ViewItem::Priority );
    RegisterCreator( Vec3ViewItem::CreateItem, Vec3ViewItem::Priority );
    RegisterCreator( Vec4ViewItem::CreateItem, Vec4ViewItem::Priority );
    RegisterCreator( ComboBoxViewItem::CreateItem, ComboBoxViewItem::Priority );

     // We put the QVariantRTVal bridge injection
    // code here, as before we build a view it won't
    // be needed
    RTVariant::injectRTHandler();

    initialized = true;
  }

  // iterate in reverse.  This ensures we test the most-specialized types
  // before testing the more generalized types
  for ( CreatorRIT itr = creatorRBegin(); itr != creatorREnd(); itr++ )
  {
    if ( BaseViewItem* viewItem = itr->createItemFn( name, value, metaData ) )
    {
      viewItem->setBaseModelItem( modelItem );
      return viewItem;
    }
  }

  return 0;
}

BaseViewItem *ViewItemFactory::CreateViewItem(
  BaseModelItem *modelItem
  )
{
  FTL::CStrRef name = modelItem->getName();
  return CreateViewItem(
    modelItem,
    QString::fromUtf8( name.data(), name.size() ),
    modelItem->GetValue(),
    modelItem->GetMetadata()
    );
}

BaseViewItem *ViewItemFactory::CreateViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  return CreateViewItem(
    0,
    name,
    value,
    metaData
    );
}
