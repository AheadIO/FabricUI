/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef FABRICUI_TEST_RTVALCRASH_H
#define FABRICUI_TEST_RTVALCRASH_H
 
#include <FabricCore.h>

namespace FabricUI {
namespace _Test {

class RTValCrash {

  public:
    RTValCrash(FabricCore::Client client) {
      m_rtVal = FabricCore::RTVal::Construct(client, "String", 0, 0);
    }

    ~RTValCrash() {}
    
    FabricCore::RTVal getRTVal() { return m_rtVal; }

    void setRTVal(FabricCore::RTVal rtVal) { m_rtVal = rtVal; }

  private:    
    /// \internal
    FabricCore::RTVal m_rtVal;
};

}
}


#endif // FABRICUI_TEST_RTVALCRASH_H
