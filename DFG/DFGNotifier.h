//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FTL/StrRef.h>
#include <QtCore/QObject>
#include <stdint.h>

namespace FabricUI {
namespace DFG {

class DFGNotifier : public QObject
{
  Q_OBJECT

public:

  ~DFGNotifier() {}

protected:

  DFGNotifier() {}

  static void Callback(
    void *userdata,
    char const *jsonCStr,
    uint32_t jsonSize
    )
  {
    static_cast<DFGNotifier *>(
      userdata
      )->handle(
        FTL::CStrRef( jsonCStr, jsonSize )
        );
  }

private:

  virtual void handle( FTL::CStrRef jsonStr ) = 0;
};

} // namespace DFG
} // namespace FabricUI
