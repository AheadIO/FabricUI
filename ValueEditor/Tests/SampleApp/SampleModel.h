#pragma once

#include <assert.h>
#include "ItemMetadata.h"

//////////////////////////////////////////////////////////////////////////
// This sample model implementation allows us to create a test
// model to run the UI against without loading FE
class SampleModelItem : public BaseModelItem {
	// Sample 
	std::string m_name;
	QVariant m_value;

	std::vector<SampleModelItem*> m_children;
  
  ViewItemMetadata m_metadata;

public:
	SampleModelItem(const char* name, const QVariant& value)
		: BaseModelItem()
		, m_name(name)
		, m_value(value)
	{

	}

	virtual size_t NumChildren() /*override*/
	{
		return m_children.size();
	}

	virtual BaseModelItem* GetChild(int i) /*override*/
	{
		return m_children[i];
	}

	virtual FTL::CStrRef getName() /*override*/
	{
		return m_name;
	}

	virtual ItemMetadata* GetMetadata()
	{
		return &m_metadata;
	}

	virtual QVariant GetValue() /*override*/
	{
		return m_value;
	}

	virtual void onViewValueChanged(
		QVariant value
		) /*override*/
	{
		if (value != m_value)
		{
      printf( "onViewValueChanged\n" );
      m_value = value;

			// Our value has changed, so notify the world
			emit modelValueChanged( m_value );
		}
	}

	// Testing functions
	void AddChild(SampleModelItem* child)
	{
		m_children.push_back(child);
	}

	void SetValueDirect(QVariant& value)
	{
		assert(value.type() == m_value.type());
		m_value = value;
		emit modelValueChanged( m_value );
	}

  virtual bool hasDefault() override;

  virtual void resetToDefault() override;

};


extern BaseModelItem* BuildSampleModel( FabricCore::Client& client );
