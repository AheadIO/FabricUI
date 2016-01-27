//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FTL/CStrRef.h>
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
  
  // A state variable tracks whether this class
  // us currently setting metadata.  Used to
  // filter messages coming from the core from
  // messages coming from the ValueEditor
  bool m_settingMetadata;

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
  virtual void setValue(
    QVariant value,
    bool commit,
    QVariant valueAtInteractionBegin
    )
    {}

public:

	BaseModelItem();
	virtual ~BaseModelItem();

	// The model is a tree
	virtual int getNumChildren();

  // Children can be accessed by name or index
  virtual BaseModelItem* getChild(
    int childIndex,
    bool doCreate = true
    );
  virtual BaseModelItem* getChild(
    FTL::CStrRef childName,
    bool doCreate = true
    );

  // Enable switching between index/name
  virtual FTL::CStrRef getChildName( int i );
  virtual int getChildIndex( FTL::CStrRef name );

  /////////////////////////////////////////////////////////////////////////
  // Name
  /////////////////////////////////////////////////////////////////////////

	// The name of this node
  virtual FTL::CStrRef getName() = 0;

  // Implement this function to prevent the system
  // from allowing renames from the UI
  virtual bool canRename() = 0;

  // Implement this to rename the underlying data
  virtual void rename( FTL::CStrRef newName ) = 0;

  // Handle the actual name change in the model
  virtual void onRenamed(
    FTL::CStrRef oldName,
    FTL::CStrRef newName
    ) = 0;

  /////////////////////////////////////////////////////////////////////////
  // Metadata
  /////////////////////////////////////////////////////////////////////////

	// We need to define a metadata syntax for 
	// additional type-info.  For example, it should
	// be possible using metadata to request a value
	// be displayed in a certain way, eg as an angle
	// or as a percentage, or 
	virtual ItemMetadata* getMetadata();

  // A view item may modify metadata to store UI
  // hints in the scene graph.
  // As a note - The metadata set here may not exactly
  // match the metadata queried from GetMetadata
  // due to way metadata is inherited
  void setMetadata( const char* key, const char* val, bool canUndo );

  // Returns true if this class is currently setting
  // metadata.  This is used to filter out Metadata
  // changed notifications made by the core from ones
  // we have initiated ourselves (the assumption is
  // no changes initiated by the ValueEditor should
  // be reflected back to it).
  bool isSettingMetadata(); 

  // Returns true if this value is read-only,
  // or false if it can be set.
  //virtual bool IsReadOnline();

	// Return a copy of this classes value
	virtual QVariant getValue() = 0;

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

private:

  // A modelitem should implement this function to
  // push metadata to the underlying class
  virtual void setMetadataImp( const char* key, const char* val, bool canUndo ) = 0;

public slots:

  void onInteractionBegin();

  void onViewValueChanged( QVariant value );

  void onInteractionEnd( bool );

signals:

	// Connect to this signal to be notified
	// when the core value changes.
	void modelValueChanged( QVariant const &newValue );
};

