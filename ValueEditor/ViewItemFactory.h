//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtCore/QObject>
#include <vector>

class BaseViewItem;
class BaseModelItem;
class ItemMetadata;

// This factory is responsible for returning the appropriate BaseViewItem
// when creating for a given data type
class ViewItemFactory /*sealed*/
{
	Q_DISABLE_COPY(ViewItemFactory)

public:
	
	typedef BaseViewItem *(*CreateItemFn)(
	  QString const &name,
	  QVariant const &value,
	  ItemMetadata* metaData
	  );

private:
	
	struct ItemCreator
	{
		CreateItemFn createItemFn;
		int priority;

		ItemCreator( CreateItemFn _createItemFn, int _priority )
			: createItemFn( _createItemFn ), priority( _priority ) {}

		// sort using a custom function object
		struct Sort
		{
		  bool operator()( ItemCreator const &a, ItemCreator const &b )
		  {
		    return a.priority < b.priority;
		  }
		};
	};

	std::vector<ItemCreator> m_creators;

protected:

	typedef std::vector<ItemCreator>::iterator CreatorIT;
	CreatorIT creatorBegin() { return m_creators.begin(); }
	CreatorIT creatorEnd() { return m_creators.end(); }

	typedef std::vector<ItemCreator>::const_reverse_iterator CreatorRIT;
	CreatorRIT creatorRBegin() const { return m_creators.rbegin(); }
	CreatorRIT creatorREnd() const { return m_creators.rend(); }

	//////////////////////////////////////////////////////////////////////////
	ViewItemFactory();
	~ViewItemFactory();

	// Create the most appropriate value editor given the following data
	// \data The data being edited.  We create a BaseViewItem that matches the
	//	type of this data.
	// \viewtype[optional] What dimension the data represents.  For example, a 
	//		float can represent any of an angle/ratio/distance etc.
	// \tag[optional] One more piece of metadata, just in case you want to 
	//		only display a value for a tagged value.
	// TODO: Perhaps just pass in a MetaData option here to represent all metadata?
	BaseViewItem* CreateViewItem(
	  BaseModelItem *modelItem,
		QString const &name,
		QVariant const &value,
    ItemMetadata* metaData
	  );

public:

	static ViewItemFactory* GetInstance();
	//////////////////////////////////////////////////////////////////////////

	// Register arbitrary widget creators 
	bool RegisterCreator( CreateItemFn createItemFn, int priority );
	// DeRegister widget creators.
	void DeRegisterCreator( CreateItemFn createItemFn );
	
	// Recursively Create the most appropriate value editor 
  // given the modelItem
	// \data The modelitem to represent in the UI
	BaseViewItem* CreateViewItem(
	  BaseModelItem *modelItem
	  );
	
	// Create the most appropriate value editor given the following data
	// \data The data being edited.  We create a BaseViewItem that matches the
	//	type of this data.
	// \viewtype[optional] What dimension the data represents.  For example, a 
	//		float can represent any of an angle/ratio/distance etc.
	// \metadata[optional] One more piece of metadata, just in case you want to 
	//		only display a value for a tagged value.
	BaseViewItem* CreateViewItem(
		QString const &name,
		QVariant const &value,
    ItemMetadata* metaData = NULL
	  );
};
