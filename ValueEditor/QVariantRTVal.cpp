//
// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.
//

#include "QVariantRTVal.h"

#include <assert.h>
#include <QtGui/QtGui>


const QVariant::Handler* RTVariant::origh = NULL;

// Code replaces the built-in handler for several
// operations in QVariant
void RTVariant::injectRTHandler()
{
  origh = handler;
  Handler* h = new Handler;
  *h = *origh;
  h->isNull = rtIsNull;
  h->canConvert = rtCanConvert;
  h->convert = rtConvert;
  h->debugStream = rtStreamDebug;
  handler = h;
}

bool isRTVal( const QVariant::Private *d )
{
  static const QVariant::Type rtType = QVariant::Type( qMetaTypeId<FabricCore::RTVal>() );
  return int( d->type ) >= int( rtType );
}

bool RTVariant::canConvert( const QVariant & var, Type type )
{
  return rtCanConvert( &reinterpret_cast<const RTVariant&>(var).d, type );
}

bool RTVariant::rtIsNull( const QVariant::Private *d )
{
  if (isRTVal(d))
  {
    const FabricCore::RTVal& val = value( d );
    if (!val.isValid() || (val.isObject() && val.isNullObject()))
      return true;
    return false;
  }
  return origh->isNull( d );
}

bool RTVariant::rtCanConvert( const QVariant::Private *d, Type t )
{
  if ( isRTVal( d ) )
  {
    const FabricCore::RTVal& val = value( d );
    if ( !val.isValid() )
      return false;

    switch ( int( t ) )
    {
      case int( QVariant::Bool ):
        return val.isBoolean();
      case int( QVariant::Int ):
        return val.isSInt8() || val.isSInt16() || val.isSInt32();
      case int( QVariant::UInt ):
        return val.isUInt8() || val.isUInt16() || val.isUInt32();
      case int( QVariant::LongLong ):
        return val.isSInt64();
      case int( QVariant::ULongLong ):
        return val.isUInt64();
      case int( QMetaType::Float ):
        return val.isFloat32();
      case int( QVariant::Double ):
        return val.isFloat64();
      case int( QVariant::String ):
        return val.isString();
      case int( QVariant::Color ):
        return val.hasType( "RGB" ) ||
          val.hasType( "RGBA" ) ||
          val.hasType( "ARGB" ) ||
          val.hasType( "Color" );
      case int( QVariant::Matrix ):
        return val.hasType( "Mat22" );
      case int( QVariant::Transform ):
        return val.hasType( "Mat33" );
      case int( QVariant::Matrix4x4 ):
        return val.hasType( "Mat44" );
      case int( QVariant::Vector2D ):
        return val.hasType( "Vec2" );
      case int( QVariant::Vector3D ):
        return val.hasType( "Vec3" );
      case int( QVariant::Vector4D ):
        return val.hasType( "Vec4" );
      case int( QVariant::Quaternion ):
        return val.hasType( "Quat" );

        // For these more complex types, 
        // a user should directly work with
        // the RTVal rather than incur the cost
        // of copying to QType

      default:
        return false;
    }
  }
  return origh->canConvert( d, t );
}

