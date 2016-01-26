/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_CMD_H__
#define __UI_SCENEHUB_CMD_H__


#include <QtGui/QUndoCommand>
#include <FabricUI/Util/macros.h>
 
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
      std::string::const_iterator it = s.begin();
      while (it != s.end() && std::isdigit(*it)) ++it;
      return !s.empty() && it == s.end();
    }

    /// Removes the character within the string.
    /// \param s The string to check
    /// \param c The character to remove
    inline std::string RemoveCharacter(const std::string& s, char c) {
      std::string str = s;
      str.erase( std::remove(str.begin(), str.end(), c), str.end());
      return str;
    }

    /// Removes the newline within the string.
    /// \param s The string to check
    inline std::string RemoveNewLine(const std::string& s) {
      std::string str = s;
      str.erase( std::remove(str.begin(), str.end(), '\n'), str.end());
      str.erase( std::remove(str.begin(), str.end(), '\r'), str.end());
      return str;
    }

    /// Removes the white space within the string.
    /// \param s The string to check
    inline std::string RemoveSpace(const std::string& s) {
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
      T value = 0;
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
    inline std::vector<std::string> Split(const std::string& s, char c) {

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
        /// Encodes a rtVal into a Json, saves the rtVal
        /// \param client The core client
        /// \param rtVal The value to encode
        static std::string EncodeRTValToJSON(FabricCore::Client const& client, FabricCore::RTVal const& rtVal);

        /// Decodes a rtVal from a Json, reads the rtVal
        /// \param client The core client
        /// \param rtVal The result value
        /// \param json The string to decode
        static void DecodeRTValFromJSON(FabricCore::Client const& client, FabricCore::RTVal &rtVal, FTL::CStrRef json); 
        
        /// Extracts the name of a command.
        /// \param command The command
        /// \param name The command's name
        static bool ExtractName(const std::string &command, std::string &name);

        /// Extracts the parameters from the command string.
        /// \param command The command
        /// \param params The array of parameters as string
        static bool ExtractParams(const std::string &command, std::vector<std::string> &params);

        /// From the parameter type and its value, creates 
        /// \param client The core client
        /// \param type The parameter type
        /// \param value The parameter value JSon encoded
        static FabricCore::RTVal SetParamValue(FabricCore::Client const& client, std::string const& type, std::string const& value);

        /// Gets the command manager.
        /// \param shObject A reference to SceneHub application
        static FabricCore::RTVal GetCmdManager(FabricCore::RTVal &shObject);

        /// Gets the command at index i of KL stack.
        /// \param client A reference to the fabric client
        /// \param shObject A reference to SceneHub application
        /// \param index The name of the object
        static FabricCore::RTVal RetrieveCmd(FabricCore::Client &client, FabricCore::RTVal &shObject, uint32_t index);

        /// Constructs and executes a command.
        /// \param shObject A reference to SceneHub application
        /// \param cmdName The name of the command
        /// \param cmdDes The command desciprtion
        /// \param params The command parameters
        /// \param exec If true executes the command, just add it to the Qt stack otherwise
        SHCmd(FabricCore::RTVal &shObject,
          const std::string &cmdName, 
          const std::string &cmdDes, 
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

        /// Gets a reference to the sceneHub application. 
        FabricCore::RTVal& getRefOnSCeneHub() { return m_shObject; }

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
        /// Refenrece to the sceneHub applcaiton.
        FabricCore::RTVal m_shObject;
    };
  };
};

#endif // __UI_SCENEHUB_CMD_H__
