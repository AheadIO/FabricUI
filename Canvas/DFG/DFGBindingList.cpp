// Copyright 2010-2015 Fabric Software Inc. All rights reserved.


#include "DFGBindingList.h"

#include <FTL/AutoSet.h>
#include <FTL/JSONEnc.h>
#include <FTL/JSONValue.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGBindingList::DFGBindingList(
  QObject * parent,
  FabricCore::Client &client,
  FabricCore::DFGHost &host
  )
: m_client(client)
, m_host(host)
, m_activeIndex(UINT_MAX)
, m_blockSignals(false)
{
  clear();
}

DFGBindingList::~DFGBindingList()
{
}

char const * DFGBindingList::getBindingTitle(unsigned int index)
{
  return getBinding(index).getExec().getMetadata("uiTitle");
}

void DFGBindingList::clear()
{
  m_bindings.clear();
  addBinding("Graph");
}

void DFGBindingList::setActiveBindingIndex(unsigned int index)
{
  if(index < m_bindings.size())
  {
    if(m_activeIndex != index)
    {
      m_activeIndex = index;
      if(!m_blockSignals)
        emit activeBindingChanged(m_activeIndex);
    }
  }
}

void DFGBindingList::setBindingTitle(unsigned int index, FTL::CStrRef name)
{
  if(name != getBindingTitle(index))
  {
    std::string uniqueName = getUniqueName(name);
    getBinding(index).getExec().setMetadata("uiTitle", uniqueName.c_str());
    if(!m_blockSignals)
      emit bindingRenamed(index, uniqueName.c_str());
  }
}

void DFGBindingList::addBinding(FTL::CStrRef name)
{
  FabricCore::DFGBinding binding = m_host.createBindingToNewGraph();
  binding.getExec().setMetadata("uiTitle", getUniqueName(name).c_str());
  BindingInfo info;
  info.binding = binding;
  m_bindings.push_back(info);
  if(!m_blockSignals)
  {
    emit bindingCreated((int)(m_bindings.size()-1));
    emit bindingsChanged();
  }
  setActiveBindingIndex(m_bindings.size()-1);
}

void DFGBindingList::addBindingFromFile(FTL::CStrRef filePath)
{
  FILE * f = fopen(filePath.c_str(), "rb");
  if(!f)
    return;

  fseek( f, 0, SEEK_END );
  int fileSize = ftell( f );
  rewind( f );

  char * buffer = (char*) malloc(fileSize + 1);
  buffer[fileSize] = '\0';

  fread(buffer, fileSize, 1, f);
  fclose(f);

  std::string json = buffer;
  free(buffer);

  try
  {
    FabricCore::DFGBinding binding = m_host.createBindingFromJSON(json.c_str());
    FTL::StrRef uiTitleRef = binding.getExec().getMetadata("uiTitle");
    if(uiTitleRef.empty())
      binding.getExec().setMetadata("uiTitle", getUniqueName("Graph").c_str());
    BindingInfo info;
    info.binding = binding;
    m_bindings.push_back(info);
    
    // we still emit this signal so that apps
    // can set their variables on them
    emit bindingCreated((int)(m_bindings.size()-1));
    
    if(!m_blockSignals)
    {
      emit bindingsChanged();
    }
    setActiveBindingIndex(m_bindings.size()-1);
  }
  catch(FabricCore::Exception e)
  {
    printf("FabricCore: %s\n", e.getDesc_cstr());
  }
}

void DFGBindingList::saveBindingToFile(int index, FTL::CStrRef filePath)
{
  FILE * f = fopen(filePath.c_str(), "wb");
  if(!f)
    return;

  FabricCore::String singleGraphJSON = getBinding(index).exportJSON();
  std::string json = singleGraphJSON.getCString();
  fwrite(json.c_str(), json.length(), 1, f);
  fclose(f);
}

void DFGBindingList::removeBinding(int index)
{
  if(m_bindings.size() == 1)
  {
    clear();
    return;
  }
  if(index < m_bindings.size() && index >= 0)
  {
    m_bindings.erase(m_bindings.begin() + index);
    if(!m_blockSignals)
      emit bindingsChanged();
    if(index < m_bindings.size())
      setActiveBindingIndex(index);
    else
      setActiveBindingIndex(m_bindings.size()-1);
  }
}

std::string DFGBindingList::persistToJSON()
{
  std::string result;
  {
    FTL::JSONEnc<> enc( result );
    FTL::JSONObjectEnc<> objEnc( enc );
    {
      FTL::JSONEnc<> graphsEnc( objEnc, FTL_STR("graphs") );
      FTL::JSONArrayEnc<> graphsArrayEnc( graphsEnc );
      for(unsigned int i=0;i<getCount();i++)
      {
        FabricCore::String singleGraphJSON = getBinding(i).exportJSON();
        FTL::CStrRef ref = singleGraphJSON.getCString();
        FTL::JSONEnc<> singleGraphEnc( graphsArrayEnc );
        FTL::JSONStringEnc<> singleGraphObjEnc( singleGraphEnc, ref );
      }
    }
  }
  return result;
}

