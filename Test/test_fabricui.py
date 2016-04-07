import sys, unittest

class FabricUITest(unittest.TestCase):
    def test_fe_6396(self):
        # [andrew 20160330] FE-6364
        if sys.version_info < (2, 7):
            return

        from FabricEngine import Core, FabricUI

        client = Core.createClient()
        rtValCrash_1 = FabricUI._Test.RTValCrash(client)
        rtValCrash_2 = FabricUI._Test.RTValCrash(client)
        rtValCrash_1.setRTVal(rtValCrash_2.getRTVal())

if __name__ == "__main__":
    unittest.main()
