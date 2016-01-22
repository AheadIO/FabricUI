//
// Copyright 2010-2016 Fabric Software Inc. All rights reserved.
//

#pragma once

#include <FabricCore.h>
#include <FTL/ArrayRef.h>
#include <FTL/StrRef.h>
#include <QtCore/QObject.h>
#include <QtCore/QSharedPointer.h>

namespace FabricUI {
namespace DFG {

class DFGExecNotifier : public QObject
{
  Q_OBJECT

public:

  static QSharedPointer<DFGExecNotifier>
  Create( FabricCore::DFGExec exec )
  {
    return QSharedPointer<DFGExecNotifier>(
      new DFGExecNotifier( exec )
      );
  }

  ~DFGExecNotifier();

signals:
  
  void nodePortRenamed(
    FTL::CStrRef nodeName,
    unsigned portIndex,
    FTL::CStrRef oldPortName,
    FTL::CStrRef newPortName
    );

private:

  DFGExecNotifier( FabricCore::DFGExec exec );

  void viewCallback( FTL::CStrRef jsonStr );

  void handle( FTL::CStrRef jsonStr );

  static void ViewCallback(
    void *userData,
    char const *jsonCString,
    uint32_t jsonLength
    );

  class DepthBracket
  {
  public:

    DepthBracket( DFGExecNotifier *notifier )
      : m_notifier( notifier )
    {
      if ( m_notifier->m_depth++ == 0 )
        assert( m_notifier->m_queued.empty() );
    }

    ~DepthBracket()
    {
      if ( --m_notifier->m_depth == 0 )
        assert( m_notifier->m_queued.empty() );
    }

  private:

    DFGExecNotifier *m_notifier;
  };

  FabricCore::DFGView m_view;
  unsigned m_depth;
  std::vector<std::string> m_queued;
};

} // namespace DFG
} // namespace FabricUI
