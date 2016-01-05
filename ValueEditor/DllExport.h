#pragma once

#if defined(VALUEEDITOR_BUILD)
#  define VALUEEDIT_API __declspec(dllexport)
#else
#  define VALUEEDIT_API __declspec(dllimport)
#endif