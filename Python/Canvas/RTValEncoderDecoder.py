"""
The RTValEncoderDecoder is responsible for encoding and decoding RTVals
to and from strings. This is used by the ScriptEditor to allow sending
plain Python commands to the Core that include the contents of RTVals
and is also used by the Fabric Core for persistence to allow saving
the contents of a scene to JSON.
"""

import json
from FabricEngine import Core

class RTValEncoderDecoder:

    def __init__(self, client):
        self.client = client

    def decode(self, rtval, codecContext, string, metadataLookup):
        try:
            if not rtval.isObject():
                return True
            if rtval.isNullObject():
                return True
            cast = self.client.RT.types.RTValFromJSONDecoder(rtval)
            if cast.isNullObject():
                return True
            decodedString = json.loads(string)
            rtvalString = self.client.RT.types.String(decodedString)
            result = cast.convertFromString('Boolean', rtvalString)
            if result.getSimpleType():
                return cast
            return True
        except Exception as _e:
            return True

    def encode(self, rtval, codecContext, metadataLookup):
        try:
            if codecContext == Core.CAPI.RTValCodecContext_BindingArgument \
                    and metadataLookup \
                    and metadataLookup("uiPersistValue") != "true":
                return False
            if not rtval.isObject():
                return True
            if rtval.isNullObject():
                return True
            cast = self.client.RT.types.RTValToJSONEncoder(rtval)
            if cast.isNullObject():
                return True
            result = cast.convertToString('String')
            ref = result.getSimpleType()
            if len(ref) < 1:
                return True
            return json.dumps(ref)
        except Exception as _e:
            return True

    def getAsString(self, rtval):
        result = self.encode(rtval, None, None)
        if type(result) == bool:
            return rtval.getJSONStr()
        return result

    def getFromString(self, typeName, string):
        rtvalType = getattr(self.client.RT.types, typeName)
        if self.client.RT.getRegisteredTypeIsObject(typeName):
            rtvalConstruct = rtvalType.create
        else:
            rtvalConstruct = rtvalType
        rtval = rtvalConstruct()
        result = self.decode(rtval, None, string, None)
        if type(result) == bool:
            rtval.setJSON(string)
        return rtval

