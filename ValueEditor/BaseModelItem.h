//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <QtCore/QObject>
#include <QtCore/QVariant>

class ItemMetadata;

// The base model item is the base class for defining
// items on the model.  This provides a common
// interface for supplying data from the FabricCore, and
// connecting notifications.  The view consumes a 
// tree/list? of BaseModelItems to build it's UI
class BaseModelItem : public QObject
{
	Q_OBJECT

	Q_DISABLE_COPY(BaseModelItem);

private:

  unsigned m_interactionBracketCount;
  QVariant m_valueAtInteractionBegin;

  unsigned m_modelValueChangedBracketCount;

  class ModelValueChangedBracket
  {
  public:

    ModelValueChangedBracket( BaseModelItem *modelItem )
      : m_modelItem( modelItem )
    {
      ++m_modelItem->m_modelValueChangedBracketCount;
    }

    ~ModelValueChangedBracket()
    {
      --m_modelItem->m_modelValueChangedBracketCount;
    }

  private:

    BaseModelItem *m_modelItem;
  };

protected:

  // The BaseModelItem is responsible for receiving values back from 
  // the the UI and setting them on the core object.
  // It is guaranteed that the QVariant value here will be equivalent
  // to the QVariant returned from GetValue
  virtual void SetValue(
    QVariant value,
    bool commit,
    QVariant valueAtInteractionBegin
    )
    {}

public:

	BaseModelItem();
	virtual ~BaseModelItem();

	// The model is a tree
	virtual size_t NumChildren() = 0;

  // Children should generally be accessed by name or index
  virtual BaseModelItem* GetChild( int childIndex ) = 0;
  virtual BaseModelItem* GetChild( QString childName );

  // Enable switching between index/name
  virtual QString ChildName( int i );
  virtual int ChildIndex( QString name );

	// The name of this node
	virtual QString GetName() = 0;
  // Implement this to rename the underlying data
  virtual void RenameItem( const char* newName ) = 0;
  // This function will be called when the name is
  // updated in the core, and can be used to update
  // any cached name strings.
  virtual void OnItemRenamed( QString newName );

	// We need to define a metadata syntax for 
	// additional type-info.  For example, it should
	// be possible using metadata to request a value
	// be displayed in a certain way, eg as an angle
	// or as a percentage, or 
	virtual ItemMetadata* GetMetadata();

  // Implement this function to indicate which
  // direction the value is heading in this
  // NOTE: the returned value is equivalent to
  // FabricCore::DFGPortType
  // Default: FabricCore::DFGPortType_In
  virtual int GetInOut();

  // Returns true if this value is read-only,
  // or false if it can be set.
  //virtual bool IsReadOnline();

	// Return a copy of this classes value
	virtual QVariant GetValue() = 0;

  // Returns true if a ModelItem has a default
  // value - in other words, if resetToDefault
  // will have an effect
  virtual bool hasDefault() = 0;

  // An implementation should implement this function
  // to reset its value back to the default
  virtual void resetToDefault() = 0;

  // Allow others to trigger these signals...
  void emitModelValueChanged( QVariant const &newValue ) 
  {
    ModelValueChangedBracket _( this );
    emit modelValueChanged( newValue );
  }
  void emitChildInserted( int index, const char* name, const char* type ) 
    { emit childInserted( index, name, type ); }
  void emitRemoved()
    { emit removed(); }
  void emitDataTypeChanged( const char* newType )
    { emit dataTypeChanged( newType ); }

public slots:

  void onInteractionBegin();

  void onViewValueChanged( QVariant value );

  void onInteractionEnd( bool );

signals:

	// Connect to this signal to be notified
	// when the core value changes.
	void modelValueChanged( QVariant const &newValue );

  // This signal is fired whenever a new child is
  // added to this modelItem
  // \param index The index of the newly added item
  void childInserted( int index, const char* name, const char* type );

  // This signal is fired whenever this modelitem
  // is being removed
  // \param index The index of the item to be removed
  void removed( );

  // Fire this signal if the type of the data served
  // by this class changes (the QVariant type).
  void dataTypeChanged( const char* newType );
};

