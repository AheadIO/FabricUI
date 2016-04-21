//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#ifndef FABRICUI_VALUEEDITOR_QVARIANTRTVAL_H
#define FABRICUI_VALUEEDITOR_QVARIANTRTVAL_H

#include <FabricCore.h>
#include <QtCore/QVariant>

// Import RTVal into the QVariant types
Q_DECLARE_METATYPE(FabricCore::RTVal);

inline bool isRTVal( const QVariant& var ) {
  if (var.type() != QVariant::UserType)
    return false;
  return var.userType() == qMetaTypeId<FabricCore::RTVal>();
}

// Initially at least - A set of functions for 
// converting RTVal <--> QVariant
inline QVariant toVariant(const FabricCore::RTVal& val) 
{ 
  return QVariant::fromValue<FabricCore::RTVal>(val); 
}


namespace FabricUI {
namespace ValueEditor {
  
// This class is not a class meant to be constructed,
// instead it overwrites a static member of the
// base class to inject our own versions of some
// functions.  This allows us to present a seamless
// bridge between RTVal and the QVariant
class RTVariant : public QVariant
{
public:
  static void injectRTHandler();

  // It turns out that the built-in versions of
  // of the can/Convert functions mostly ignore
  // the handler, which renders our injection
  // of handling code (and the built-in handler)
  // mostly pointless.  Qt5 supports the handler
  // in a much better method, so we will keep
  // the code as/is, and when upgrading to Qt5
  // fix-up the system so we can just use Qt's
  // built-in functions
  // Note - some of the functions (value<T> etc)
  // are successfully hooked by our injection
  //-------------------------------------------

  // A strict test of the current type, tests
  // Qt type or RTVal type (if appropriate)
  template<typename T>
  static bool isType( QVariant var )
  {
    QVariant::Type testType = QVariant::Type( qMetaTypeId<T>() );
    QVariant::Type currType = var.type();
    return (testType == currType ||
             (
             isRTVal( var ) &&
             canConvert( var, testType )
             )
             );
  }

  // Test if we can convert to the listed type.
  static bool canConvert( const QVariant& var, Type type );

  // Helper function provides east conversion to provided RTVal
  static bool toRTVal( const QVariant& var, FabricCore::RTVal& ioVal );

private:
  // Overrides for built-in functionality

  // isNull tests rtVal for isValid
  static bool rtIsNull( const QVariant::Private *d);

  static bool rtCanConvert( const QVariant::Private *d, Type t );

  static bool rtConvert( const QVariant::Private *d, QVariant::Type t, void *result, bool *ok );

  static void rtStreamDebug( QDebug dbg, const QVariant &v );

  static const void *constData( const QVariant::Private *d );

  static const FabricCore::RTVal& value( const Private *p ) {
    // Code taken from QVariant::value<T>
    return *reinterpret_cast<const FabricCore::RTVal*>(p->is_shared ? p->data.shared->ptr : &p->data.ptr);
  }

  static const Handler* origh;
};

} // namespace FabricUI 
} // namespace ValueEditor 

#endif // FABRICUI_VALUEEDITOR_QVARIANTRTVAL_H
