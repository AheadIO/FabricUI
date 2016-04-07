/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_FABRICUI_STRING_UTILS_H__
#define __UI_FABRICUI_STRING_UTILS_H__


#include <FabricUI/Util/macros.h>

namespace FabricUI
{
  namespace Util
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
  };
};
 
#endif // __UI_FABRICUI_STRING_UTILS_H__
