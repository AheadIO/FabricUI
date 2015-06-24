// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <FTL/Config.h>
#include "GraphConfig.h"

using namespace FabricUI::GraphView;

GraphConfig::GraphConfig()
{
#if defined(FTL_OS_DARWIN)
  useOpenGL = false;
#else
  useOpenGL = true;
#endif
  
  pathSep = ".";
  disconnectInputsAutomatically = true;
  rightClickDeletesConnections = true;

  mainPanelBackgroundColor = QColor(90, 90, 90, 255);
  mainPanelDrawGrid = false;
  mainPanelGridSpanS = 30;
  mainPanelGridSpanL = 300;
  mainPanelGridPenS = QPen(QColor(44, 44, 44, 255), 0.5);
  mainPanelGridPenL = QPen(QColor(40, 40, 40, 255), 1.0);
  mainPanelBackGroundPanFixed = true;
  
  nodeMinWidth = 100.0f;
  nodeMinHeight = 28.0f;
  nodeDefaultColor = QColor(121, 134, 143);
  nodeDefaultLabelColor = QColor(80, 98, 110);
  nodeFont = QFont("Lucida", 9);
  nodeFont.setBold(true);
  nodeFontColor = QColor(20, 20, 20, 255);
  nodeFontHighlightColor = QColor(195, 195, 195, 255);
  nodeDefaultPen = QPen(nodeFontColor, 1.0);
  nodeDefaultPenUsesNodeColor = false;
  nodeSelectedPen = QPen(Qt::SolidLine);
  nodeSelectedPen.setColor(nodeFontHighlightColor);
  nodeSelectedPen.setWidth(1.5);
  nodeErrorPen = QPen(Qt::SolidLine);
  nodeErrorPen.setColor(QColor(255, 0, 0, 255));
  nodeErrorPen.setWidth(1.5);
  nodeContentMargins = 2.0f;
  nodeWidthReduction = 15.0f;
  nodeHeaderContentMargins = 2.0f;
  nodeHeaderSpacing = 4.0f;
  nodeHeaderSpaceTop = 3.0;
  nodeHeaderSpaceBottom = 2.0;
  nodeCornerRadius = 5.0f;
  nodeSpaceAbovePorts = 4.0f;
  nodeSpaceBelowPorts = 4.0f;
  nodePinSpacing = 8.0f;
  nodeShadowEnabled = true;
  nodeShadowColor = QColor(0, 0, 0, 75);
  nodeShadowOffset = QPointF(2.5, 2.5);
  nodeShadowBlurRadius = 10.0;

  nodeToolbarColor = QColor(180, 183, 193);
  nodeToolbarPen = nodeDefaultPen;
  nodeToolbarHeight = 18.0f;
  nodeToolbarBaseWidth = 6.0f;
  nodeToolbarSeparator = 2.0f;
  nodeToolbarIconDir = "${FABRIC_DIR}/Resources/Icons/";

  pinFont = QFont("Lucida", 9);
  pinFontColor = nodeFontColor;
  pinFontHighlightColor = nodeFontHighlightColor;
  pinDefaultPen = nodeDefaultPen;
  pinUsesColorForPen = false;
  pinHoverPen = QPen(nodeFontHighlightColor, 1.5);
  pinRadius = 5.5f;
  pinLabelSpacing = 2.0f;
  pinInputUsesFullCircle = false;
  pinOutputUsesFullCircle = true;
  pinClickableDistance = 30.0f;

  connectionUsePinColor = true;
  connectionNodePortColorRatio = 0.75;
  connectionColor = QColor(130, 130, 130);
  connectionDefaultPen = QPen(connectionColor, 1.5);
  connectionHoverPen = QPen(QColor(170, 170, 170), 2.0, Qt::SolidLine);
  connectionFixedTangentLength = 10.0f;
  connectionPercentualTangentLength = 45.0f;

  sidePanelBackgroundColor = nodeDefaultColor;
  sidePanelCollapsedWidth = 24.0f;
  sidePanelPen = QPen(Qt::NoPen);
  sidePanelContentMargins = 4.0f;
  sidePanelSpacing = 10.0f;
  sidePanelFont = QFont("Lucida", 9);
  sidePanelFontColor = pinFontColor;
  sidePanelFontHighlightColor = QColor(255, 255, 255, 255);
  sidePanelPortLabelSpacing = 4.0f;
  sidePanelWidthReduction = 10.5f;

  headerMargins = 4.0;
  headerBackgroundColor = nodeDefaultLabelColor;
  headerPen = nodeDefaultPen;
  headerFont = nodeFont;
  headerFontColor = nodeFontColor;

  selectionRectColor = QColor(100, 100, 100, 50);
  selectionRectPen = QPen(QColor(25, 25, 25), 1.0, Qt::DashLine);

  mouseGrabberRadius = pinRadius * 1.75;
  mouseWheelZoomRate = 0.0f; // disable zoom for now 0.0005f;
  mouseWheelZoomRate = 0.0005f;

  backDropNodeAlpha = 0.45;
}