bool RTVariant::rtConvert( const QVariant::Private *d, QVariant::Type t, void *result, bool *ok )
{
  //qDebug() << Q_FUNC_INFO << "type:" << d->type;
  if (isRTVal( d ))
  {
    const FabricCore::RTVal& val = value( d );
    
    switch ( int ( t ) )
    {
      case int( QVariant::Bool ):
        if ( val.isBoolean() )
        {
          bool& b = *((bool*)result);
          b = val.getBoolean();
        }
        break;
      case int( QVariant::Int ):
      {
        int& v = *((int*)result);
        FabricCore::RTVal::SimpleData simpleData;
        if ( val.maybeGetSimpleData( &simpleData ) )
        {
          switch ( simpleData.type )
          {
            case FEC_RTVAL_SIMPLE_TYPE_BOOLEAN:
              v = int( simpleData.value.boolean );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT8:
              v = int( simpleData.value.uint8 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT16:
              v = int( simpleData.value.uint16 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT32:
              v = int( simpleData.value.uint32 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT64:
              v = int( simpleData.value.uint64 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT8:
              v = int( simpleData.value.sint8 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT16:
              v = int( simpleData.value.sint16 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT32:
              v = int( simpleData.value.sint32 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT64:
              v = int( simpleData.value.sint64 );
              break;
            default:
              break;
          }
        }
      }
      break;
      case int( QVariant::UInt ):
      {
        unsigned int& v = *((unsigned int*)result);
        FabricCore::RTVal::SimpleData simpleData;
        if ( val.maybeGetSimpleData( &simpleData ) )
        {
          switch ( simpleData.type )
          {
            case FEC_RTVAL_SIMPLE_TYPE_BOOLEAN:
              v = unsigned( simpleData.value.boolean );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT8:
              v = unsigned( simpleData.value.uint8 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT16:
              v = unsigned( simpleData.value.uint16 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT32:
              v = unsigned( simpleData.value.uint32 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_UINT64:
              v = unsigned( simpleData.value.uint64 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT8:
              v = unsigned( simpleData.value.sint8 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT16:
              v = unsigned( simpleData.value.sint16 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT32:
              v = unsigned( simpleData.value.sint32 );
              break;
            case FEC_RTVAL_SIMPLE_TYPE_SINT64:
              v = unsigned( simpleData.value.sint64 );
              break;
            default:
              break;
          }
        }
      }
      break;
      case int( QVariant::LongLong ):
        if ( val.isSInt64() )
        {
          long long int& v = *((long long int*)result);
          v = val.getSInt64();
        }
        break;
      case int( QVariant::ULongLong ):
      {
        unsigned long long int& v = *((unsigned long long int*)result);
        if ( val.isUInt64() )
        {
          v = val.getUInt64();
        }
      }
      break;
      case int( QMetaType::Float ):
        if ( val.isFloat32() )
        {
          float& v = *((float*)result);
          v = val.getFloat32();
        }
      case int( QVariant::Double ):
      {
        double& v = *((double*)result);
        if ( val.isFloat32() )
        {
          v = val.getFloat32();
        }
        else if ( val.isFloat64() )
        {
          v = val.getFloat64();
        }
      }
      break;
      case int( QVariant::String ):
      {
        QString& str = *((QString*)result);
        if ( val.isString() )
          str = val.getStringCString();
        else
        {
          FabricCore::RTVal desc = val.getDesc();
          str = desc.getStringCString();
        }
        break;
      }
      case int( QVariant::Color ):
      {
        QColor& v = *((QColor*)result);
        if ( val.hasType( "RGB" ) )
        {
          v.setRed( val.maybeGetMember( "r" ).getUInt8() );
          v.setGreen( val.maybeGetMember( "g" ).getUInt8() );
          v.setBlue( val.maybeGetMember( "b" ).getUInt8());
        }
        else if ( val.hasType( "RGBA" )
          || val.hasType( "ARGB" ) )
        {
          v.setRed( val.maybeGetMember( "r" ).getUInt8() );
          v.setGreen( val.maybeGetMember( "g" ).getUInt8() );
          v.setBlue( val.maybeGetMember( "b" ).getUInt8() );
          v.setAlpha( val.maybeGetMember( "a" ).getUInt8() );
        }
        else if ( val.hasType( "Color" ) )
        {
          v.setRedF( val.maybeGetMember( "r" ).getFloat32() );
          v.setGreenF( val.maybeGetMember( "g" ).getFloat32() );
          v.setBlueF( val.maybeGetMember( "b" ).getFloat32() );
          v.setAlphaF( val.maybeGetMember( "a" ).getFloat32() );
        }
      }
      break;
      //case int( QVariant::Matrix ):
      //  if ( strcmp( rtype, "Mat22" ) == 0 )
      //  {
      //    QMatrix& v = *((QMatrix*)result);
      //    FabricCore::RTVal row0 = val.maybeGetMemberRef( "row0" );
      //    v.m11 = row0.maybeGetMember( "x" ).getFloat32();
      //    v.m12 = row0.maybeGetMember( "y" ).getFloat32();

      //    FabricCore::RTVal row1 = val.maybeGetMemberRef( "row1" );
      //    v.m21 = row1.maybeGetMember( "x" ).getFloat32();
      //    v.m22 = row1.maybeGetMember( "y" ).getFloat32();
      //  }
      //  break;
      case int( QVariant::Transform ):
        if ( val.hasType( "Mat33" ) )
        {
          QTransform& v = *((QTransform*)result);
          FabricCore::RTVal row0 = val.maybeGetMemberRef( "row0" );
          FabricCore::RTVal row1 = val.maybeGetMemberRef( "row1" );
          FabricCore::RTVal row2 = val.maybeGetMemberRef( "row2" );
          v.setMatrix(
            row0.maybeGetMember( "x" ).getFloat32(),
            row0.maybeGetMember( "y" ).getFloat32(),
            row0.maybeGetMember( "z" ).getFloat32(),

            row1.maybeGetMember( "x" ).getFloat32(),
            row1.maybeGetMember( "y" ).getFloat32(),
            row1.maybeGetMember( "z" ).getFloat32(),

            row2.maybeGetMember( "x" ).getFloat32(),
            row2.maybeGetMember( "y" ).getFloat32(),
            row2.maybeGetMember( "z" ).getFloat32()
            );
        }
        break;
      case int( QVariant::Matrix4x4 ):
        if ( val.hasType( "Mat44" ) )
        {
          QMatrix4x4& v = *((QMatrix4x4*)result);
          FabricCore::RTVal row0 = val.maybeGetMemberRef( "row0" );
          FabricCore::RTVal row1 = val.maybeGetMemberRef( "row1" );
          FabricCore::RTVal row2 = val.maybeGetMemberRef( "row2" );
          FabricCore::RTVal row3 = val.maybeGetMemberRef( "row3" );

          v(0, 0) = row0.maybeGetMember( "x" ).getFloat32();
          v(0, 1) = row0.maybeGetMember( "y" ).getFloat32();
          v(0, 2) = row0.maybeGetMember( "z" ).getFloat32();
          v(0, 3) = row0.maybeGetMember( "w" ).getFloat32();

          v(1, 0) = row1.maybeGetMember( "x" ).getFloat32();
          v(1, 1) = row1.maybeGetMember( "y" ).getFloat32();
          v(1, 2) = row1.maybeGetMember( "z" ).getFloat32();
          v(1, 3) = row1.maybeGetMember( "w" ).getFloat32();

          v( 2, 0 ) = row2.maybeGetMember( "x" ).getFloat32();
          v( 2, 1 ) = row2.maybeGetMember( "y" ).getFloat32();
          v( 2, 2 ) = row2.maybeGetMember( "z" ).getFloat32();
          v( 2, 3 ) = row2.maybeGetMember( "w" ).getFloat32();

          v( 3, 0 ) = row3.maybeGetMember( "x" ).getFloat32();
          v( 3, 1 ) = row3.maybeGetMember( "y" ).getFloat32();
          v( 3, 2 ) = row3.maybeGetMember( "z" ).getFloat32();
          v( 3, 3 ) = row3.maybeGetMember( "w" ).getFloat32();
        }
        break;
      case int( QVariant::Vector2D ):
        if ( val.hasType( "Vec2" ) )
        {
          QVector2D& v = *((QVector2D*)result);
          v.setX(val.maybeGetMember( "x" ).getFloat32());
          v.setY(val.maybeGetMember( "y" ).getFloat32());
        }
        break;
      case int( QVariant::Vector3D ):
        if ( val.hasType( "Vec3" ) )
        {
          QVector3D& v = *((QVector3D*)result);
          v.setX(val.maybeGetMember( "x" ).getFloat32());
          v.setY(val.maybeGetMember( "y" ).getFloat32());
          v.setZ(val.maybeGetMember( "z" ).getFloat32());
        }
        break;
      case int( QVariant::Vector4D ):
        if ( val.hasType( "Vec4" ) )
        {
          QVector4D& v = *((QVector4D*)result);
          v.setX( val.maybeGetMember( "x" ).getFloat32() );
          v.setY( val.maybeGetMember( "y" ).getFloat32() );
          v.setZ( val.maybeGetMember( "z" ).getFloat32());
          v.setW( val.maybeGetMember( "t" ).getFloat32());
        }
        break;
      case int( QVariant::Quaternion ):
        if ( val.hasType( "Quat" ) )
        {
          QQuaternion& v = *((QQuaternion*)result);
          FabricCore::RTVal row = val.maybeGetMemberRef( "v" );
          v.setX(row.maybeGetMember( "x" ).getFloat32());
          v.setY(row.maybeGetMember( "y" ).getFloat32());
          v.setZ(row.maybeGetMember( "z" ).getFloat32());
          v.setScalar(val.maybeGetMember( "w" ).getFloat32());
        }
        break;
      default:
        assert( !"Invalid conversion: Implement this type" );
    }
  }
  // Apply default conversion
  else
    return origh->convert( d, t, result, ok );
  return true;
}

void RTVariant::rtStreamDebug( QDebug dbg, const QVariant &v )
{
  if (isRTVal( v ))
    dbg << v.value<QString>();
  else
    origh->debugStream( dbg, v );
}

bool RTVariant::toRTVal( const QVariant & var, FabricCore::RTVal & ioVal )
{
  static const int rtType = qMetaTypeId<FabricCore::RTVal>();
  // Assume - if the value being passed is an RTVal already, it is
  // the correct type.  In this case,int(  just set it on ioVal and retur )n
  int type = var.userType();
  if (type == rtType)
    ioVal = var.value<FabricCore::RTVal>();
  else
  {
    // A single QVariant type can translate to multiple RTVal
    // types (eg QColor -> RGB && RGBA && Color).  Because we
    // cannot know the correct type from just the QVariant, we
    // require a valid type to cast to.
    if (!ioVal.isValid())
      return false;

    // If not, do the translation from the QVariant type
    // to the appropriate RTVal type
    FabricCore::Context ctxt = ioVal.getContext();
    if ( ioVal.isBoolean() )
    {
      ioVal = FabricCore::RTVal::ConstructBoolean( ctxt, var.toBool() );
    }
    else if ( ioVal.isSInt32() )
    {
      ioVal = FabricCore::RTVal::ConstructSInt32( ctxt, var.toInt() );
    }
    else if ( ioVal.isSInt16() )
    {
      ioVal = FabricCore::RTVal::ConstructSInt16( ctxt, var.toInt() );
    }
    else if ( ioVal.isSInt8() )
    {
      ioVal = FabricCore::RTVal::ConstructSInt8( ctxt, var.toInt() );
    }
    else if ( ioVal.isUInt32() )
    {
      ioVal = FabricCore::RTVal::ConstructUInt32( ctxt, var.toUInt() );
    }
    else if ( ioVal.isUInt16() )
    {
      ioVal = FabricCore::RTVal::ConstructUInt16( ctxt, var.toUInt() );
    }
    else if ( ioVal.isUInt8() )
    {
      ioVal = FabricCore::RTVal::ConstructUInt8( ctxt, var.toUInt() );
    }
    else if ( ioVal.isSInt64() )
    {
      ioVal = FabricCore::RTVal::ConstructSInt64( ctxt, var.toLongLong() );
    }
    else if ( ioVal.isUInt64() )
    {
      ioVal = FabricCore::RTVal::ConstructUInt64( ctxt, var.toULongLong() );
    }
    else if ( ioVal.isFloat32() )
    {
      ioVal.setFloat32( var.toFloat() );
    }
    else if ( ioVal.isFloat64() )
    {
      ioVal = FabricCore::RTVal::ConstructFloat64( ctxt, var.toDouble() );
    }
    else if ( ioVal.isString() )
    {
      QString str = var.toString();
      QByteArray asAscii = str.toAscii();
      ioVal = FabricCore::RTVal::ConstructString( ctxt, asAscii.data(), asAscii.size() );
    }
    else if ( ioVal.hasType( "RGB" ) )
    {
      QColor color = var.value<QColor>();
      ioVal.setMember( "r", FabricCore::RTVal::ConstructUInt8( ctxt, color.red() ) );
      ioVal.setMember( "g", FabricCore::RTVal::ConstructUInt8( ctxt, color.green() ) );
      ioVal.setMember( "b", FabricCore::RTVal::ConstructUInt8( ctxt, color.blue() ) );
    }
    else if ( ioVal.hasType( "RGBA" ) || ioVal.hasType( "ARGB" ) )
    {
      QColor color = var.value<QColor>();
      ioVal.setMember( "r", FabricCore::RTVal::ConstructUInt8( ctxt, color.red() ) );
      ioVal.setMember( "g", FabricCore::RTVal::ConstructUInt8( ctxt, color.green() ) );
      ioVal.setMember( "b", FabricCore::RTVal::ConstructUInt8( ctxt, color.blue() ) );
      ioVal.setMember( "a", FabricCore::RTVal::ConstructUInt8( ctxt, color.alpha() ) );
    }
    else if ( ioVal.hasType( "Color" ) )
    {
      QColor color = var.value<QColor>();
      ioVal.maybeGetMemberRef( "r" ).setFloat32( color.redF() );
      ioVal.maybeGetMemberRef( "g" ).setFloat32( color.greenF() );
      ioVal.maybeGetMemberRef( "b" ).setFloat32( color.blueF() );
      ioVal.maybeGetMemberRef( "a" ).setFloat32( color.alphaF() );
    }
    else if ( ioVal.hasType( "Mat33" ) )
    {
      QTransform transform = var.value<QTransform>();
      FabricCore::RTVal row0 = ioVal.maybeGetMemberRef( "row0" );
      FabricCore::RTVal row1 = ioVal.maybeGetMemberRef( "row1" );
      FabricCore::RTVal row2 = ioVal.maybeGetMemberRef( "row2" );
      
      row0.maybeGetMemberRef( "x" ).setFloat32( transform.m11() ),
      row0.maybeGetMemberRef( "y" ).setFloat32( transform.m12() ),
      row0.maybeGetMemberRef( "z" ).setFloat32( transform.m13() ),

      row1.maybeGetMemberRef( "x" ).setFloat32( transform.m21() );
      row1.maybeGetMemberRef( "y" ).setFloat32( transform.m22() );
      row1.maybeGetMemberRef( "z" ).setFloat32( transform.m23() );

      row2.maybeGetMemberRef( "x" ).setFloat32( transform.m31() );
      row2.maybeGetMemberRef( "y" ).setFloat32( transform.m32() );
      row2.maybeGetMemberRef( "z" ).setFloat32( transform.m33() );  
    }
    else if ( ioVal.hasType( "Mat44" ) )
    {
      QMatrix4x4 qmat = var.value<QMatrix4x4>();
      FabricCore::RTVal row0 = ioVal.maybeGetMemberRef( "row0" );
      FabricCore::RTVal row1 = ioVal.maybeGetMemberRef( "row1" );
      FabricCore::RTVal row2 = ioVal.maybeGetMemberRef( "row2" );
      FabricCore::RTVal row3 = ioVal.maybeGetMemberRef( "row3" );

      row0.maybeGetMemberRef( "x" ).setFloat32( qmat( 0, 0 ) );
      row0.maybeGetMemberRef( "y" ).setFloat32( qmat( 0, 1 ) );
      row0.maybeGetMemberRef( "z" ).setFloat32( qmat( 0, 2 ) );
      row0.maybeGetMemberRef( "w" ).setFloat32( qmat( 0, 3 ) );

      row1.maybeGetMemberRef( "x" ).setFloat32( qmat( 1, 0 ) );
      row1.maybeGetMemberRef( "y" ).setFloat32( qmat( 1, 1 ) );
      row1.maybeGetMemberRef( "z" ).setFloat32( qmat( 1, 2 ) );
      row1.maybeGetMemberRef( "w" ).setFloat32( qmat( 1, 3 ) );

      row2.maybeGetMemberRef( "x" ).setFloat32( qmat( 2, 0 ) );
      row2.maybeGetMemberRef( "y" ).setFloat32( qmat( 2, 1 ) );
      row2.maybeGetMemberRef( "z" ).setFloat32( qmat( 2, 2 ) );
      row2.maybeGetMemberRef( "w" ).setFloat32( qmat( 2, 3 ) );

      row3.maybeGetMemberRef( "x" ).setFloat32( qmat( 3, 0 ) );
      row3.maybeGetMemberRef( "y" ).setFloat32( qmat( 3, 1 ) );
      row3.maybeGetMemberRef( "z" ).setFloat32( qmat( 3, 2 ) );
      row3.maybeGetMemberRef( "w" ).setFloat32( qmat( 3, 3 ) );
    }
    else if ( ioVal.hasType( "Vec2" ) )
    {
      QVector2D v = var.value<QVector2D>();
      ioVal.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      ioVal.maybeGetMemberRef( "y" ).setFloat32( v.y() );
    }
    else if ( ioVal.hasType( "Vec3" ) )
    {
      QVector3D v = var.value<QVector3D>();
      ioVal.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      ioVal.maybeGetMemberRef( "y" ).setFloat32( v.y() );
      ioVal.maybeGetMemberRef( "z" ).setFloat32( v.z() );
    }
    else if ( ioVal.hasType( "Vec4" ) )
    {
      QVector4D v = var.value<QVector4D>();
      ioVal.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      ioVal.maybeGetMemberRef( "y" ).setFloat32( v.y() );
      ioVal.maybeGetMemberRef( "z" ).setFloat32( v.z() );
      ioVal.maybeGetMemberRef( "t" ).setFloat32( v.w() );
    }
    else if ( ioVal.hasType( "Quat" ) )
    {
      QQuaternion v = var.value<QQuaternion>();
      FabricCore::RTVal row = ioVal.maybeGetMemberRef( "v" );
      row.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      row.maybeGetMemberRef( "y" ).setFloat32( v.y() );
      row.maybeGetMemberRef( "z" ).setFloat32( v.z() );
      ioVal.maybeGetMemberRef( "w" ).setFloat32( v.scalar() );
    }
    else
    {
      // return a NULL value, this will probably throw sooner or later.
      assert( false );
      return false;
    }
  }

  return true;
}
