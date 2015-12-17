/*
 *  Copyright 2010-2015 Fabric Software Inc. All rights reserved.
 */

#include <FabricUI/DFG/DFGUICmdHandler.h>

//NOTE: encode/decode behavior is partially a duplicate of the following:
//  #include <Persistence/RTValFromJSONDecoder.hpp>
//  #include <Persistence/RTValToJSONEncoder.hpp>

#include <FTL/JSONValue.h>
#include <FTL/JSONEnc.h>
#include <FTL/Path.h>

FABRIC_UI_DFG_NAMESPACE_BEGIN

QString DFGUICmdHandler::encodeRTValToJSON(
  FabricCore::Context const& context,
  FabricCore::RTVal const& rtVal
  )
{
  if(rtVal.isValid())
  {
    if(rtVal.isObject())
    {
      if(!rtVal.isNullObject())
      {
        FabricCore::RTVal cast = FabricCore::RTVal::Construct(context, "RTValToJSONEncoder", 1, &rtVal);
        if(cast.isValid())
        {
          if(!cast.isNullObject())
          {
            FabricCore::RTVal result = cast.callMethod("String", "convertToString", 0, 0);
            if(result.isValid())
            {
              FTL::CStrRef ref = result.getStringCString();
              if(ref.size() > 0)
              {
                std::string json;
                {
                  FTL::JSONEnc<> jsonEnc( json );
                  FTL::JSONStringEnc<> jsonStringEnc( jsonEnc, ref );
                }
                return QString::fromUtf8( json.c_str() );
              }
            }
          }
        }
      }
    }
  }
  FabricCore::RTVal valueJSON = rtVal.getJSON();
  return QString::fromUtf8( valueJSON.getStringCString() );
}

void DFGUICmdHandler::decodeRTValFromJSON(
  FabricCore::Context const& context,
  FabricCore::RTVal & rtVal,
  QString jsonQS
  )
{
  std::string json = jsonQS.toUtf8().constData();
  if(json.length() > 2)
  {
    try
    {
      if(rtVal.isValid())
      {
        if(rtVal.isObject())
        {
          if(rtVal.isNullObject())
          {
            rtVal = FabricCore::RTVal::Create( context, rtVal.getTypeName().getStringCString(), 0, NULL );
          }

          std::string decodedString;
          {
            FTL::JSONStrWithLoc strWithLoc( json );
            FTL::JSONDec jsonDec( strWithLoc );
            FTL::JSONEnt jsonEnt;
            if ( jsonDec.getNext( jsonEnt )
              && jsonEnt.getType() == jsonEnt.Type_String )
              jsonEnt.stringAppendTo( decodedString );
          }

          if(decodedString.length() > 0)
          {
            FabricCore::RTVal cast = FabricCore::RTVal::Construct(context, "RTValFromJSONDecoder", 1, &rtVal);
            if(cast.isInterface())
            {
              if(!cast.isNullObject())
              {
                FabricCore::RTVal data =
                  FabricCore::RTVal::ConstructString(
                    context,
                    decodedString.data(),
                    decodedString.size()
                    );
                FabricCore::RTVal result = cast.callMethod("Boolean", "convertFromString", 1, &data);
                if(result.isValid())
                {
                  if(result.getBoolean())
                  {
                    return;
                  }
                }
              }
            }
          }
        }
      }
    }
    catch(FabricCore::Exception e)
    {
      printf("decodeRTValFromJSON: Hit exception: %s\n", e.getDesc_cstr());
    }
  }

  rtVal.setJSON( json.c_str() );
}

QString DFGUICmdHandler::NewPresetPathname(
  FabricCore::DFGHost &host,
  QString presetDirPath,
  QString presetName
  )
{
  std::string pathname =
    host.getPresetImportPathname( presetDirPath.toUtf8().constData() );
  if ( !pathname.empty() )
  {
    FTL::PathAppendEntry( pathname, presetName.toUtf8().constData() );
    pathname += FTL_STR(".canvas");
  }
  return QString::fromUtf8( pathname.c_str() );
}

FABRIC_UI_DFG_NAMESPACE_END
