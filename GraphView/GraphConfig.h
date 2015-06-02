// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_GraphConfig__
#define __UI_GraphView_GraphConfig__

#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QFont>

namespace FabricUI
{

  namespace GraphView
  {

    struct GraphConfig
    {
      bool useOpenGL;
      
      char const *pathSep;
      bool disconnectInputsAutomatically;
      bool rightClickDeletesConnections;

      float headerMargins;
      QColor headerBackgroundColor;
      QPen headerPen;
      QFont headerFont;
      QColor headerFontColor;

      QColor mainPanelBackgroundColor;
      bool mainPanelDrawGrid;
      int mainPanelGridSpanS;
      int mainPanelGridSpanL;
      QPen mainPanelGridPenS;
      QPen mainPanelGridPenL;
      bool mainPanelBackGroundPanFixed;

      QColor sidePanelBackgroundColor;
      float sidePanelCollapsedWidth;
      QPen sidePanelPen;
      float sidePanelContentMargins;
      float sidePanelSpacing;
      QFont sidePanelFont;
      QColor sidePanelFontColor;
      QColor sidePanelFontHighlightColor;
      float sidePanelPortLabelSpacing;
      float sidePanelWidthReduction;
      
      float nodeMinWidth;
      float nodeMinHeight;
      QColor nodeDefaultColor;
      QColor nodeDefaultLabelColor;
      QPen nodeDefaultPen;
      bool nodeDefaultPenUsesNodeColor;
      QPen nodeSelectedPen;
      QPen nodeErrorPen;
      QFont nodeFont;
      QColor nodeFontColor;
      QColor nodeFontHighlightColor;
      float nodeContentMargins;
      float nodeWidthReduction;
      float nodeHeaderContentMargins;
      float nodeHeaderSpacing;
      float nodeHeaderSpaceTop;
      float nodeHeaderSpaceBottom;
      float nodeCornerRadius;
      float nodeSpaceAbovePorts;
      float nodeSpaceBelowPorts;
      float nodePinSpacing;
      bool nodeShadowEnabled;
      QColor nodeShadowColor;
      QPointF nodeShadowOffset;
      float nodeShadowBlurRadius;

      QColor nodeToolbarColor;
      QPen nodeToolbarPen;
      float nodeToolbarHeight;
      float nodeToolbarBaseWidth;
      float nodeToolbarSeparator;
      QString nodeToolbarIconDir;

      QPen pinDefaultPen;
      bool pinUsesColorForPen;
      QPen pinHoverPen;
      QFont pinFont;
      QColor pinFontColor;
      QColor pinFontHighlightColor;
      float pinRadius;
      float pinLabelSpacing;
      bool pinInputUsesFullCircle;
      bool pinOutputUsesFullCircle;
      float pinClickableDistance;

      QPen connectionDefaultPen;
      QPen connectionHoverPen;
      float connectionFixedTangentLength;
      float connectionPercentualTangentLength;
      QColor connectionColor;
      bool connectionUsePinColor;
      float connectionNodePortColorRatio;

      QColor selectionRectColor;
      QPen selectionRectPen;
      float mouseGrabberRadius;
      float mouseWheelZoomRate;

      GraphConfig();
    };

  };

};

#endif // __UI_GraphView_GraphConfig__
