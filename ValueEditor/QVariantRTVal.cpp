#include "StdAfx.h"

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
  return (int)d->type >= rtType;
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
  if (isRTVal( d ))
  {
    const FabricCore::RTVal& val = value( d );
    if (!val.isValid())
      return false;

    const char* rtype = val.getTypeNameCStr();

    switch (t)
    {
      case QVariant::Bool:
        return strcmp( rtype, "Boolean" ) == 0;
      case QVariant::Int:
        return strcmp( rtype, "SInt32" ) == 0 ||
          strcmp( rtype, "SInt16" ) == 0 ||
          strcmp( rtype, "SInt8" ) == 0;
      case QVariant::UInt:
        return strcmp( rtype, "UInt32" ) == 0 ||
          strcmp( rtype, "UInt16" ) == 0 ||
          strcmp( rtype, "byte" ) == 0 ||
          strcmp( rtype, "Size" ) == 0 ||
          strcmp( rtype, "Count" ) == 0 ||
          strcmp( rtype, "Index" ) == 0 ||
          strcmp( rtype, "UInt8" ) == 0;
      case QVariant::LongLong:
        return strcmp( rtype, "SInt64" ) == 0;
      case QVariant::ULongLong:
        return strcmp( rtype, "UInt64" ) == 0;
      case QMetaType::Float:
        return strcmp( rtype, "Float32" ) == 0 ||
          strcmp( rtype, "Scalar" ) == 0;
      case QVariant::Double:
        return strcmp( rtype, "Float32" ) == 0 ||
          strcmp( rtype, "Float64" ) == 0 ||
          strcmp( rtype, "Scalar" ) == 0;
      case QVariant::String:
        return true; // Anything can be converted to a string
      case QVariant::Color:
        return strcmp( rtype, "RGB" ) == 0 ||
          strcmp( rtype, "RGBA" ) == 0 ||
          strcmp( rtype, "Color" ) == 0;
      case QVariant::Matrix:
        return strcmp( rtype, "Mat22" ) == 0;
      case QVariant::Transform:
        return strcmp( rtype, "Mat33" ) == 0;
      case QVariant::Matrix4x4:
        return strcmp( rtype, "Mat44" ) == 0;
      case QVariant::Vector2D:
        return strcmp( rtype, "Vec2" ) == 0;
      case QVariant::Vector3D:
        return strcmp( rtype, "Vec3" ) == 0;
      case QVariant::Vector4D:
        return strcmp( rtype, "Vec4" ) == 0;
      case QVariant::Quaternion:
        return strcmp( rtype, "Quat" ) == 0;

        // For these more complex types, 
        // a user should directly work with
        // the RTVal rather than incur the cost
        // of copying to QType

      case QVariant::SizeF:
      case QVariant::Char:
      case QVariant::Map:
      case QVariant::List:
      case QVariant::StringList:
      case QVariant::ByteArray:
      case QVariant::BitArray:
      case QVariant::Date:
      case QVariant::Time:
      case QVariant::DateTime:
      case QVariant::Url:
      case QVariant::Locale:
      case QVariant::Rect:
      case QVariant::RectF:
      case QVariant::Size:
      case QVariant::Line:
      case QVariant::LineF:
      case QVariant::Point:
      case QVariant::PointF:
      case QVariant::RegExp:
      case QVariant::Hash:
      case QVariant::EasingCurve:
      case QVariant::Font:
      case QVariant::Pixmap:
      case QVariant::Brush:
      case QVariant::Palette:
      case QVariant::Icon:
      case QVariant::Image:
      case QVariant::Polygon:
      case QVariant::Region:
      case QVariant::Bitmap:
      case QVariant::Cursor:
      case QVariant::SizePolicy:
      case QVariant::KeySequence:
      case QVariant::Pen:
      case QVariant::TextLength:
      case QVariant::TextFormat:
        return false;
      default:
        assert( !"Unknown type" );
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
    const char* rtype = val.getTypeNameCStr();
    switch (t)
    {
      case QVariant::Bool:
        if (strcmp( rtype, "Boolean" ))
        {
          bool& b = *((bool*)result);
          b = val.getBoolean();
        }
        break;
      case QVariant::Int:
      {
        int& v = *((int*)result);
        if (strcmp(rtype, "SInt32") == 0)
        {
          v = val.getSInt32();
        }
        else if (strcmp(rtype, "SInt16") == 0)
        {
          v = val.getSInt16();
        }
        else if (strcmp(rtype, "SInt8") == 0)
        {
          v = val.getSInt8();
        }
      }
      break;
      case QVariant::UInt:
      {
        unsigned int& v = *((unsigned int*)result);
        if (strcmp(rtype, "UInt32") == 0)
        {
          v = val.getUInt32();
        }
        else if (strcmp(rtype, "UInt16") == 0)
        {
          v = val.getUInt16();
        }
        else if (strcmp(rtype, "UInt8") == 0)
        {
          v = val.getUInt8();
        }
        else if (strcmp( rtype, "byte" ) == 0)
        {
          v = val.getUInt8();
        }
        else if (strcmp( rtype, "Size" ) == 0)
        {
          v = val.getUInt32();
        }
        else if (strcmp( rtype, "Count" ) == 0)
        {
          v = val.getUInt32();
        }
        else if (strcmp( rtype, "Index" ) == 0)
        {
          v = val.getUInt32();
        }
      }
      break;
      case QVariant::LongLong:
        if ( rtype, "SInt64" )
        {
          long long int& v = *((long long int*)result);
          v = val.getSInt64();
        }
        break;
      case QVariant::ULongLong:
      {
        unsigned long long int& v = *((unsigned long long int*)result);
        if (strcmp(rtype, "UInt64") == 0)
        {
          v = val.getUInt64();
        }
      }
      break;
      case QMetaType::Float:
        if (strcmp( rtype, "Float32" ) == 0 ||
          strcmp( rtype, "Scalar" ) == 0)
        {
          float& v = *((float*)result);
          v = val.getFloat32();
        }
      case QVariant::Double:
      {
        double& v = *((double*)result);
        if (strcmp( rtype, "Float32" ) == 0)
        {
          v = val.getFloat32();
        }
        else if (strcmp( rtype, "Float64" ) == 0)
        {
          v = val.getFloat64();
        }
        else if (strcmp( rtype, "Scalar" ) == 0)
        {
          v = val.getFloat32();
        }
      }
      break;
      case QVariant::String:
      {
        QString& str = *((QString*)result);
        if (strcmp( rtype, "String" ) == 0)
          str = val.getStringCString();
        else
        {
          FabricCore::RTVal desc = val.getDesc();
          str = desc.getStringCString();
        }
        break;
      }
      case QVariant::Color:
      {
        QColor& v = *((QColor*)result);
        if (strcmp( rtype, "RGB" ) == 0)
        {
          v.setRed( val.maybeGetMember( "r" ).getUInt8() );
          v.setGreen( val.maybeGetMember( "g" ).getUInt8() );
          v.setBlue( val.maybeGetMember( "b" ).getUInt8());
        }
        else if (strcmp( rtype, "RGBA" ) == 0)
        {
          v.setRed( val.maybeGetMember( "r" ).getUInt8() );
          v.setGreen( val.maybeGetMember( "g" ).getUInt8() );
          v.setBlue( val.maybeGetMember( "b" ).getUInt8() );
          v.setAlpha( val.maybeGetMember( "a" ).getUInt8() );
        }
        else if (strcmp( rtype, "Color" ) == 0)
        {
          v.setRedF( val.maybeGetMember( "r" ).getFloat32() );
          v.setGreenF( val.maybeGetMember( "g" ).getFloat32() );
          v.setBlueF( val.maybeGetMember( "b" ).getFloat32() );
          v.setAlphaF( val.maybeGetMember( "a" ).getFloat32() );
        }
      }
      break;
      //case QVariant::Matrix:
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
      case QVariant::Transform:
        if ( strcmp( rtype, "Mat33" ) == 0 )
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
      case QVariant::Matrix4x4:
        if ( strcmp( rtype, "Mat44" ) == 0 )
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
      case QVariant::Vector2D:
        if ( strcmp( rtype, "Vec2" ) == 0 )
        {
          QVector2D& v = *((QVector2D*)result);
          v.setX(val.maybeGetMember( "x" ).getFloat32());
          v.setY(val.maybeGetMember( "y" ).getFloat32());
        }
        break;
      case QVariant::Vector3D:
        if ( strcmp( rtype, "Vec3" ) == 0 )
        {
          QVector3D& v = *((QVector3D*)result);
          v.setX(val.maybeGetMember( "x" ).getFloat32());
          v.setY(val.maybeGetMember( "y" ).getFloat32());
          v.setZ(val.maybeGetMember( "z" ).getFloat32());
        }
        break;
      case QVariant::Vector4D:
        if ( strcmp( rtype, "Vec4" ) == 0 )
        {
          QVector4D& v = *((QVector4D*)result);
          v.setX( val.maybeGetMember( "x" ).getFloat32() );
          v.setY( val.maybeGetMember( "y" ).getFloat32() );
          v.setZ( val.maybeGetMember( "z" ).getFloat32());
          v.setW( val.maybeGetMember( "w" ).getFloat32());
        }
        break;
      case QVariant::Quaternion:
        if ( strcmp( rtype, "Quat" ) == 0 )
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
  // the correct type.  In this case, just set it on ioVal and return
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
    const char* rtype = ioVal.getTypeNameCStr();
    FabricCore::Context ctxt;// = ioVal.getContext();
    if (strcmp( rtype, "Boolean" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructBoolean( ctxt, var.toBool() );
    }
    else if (strcmp( rtype, "SInt32" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructSInt32( ctxt, var.toInt() );
    }
    else if (strcmp( rtype, "SInt16" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructSInt16( ctxt, var.toInt() );
    }
    else if (strcmp( rtype, "SInt8" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructSInt8( ctxt, var.toInt() );
    }
    else if (strcmp( rtype, "UInt32" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt32( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "UInt16" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt16( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "UInt8" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt8( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "byte" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt8( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "Size" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt32( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "Count" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt32( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "Index" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt32( ctxt, var.toUInt() );
    }
    else if (strcmp( rtype, "SInt64" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructSInt64( ctxt, var.toLongLong() );
    }
    else if (strcmp( rtype, "UInt64" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructUInt64( ctxt, var.toULongLong() );
    }
    else if (strcmp( rtype, "Float32" ) == 0 ||
              strcmp( rtype, "Scalar" ) == 0)
    {
      ioVal.setFloat32( var.toFloat() );
    }
    else if (strcmp( rtype, "Float64" ) == 0)
    {
      ioVal = FabricCore::RTVal::ConstructFloat64( ctxt, var.toDouble() );
    }
    else if (strcmp( rtype, "String" ) == 0)
    {
      QString str = var.toString();
      QByteArray asAscii = str.toAscii();
      ioVal = FabricCore::RTVal::ConstructString( ctxt, asAscii.data(), asAscii.size() );
    }
    else if (strcmp( rtype, "RGB" ) == 0)
    {
      QColor color = var.value<QColor>();
      ioVal.setMember( "r", FabricCore::RTVal::ConstructUInt8( ctxt, color.red() ) );
      ioVal.setMember( "g", FabricCore::RTVal::ConstructUInt8( ctxt, color.green() ) );
      ioVal.setMember( "b", FabricCore::RTVal::ConstructUInt8( ctxt, color.blue() ) );
    }
    else if (strcmp( rtype, "RGBA" ) == 0)
    {
      QColor color = var.value<QColor>();
      ioVal.setMember( "r", FabricCore::RTVal::ConstructUInt8( ctxt, color.red() ) );
      ioVal.setMember( "g", FabricCore::RTVal::ConstructUInt8( ctxt, color.green() ) );
      ioVal.setMember( "b", FabricCore::RTVal::ConstructUInt8( ctxt, color.blue() ) );
      ioVal.setMember( "a", FabricCore::RTVal::ConstructUInt8( ctxt, color.alpha() ) );
    }
    else if (strcmp( rtype, "Color" ) == 0)
    {
      QColor color = var.value<QColor>();
      ioVal.maybeGetMemberRef( "r" ).setFloat32( color.redF() );
      ioVal.maybeGetMemberRef( "g" ).setFloat32( color.greenF() );
      ioVal.maybeGetMemberRef( "b" ).setFloat32( color.blueF() );
      ioVal.maybeGetMemberRef( "a" ).setFloat32( color.alphaF() );
    }
    else if (strcmp( rtype, "Mat33" ) == 0)
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
    else if (strcmp( rtype, "Mat44" ) == 0)
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
    else if (strcmp( rtype, "Vec2" ) == 0)
    {
      QVector2D v = var.value<QVector2D>();
      ioVal.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      ioVal.maybeGetMemberRef( "y" ).setFloat32( v.y() );
    }
    else if (strcmp( rtype, "Vec3" ) == 0)
    {
      QVector3D v = var.value<QVector3D>();
      ioVal.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      ioVal.maybeGetMemberRef( "y" ).setFloat32( v.y() );
      ioVal.maybeGetMemberRef( "z" ).setFloat32( v.z() );
    }
    else if (strcmp( rtype, "Vec4" ) == 0)
    {
      QVector4D v = var.value<QVector4D>();
      ioVal.maybeGetMemberRef( "x" ).setFloat32( v.x() );
      ioVal.maybeGetMemberRef( "y" ).setFloat32( v.y() );
      ioVal.maybeGetMemberRef( "z" ).setFloat32( v.z() );
      ioVal.maybeGetMemberRef( "w" ).setFloat32( v.w() );
    }
    else if (strcmp( rtype, "Quat" ) == 0)
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
