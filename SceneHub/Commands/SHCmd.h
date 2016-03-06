/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_H__
#define __UI_SCENEHUB_CMD_H__

#include <QtGui/QUndoCommand>
#include <FabricUI/Util/StringUtils.h>
#include <FabricUI/SceneHub/SHGLScene.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHCmd
    {
      public:
        /// Extracts the name of a command.
        /// \param command The command
        /// \param name The command's name
        static bool ExtractName(const std::string &command, std::string &name);

        /// Extracts the parameters from the command string.
        /// \param command The command
        /// \param params The array of parameters as string
        static bool ExtractParams(const std::string &command, std::vector<std::string> &params);

        /// Constructs and executes a command.
        /// \param shGLScene A reference to shGLScene
        /// \param cmdName The name of the command
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SHCmd(
          SHGLScene *shGLScene,
          std::string cmdName, 
          std::string cmdDes, 
          std::vector<FabricCore::RTVal> &params, 
          bool exec = true);

        ~SHCmd() {};

        /// Does nothing (don't call the command in KL).
        void doit();
        /// Undoes the command.
        void undo();
        /// Redoes the command.
        void redo();

        /// Gets the command description
        std::string getDesc() const { assert(wasInvoked()); return m_desc; };
        /// Sets the command decription (here the command itself).
        /// \param desc The description
        void setDesc(std::string desc) {m_desc = desc;};
 

      protected:
        /// Checks if the command has been already applied.
        bool wasInvoked() const { return m_state != State_New; };

        /// Command states
        enum State {
          State_New,
          State_Done,
          State_Undone,
          State_Redone
        };

        State m_state;
        std::string m_desc;
        unsigned m_coreUndoCount;

        /// \internal
        SHGLScene *m_shGLScene;
    };
  }
}

#endif // __UI_SCENEHUB_CMD_H__
