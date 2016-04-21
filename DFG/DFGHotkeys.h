// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef _DFGHOTKEYS_H_
#define _DFGHOTKEYS_H_
 
#include <QtGui/QKeyEvent>

namespace FabricUI
{
  namespace DFG
  {
    class DFGHotkeys
    {
    public:
      static const char *DELETE_1;
      static const char *DELETE_2;
      static const char *EXECUTE;
      static const char *PAN_GRAPH;
      static const char *FRAME_SELECTED;
      static const char *FRAME_ALL;
      static const char *TAB_SEARCH;
      static const char *SELECT_ALL;
      static const char *DISCONNECT_ALL_PORTS;
      static const char *COPY;
      static const char *CUT;
      static const char *PASTE;
      static const char *NEW_SCENE;
      static const char *OPEN_SCENE;
      static const char *SAVE_SCENE;
      static const char *EDIT_PROPERTIES;
      static const char *RELAX_NODES;
      static const char *RESET_ZOOM;
      static const char *TOGGLE_SIDE_PANEL;
      static const char *TOGGLE_MANIPULATION;
      static const char *COLLAPSE_LEVEL_1;
      static const char *COLLAPSE_LEVEL_2;
      static const char *COLLAPSE_LEVEL_3;

#ifdef ENABLE_STYLERELOAD
      // This additional action allows users
      // to quickly reload and iterate on styles
      // in Canvas
      static const char *RELOAD_STYLES;
#endif
    };
  }
}

#endif 
