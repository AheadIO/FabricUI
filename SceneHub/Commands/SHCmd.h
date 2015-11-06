// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __FABRICUI_SceneHub_Cmd__
#define __FABRICUI_SceneHub_Cmd__

#include <cctype>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <FabricCore.h>
#include <FTL/StrRef.h>
#include <FTL/StrRef.h>
#include <QtGui/QUndoCommand>
#include <FabricUI/SceneHub/macros.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    /// Lower a string
    /// \param s The string to lower
    inline std::string ToLower(const std::string &s) {
      std::string str = s;
      transform(str.begin(), str.end(), str.begin(),::tolower);
      return str;
    }

    /// Checks if a string is a number.
    /// \param s The string to check
    inline bool IsNumber(const std::string& s) {
      return std::all_of(s.begin(), s.end(), ::isdigit);
    }

    /// Removes the white space within the string.
    /// \param s The string to check
    inline std::string RemoveWithSpace(const std::string& s) {
      std::string str = s;
      str.erase( std::remove_if(str.begin(), str.end(), ::isspace), str.end());
      return str;
    }

    /// Converts a number into to string.
    /// \param t The number
    template <typename T> std::string ToStr(T t) { 
      std::ostringstream os; 
      os<<t; 
      return os.str(); 
    } 

    /// Converts a string into a number.
    /// \param s The string to check
    template<typename T> T ToNum(const std::string &number) {
      
      T value;
      if(IsNumber(number)) {
        std::stringstream stream(number);
        stream >> value;
        if(stream.fail()) {
          std::runtime_error e(number);
          throw e;
        }
      }

      return value;
    }

    /// Spits a string into a vector.
    /// \param s The string to split
    /// \param c The delimiter tat splits the string
    static std::vector<std::string> Split(const std::string& s, char c) {

      std::vector<std::string> v;
      std::string::size_type i = 0;
      std::string::size_type j = s.find(c);

      while (j != std::string::npos) {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j == std::string::npos)
         v.push_back(s.substr(i, s.length()));
      }
      return v;
    }


    class SHCmd
    {
      public:
        /// Constructs a command.
        /// \param cmdManager A reference to the SGCmdManager (kl).
        SHCmd(FabricCore::RTVal &cmdManager) : 
          m_cmdManager(cmdManager), 
          m_coreUndoCount(0), 
          m_state(State_New) {};

        ~SHCmd() {};

        /// Does nothing (don't call the command in KL).
        void doit() {
          assert(m_state == State_New);
          ++m_coreUndoCount;
          m_state = State_Done;
        };

        /// Undoes the command.
        void undo() {
          assert(m_state == State_Done || m_state == State_Redone);
          m_state = State_Undone;
          FABRIC_TRY("SHCmd::undo", 
            for(unsigned i=0; i<m_coreUndoCount; ++i)
              FabricCore::RTVal res = m_cmdManager.callMethod("Boolean", "undo", 0, 0);
          );
        };

        /// Redoes the command.
        void redo() {
          assert( m_state = State_Undone );
          m_state = State_Redone;          
          FABRIC_TRY("SHCmd::redo", 
            for ( unsigned i = 0; i < m_coreUndoCount; ++i )
              FabricCore::RTVal res = m_cmdManager.callMethod("", "redo", 0, 0);
          );
        };

        /// Undoes the commands
        std::string getDesc() {
          assert(wasInvoked());
          return m_desc;
        };

        /// Undoes the commands
        void setDesc(std::string desc) {
          m_desc = desc;
        };

      private:
        /// Checks if the command has been already applied.
        bool wasInvoked() const { return m_state != State_New; };
 
        enum State {
          State_New,
          State_Done,
          State_Undone,
          State_Redone
        };

        std::string m_desc;
        FabricCore::RTVal m_cmdManager;
        unsigned m_coreUndoCount;
        State m_state;
    };
  };
};

#endif // __FABRICUI_SceneHub_Cmd__
