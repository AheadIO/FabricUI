// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef _DFGACTIONS_H_
#define _DFGACTIONS_H_
 
#include <QtGui/QKeyEvent>
#include <FabricUI/DFG/DFGHotkeys.h>

namespace FabricUI
{
  namespace DFG
  {
    #define DFG_CREATE_PRESET           "Create preset"
    #define DFG_OPEN_PRESET_DOC         "Documentation"
    #define DFG_INSPECT_PRESET          "Inspect - DoubleClick"
    #define DFG_EDIT_PRESET             "Edit - Shift+DoubleClick"
    #define DFG_EDIT_PRESET_PROPERTIES  "Properties - F2"
    #define DFG_DELETE_PRESET           "Delete - Del"
    #define DFG_SELECT_ALL_PRESET       "Select all - Ctrl+A"
    #define DFG_COPY_PRESET             "Copy - Ctrl+C"
    #define DFG_CUT_PRESET              "Cut - Ctrl+X"
    #define DFG_PASTE_PRESET            "Paste - Ctrl+V"
    #define DFG_EXPORT_GRAPH            "Export graph"
    #define DFG_IMPLODE_NODE            "Implode nodes"
    #define DFG_EXPLODE_NODE            "Explode node"
    #define DFG_NEW_GRAPH               "New empty graph"
    #define DFG_NEW_FUNCTION            "New empty function"
    #define DFG_NEW_BACKDROP            "New backdrop"
    #define DFG_NEW_VARIABLE            "New variable"
    #define DFG_READ_VARIABLE           "Read variable (Get)"
    #define DFG_WRITE_VARIABLE          "Write variable (Set)"
    #define DFG_CACHE_NODE              "Cache node"
    #define DFG_RESET_ZOOM              "Reset zoom"
    #define DFG_RELOAD_EXTENSION        "Reload extension(s)"
    #define DFG_SET_COMMENT             "Set comment"
    #define DFG_REMOVE_COMMENT          "Remove comment"
    #define DFG_CREATE_PORT             "Create port"
    #define DFG_SCROLL_UP               "Scroll up"
    #define DFG_SCROLL_DOWN             "Scroll down"
    #define DFG_MOVE_TOP                "Move top"
    #define DFG_MOVE_UP                 "Move up"
    #define DFG_MOVE_DOWN               "Move down"
    #define DFG_MOVE_BOTTOM             "Move bottom"
    #define DFG_MOVE_INPUTS_TO_END      "Move inputs to end"
    #define DFG_MOVE_OUTPUTS_TO_END     "Move outputs to end"
  };
};

#endif 
