#pragma once

#include <assert.h>

//////////////////////////////////////////////////////////////////////////
// This sample model implementation allows us to create a test
// model to run the UI against without loading FE
class SampleModelItem : public BaseModelItem {
	// Sample 
	QString m_name;
	QVariant m_value;

	std::vector<SampleModelItem*> m_children;

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

	virtual QString GetName() /*override*/
	{
		return m_name;
	}

	virtual QString GetMetadata(QString string) /*override*/
	{
		return QString();
	}

	virtual QVariant GetValue() /*override*/
	{
		return m_value;
	}

	virtual void onViewValueChanged(
		QVariant const &value,
		bool commit
		) /*override*/
	{
		if (value != m_value || commit == 1)
		{
      printf( "onViewValueChanged commit=%i\n", commit );
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
};


extern BaseModelItem* BuildSampleModel( FabricCore::Client& client );
