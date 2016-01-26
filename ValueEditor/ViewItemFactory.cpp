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
#include "NotInspectableViewItem.h"
#include "QVariantRTVal.h"
#include "RTValViewItem.h"
#include "SIntViewItem.h"
#include "StringViewItem.h"
#include "UIntViewItem.h"
#include "Vec2ViewItem.h"
#include "Vec3ViewItem.h"
#include "Vec4ViewItem.h"
#include "ComboBoxViewItem.h"
#include "ArrayViewItem.h"
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

bool ViewItemFactory::registerCreator(
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

void ViewItemFactory::deRegisterCreator( CreateItemFn createItemFn )
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

BaseViewItem *ViewItemFactory::createViewItem(
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
    registerCreator( BooleanCheckBoxViewItem::CreateItem, BooleanCheckBoxViewItem::Priority );
    registerCreator( ColorViewItem::CreateItem, ColorViewItem::Priority );
    registerCreator( DefaultViewItem::CreateItem, DefaultViewItem::Priority );
    registerCreator( FilepathViewItem::CreateItem, FilepathViewItem::Priority );
    registerCreator( FloatSliderViewItem::CreateItem, FloatSliderViewItem::Priority );
    registerCreator( FloatViewItem::CreateItem, FloatViewItem::Priority );
    registerCreator( IntSliderViewItem::CreateItem, IntSliderViewItem::Priority );
    registerCreator( NotInspectableViewItem::CreateItem, NotInspectableViewItem::Priority );
    registerCreator( RTValViewItem::CreateItem, RTValViewItem::Priority );
    registerCreator( SIntViewItem::CreateItem, SIntViewItem::Priority );
    registerCreator( StringViewItem::CreateItem, StringViewItem::Priority );
    registerCreator( UIntViewItem::CreateItem, UIntViewItem::Priority );
    registerCreator( Vec2ViewItem::CreateItem, Vec2ViewItem::Priority );
    registerCreator( Vec3ViewItem::CreateItem, Vec3ViewItem::Priority );
    registerCreator( Vec4ViewItem::CreateItem, Vec4ViewItem::Priority );
    registerCreator( ComboBoxViewItem::CreateItem, ComboBoxViewItem::Priority );
    registerCreator( ArrayViewItem::CreateItem, ArrayViewItem::Priority );

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

BaseViewItem *ViewItemFactory::createViewItem(
  BaseModelItem *modelItem
  )
{
  FTL::CStrRef name = modelItem->getName();
  return createViewItem(
    modelItem,
    QString::fromUtf8( name.data(), name.size() ),
    modelItem->getValue(),
    modelItem->getMetadata()
    );
}

BaseViewItem *ViewItemFactory::createViewItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* metaData
  )
{
  return createViewItem(
    0,
    name,
    value,
    metaData
    );
}
