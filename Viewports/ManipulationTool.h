#ifndef __MANIPULATIONTOOL_H__
#define __MANIPULATIONTOOL_H__

#include <QtCore/QObject>
#include <QtCore/QEvent>

#include <FabricCore.h>

#include <Commands/Command.h>
#include "GLViewportWidget.h"

namespace FabricUI
{
  namespace Viewports
  {
    class ManipulationCmd : FabricServices::Commands::Command
    {

    private:
      FabricCore::RTVal m_rtval_commands;
      
    public:
      ManipulationCmd(); 
      virtual ~ManipulationCmd(); 
      
      virtual const char * getName() const { return "ManipulationCmd"; }
      virtual const char * getShortDesc() const { return "Performs any manipulation within a Fabric application."; }
      virtual const char * getFullDesc() const { return getShortDesc(); }

      static void setStaticRTValCommands(FabricCore::RTVal commands) { s_rtval_commands = commands; }

    protected:
      
      virtual bool invoke();
      virtual bool undo();

      // We set the static commands pointer, and then construct the command. 
      static FabricCore::RTVal s_rtval_commands;
    };

    class ManipulationTool 
    {
    public:
      ManipulationTool(GLViewportWidget * glView);
      virtual ~ManipulationTool() {}

      virtual void toolOnSetup();
      virtual void toolOffCleanup();

      bool onEvent(QEvent *event);
      bool isActive() { return m_active;}

    private:
      bool  m_active;
      FabricCore::RTVal mEventDispatcher;

      GLViewportWidget * m_view;
    };
  };
};

#endif
