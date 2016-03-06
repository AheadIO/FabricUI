/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#include "SHCmd.h"

using namespace FabricUI;
using namespace FabricUI::SceneHub;
 
/// Extracts the parameters from the command string.
/// \param client The core client
/// \param command The command
/// \param params The array of parameters as string
bool SHCmd::ExtractParams(const std::string &command, std::vector<std::string> &params) {
  std::vector<std::string> split = FabricUI::Util::Split(command, '(');
  if(split.size() == 2)
  {
    std::vector<std::string> split_ = FabricUI::Util::Split(split[1], ')');
    if(split_.size() == 2)
    {             
      std::string paramArray = FabricUI::Util::RemoveSpace(split_[0]);

      // Parameter like {...,...,...}
      size_t begin = paramArray.find("{");
      size_t end = paramArray.find_last_of("}");
    
      if(begin < end)
      {
        // Get the index to insert the {} params
        std::string temp = paramArray.substr(0, begin);
        size_t index = std::count(temp.begin(), temp.end(), ',');

        // Get the {} params string
        std::string params_ = paramArray.substr(begin, end-begin+1);
        // Get the {} params string
        size_t index2 = paramArray.substr(end, paramArray.length()-1).find(",");
        paramArray.erase(begin, end+index2+1-begin);
        
        // Construct the vector of parameters
        std::vector<std::string> tempArray = FabricUI::Util::Split(paramArray, ',');
        for(uint32_t i=0; i<index; ++i) params.push_back(tempArray[i]);
        params.push_back(params_);
        for(uint32_t i=index; i<tempArray.size(); ++i) params.push_back(tempArray[i]);
      }
      else 
      {
        params = FabricUI::Util::Split(paramArray, ',');
        if(params.size() == 0 && paramArray.compare("") != 0)
          params.push_back(paramArray);
      }

      return true;
    }
  }
  return false;
}

/// Extracts the name of a command.
/// \param command The command
/// \param name The command's name
bool SHCmd::ExtractName(const std::string &command, std::string &name) {
  std::vector<std::string> split = FabricUI::Util::Split(command, '(');
  if(split.size() > 1) 
  {
    name = split[0];
    return true;
  }
  return false;
}

/// Constructs and executes a command.
/// \param shObject A reference to SceneHub application
/// \param cmdName The name of the command
/// \param cmdDes The command desciprtion
/// \param params The command parameters
/// \param exec If true executes the command, just add it to the Qt stack otherwise
SHCmd::SHCmd(
  //FabricCore::RTVal &shObject,
  SHGLScene *shGLScene,
  std::string cmdName, 
  std::string cmdDes, 
  std::vector<FabricCore::RTVal> &params, 
  bool exec) 
  : m_state(State_New),
    m_coreUndoCount(0), 
    m_shGLScene(shGLScene) 
{
  setDesc(cmdDes);
  // !!! executeCmd(std::string cmdName, std::vector<RTVal> &params)
}

/// Does nothing (don't call the command in KL).
void SHCmd::doit() {
  assert(m_state == State_New);
  ++m_coreUndoCount;
  m_state = State_Done;
}

/// Undoes the command.
void SHCmd::undo() {
  assert(m_state == State_Done || m_state == State_Redone);
  m_state = State_Undone;
  m_shGLScene->undoCmd(m_coreUndoCount);
}

/// Redoes the command.
void SHCmd::redo() {
  assert( m_state = State_Undone );
  m_state = State_Redone; 
  m_shGLScene->redoCmd(m_coreUndoCount);
}
