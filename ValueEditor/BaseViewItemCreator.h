#pragma once

class BaseModelItem;
class BaseViewItem;

namespace FTL {
  class JSONObject;
}

// A ViewItemCreator allows plugging in custom ViewItem implementations.
// An instance of this class should be registered with the ViewItemFactor.
// Whenever the system needs to create a ViewItem for a piece of data,
// the Factory will query each ViewItemCreator in order until a Creator
// matches the input parameters.
class BaseViewItemCreator
{
  Q_DISABLE_COPY(BaseViewItemCreator);

public:

  // This class will automatically register itself with the 
  // ViewItemFactory on creation.
  BaseViewItemCreator();
  // This class will automatically remove itself from the 
  // ViewItemFactory on release.
  virtual ~BaseViewItemCreator();

  // Implement this function to return true if this 
  // creator can generate a viewitem for the input data.
  //virtual bool Matches(QVariant data, const char* type, const char* tag) = 0;

  // Create the view item.
  virtual BaseViewItem* CreateItem(
    QString const &name,
    QVariant const &value,
    FTL::JSONObject* metaData
    ) = 0;

  // Implement this function to delete this class, it will 
  // be called when the item is released.  The de-allocation
  // should be appropriate to the allocation (eg, if a class
  // is statically allocated, then it should not be deleted at all)
  virtual void DeleteThis() { delete this; };

  // A creators priority defines the order in which it will
  // be tested for compatibilty with a given value, with 0 
  // being the lowest priority.  A higher priority means that
  // the creator will be tested sooner than other creators.
  // In general, the more specialized a ViewItem, the higher its
  // priority should be.  All in-built ViewItems have priorities
  // lower than 5, so ensure your priority is 5 or higher in
  // order to overwrite a default implementation.
  virtual int Priority() { return 5; }
};

//////////////////////////////////////////////////////////////////////////
// In order to add a ViewItem class to the UI system, its necessary to show the
// system how/when it should be created.  For convenience, this macro
// handles defining the generator.  Implement a function matching the signature
// BaseViewItem* YourFnName(const QVariant& data, const QString& name, const char* tag)
// and pass it and your classes name to the macro below.
#define EXPOSE_VIEW_ITEM(ViewItemClass, CreatorFn, _priority) \
class Creator_##ViewItemClass : public BaseViewItemCreator { \
public: \
  Creator_##ViewItemClass() {} \
  BaseViewItem* CreateItem( \
    QString const &name, \
    QVariant const &value, \
    FTL::JSONObject* metaData \
    ) /*override*/ \
  { return ::CreatorFn( name, value, metaData ); } \
  int Priority() /*override*/ { return _priority; } \
}; \
static Creator_##ViewItemClass s_CreatorInstance;
