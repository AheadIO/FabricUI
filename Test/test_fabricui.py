import os
import sys
import pytest

@pytest.fixture
def core_client():
    import FabricEngine.Core as Core
    return Core.createClient({'noOptimization': True})

def test_fe_6396(core_client):
    # [andrew 20160330] FE-6364
    if sys.version_info < (2, 7):
        return

    from FabricEngine import FabricUI

    rtValCrash_1 = FabricUI._Test.RTValCrash(core_client)
    rtValCrash_2 = FabricUI._Test.RTValCrash(core_client)
    rtValCrash_1.setRTVal(rtValCrash_2.getRTVal())

    emptyRTVal = rtValCrash_1.getEmptyRTVal()
    print emptyRTVal

def test_fe_6548(core_client):
    from FabricEngine.Canvas.RTValEncoderDecoder import RTValEncoderDecoder
    core_client.loadExtension('FileIO')

    encdec = RTValEncoderDecoder(core_client)

    filepath = core_client.RT.types.FilePath.create(os.path.join('home', 'user', 'test.txt'))
    print 'filepath:'+str(filepath)
    filepath_str = encdec.getAsString(filepath)
    print 'filepath_str:'+filepath_str
    dec_filepath = encdec.getFromString('FilePath', filepath_str)
    print 'dec_filepath:'+str(dec_filepath)
    assert str(filepath) == str(dec_filepath)

    vec3 = core_client.RT.types.Vec3(1,2,3)
    print 'vec3:'+str(vec3)
    vec3_str = encdec.getAsString(vec3)
    print 'vec3_str:'+vec3_str
    dec_vec3 = encdec.getFromString('Vec3', vec3_str)
    print 'dec_vec3:'+str(dec_vec3)
    assert str(vec3) == str(dec_vec3)