void DFGBindingList::restoreFromJSON(FTL::CStrRef json)
{
  {
    FTL::AutoSet<bool> blockSignals(m_blockSignals, true);

    try
    {
      FTL::JSONStrWithLoc jsonStrWithLoc( json.c_str() );
      FTL::OwnedPtr<FTL::JSONObject> jsonObject(
        FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
        );

      m_bindings.clear();

      FTL::JSONArray const * varsArray = jsonObject->maybeGetArray( FTL_STR("graphs") );
      for(size_t i=0;i<varsArray->size();i++)
      {
        FTL::JSONValue const *singleGraphJSON = varsArray->get( i );
        if( !singleGraphJSON )
          continue;
        FTL::JSONString const *singleGraphJSONStr = singleGraphJSON->maybeCast<FTL::JSONString>();    
        if(!singleGraphJSONStr)
          continue;
      
        FabricCore::DFGBinding binding = m_host.createBindingFromJSON(singleGraphJSONStr->getValue().c_str());
        BindingInfo info;
        info.binding = binding;
        m_bindings.push_back(info);
    
        // we still emit this signal so that apps
        // can set their variables on them
        emit bindingCreated((int)(m_bindings.size()-1));
      }
    }
    catch(FabricCore::Exception e)
    {
      printf("FabricCore: %s\n", e.getDesc_cstr());
    }
    catch(FTL::JSONException e)
    {
      printf("JSON: %s\n", e.getDescCStr());
    }

    prepAllBindingForExecution();
  }

  if(m_bindings.size() == 0)
    addBinding("Graph");
  else
  {
    emit bindingsChanged();
    setActiveBindingIndex(0);
  }
}

bool DFGBindingList::persistToFile(FTL::CStrRef filePath)
{
  FILE * f = fopen(filePath.c_str(), "wb");
  if(!f)
    return false;

  std::string json = persistToJSON();
  fwrite(json.c_str(), json.length(), 1, f);
  fclose(f);
  return true;
}

bool DFGBindingList::restoreFromFile(FTL::CStrRef filePath)
{
  FILE * f = fopen(filePath.c_str(), "rb");
  if(!f)
    return false;

  fseek( f, 0, SEEK_END );
  int fileSize = ftell( f );
  rewind( f );

  char * buffer = (char*) malloc(fileSize + 1);
  buffer[fileSize] = '\0';

  fread(buffer, fileSize, 1, f);
  fclose(f);

  std::string json = buffer;
  free(buffer);

  restoreFromJSON(json.c_str());
  return true;
}

std::string DFGBindingList::getUniqueName(FTL::CStrRef name)
{
  std::map<std::string, std::string> names;
  for(unsigned int i=0;i<getCount();i++)
    names.insert(std::pair<std::string, std::string>(getBindingTitle(i), ""));

  std::string unique = name;
  char suffix[16];
  int suffixId = 2;

  while(names.find(unique) != names.end())
  {
    unique = name;
    sprintf(suffix, "%d", suffixId++);
    unique += suffix;
  }

  return unique;
}

void DFGBindingList::setRTVal(const char * name, const FabricCore::RTVal & value, bool onlyOnce)
{
  try
  {
    for(unsigned int i=0;i<getCount();i++)
    {
      BindingInfo & info = m_bindings[i];

      std::map<std::string, bool>::iterator it = info.containsPort.find(name);
      if(it != info.containsPort.end())
      {
        if(!it->second)
          continue;
      }
      else
      {
        FabricCore::DFGExec exec = info.binding.getExec();
        bool exists = exec.haveExecPort(name);
        info.containsPort.insert(std::pair<std::string,bool>(name, exists));
        if(!exists)
          continue;
      }

      if(onlyOnce)
      {
        if(info.rtValsSet.find(name) != info.rtValsSet.end())
          continue;
        info.rtValsSet.insert(std::pair<std::string, bool>(name, true));
      }

      info.binding.setArgValue(name, value);
      info.dirty = true;

      // printf("set RTVal %s on %d\n", name, (int)i);
    }
  }
  catch(FabricCore::Exception e)
  {
    printf("FabricCore: %s\n", e.getDesc_cstr());
  }
}

bool DFGBindingList::execute()
{
  bool result = false;
  try
  {
    // todo: we should allow to move values from one binding to the next
    // eventually this should also be done by a lookup table
    for(unsigned int i=0;i<getCount();i++)
    {
      BindingInfo & info = m_bindings[i];
      if(!info.dirty)
        continue;

      // this will not execute any binding where no input 
      // has changed...!
      info.binding.execute();
      info.dirty = false;
      result = true;
    }  
  }
  catch(FabricCore::Exception e)
  {
    printf("FabricCore: %s\n", e.getDesc_cstr());
    return false;
  }

  return result;
}

void DFGBindingList::prepBindingForExecution(int index)
{
  m_bindings[index].dirty = true;
  m_bindings[index].containsPort.clear();
}

void DFGBindingList::prepAllBindingForExecution()
{
  for(size_t i=0;i<m_bindings.size();i++)
    prepBindingForExecution(i);
}
