#ifndef __MACROS_H__
#define __MACROS_H__

#include <FabricCore.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>


template <typename T> std::string ToStr(T t) { 
  std::ostringstream os; 
  os<<t; 
  return os.str(); 
} 

template<typename T> T ToNum(std::string number) {
  T value;
  std::stringstream stream(number);
  stream >> value;
  if(stream.fail()) 
  {
    std::runtime_error e(number);
    throw e;
  }
  return value;
}

/// \internal
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


#ifdef FABRIC_SPLICE_TRACE

#define FABRIC_TRY(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return; \
  }

#define FABRIC_TRY_RETURN(func, returnVal, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return returnVal; \
  }

#define FABRIC_TRY_IGNORE(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
  }

#else

#define FABRIC_TRY(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return; \
  }

#define FABRIC_TRY_RETURN(func, returnVal, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
    printf("[%s] %s\n", func, e.getDesc_cstr()); \
    return returnVal; \
  }

#define FABRIC_TRY_IGNORE(func, code) \
  try \
  { \
    code \
  } \
  catch(FabricCore::Exception e) \
  { \
  }

#endif
#endif
