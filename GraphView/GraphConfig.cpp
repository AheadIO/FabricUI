// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "GraphConfig.h"
#include <FTL/Config.h>

using namespace FabricUI::GraphView;

GraphConfig::GraphConfig()
{
#if defined(FTL_OS_DARWIN)
  // [pzion 20160218] We can't use OpenGL drawing on OS X because you
  // can't pop up the tab-search window overtop.  We can fix this if we
  // convert the tab-search window into a QGraphicsItem, which should be 
  // quite trivial to do.
  useOpenGL = false;
#else
  useOpenGL = true;
#endif
  
  pathSep = ".";
  disconnectInputsAutomatically = true;
  middleClickDeletesConnections = true;

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
  nodeSelectedPen.setWidth(1);
  nodeErrorPen = QPen(Qt::SolidLine);
  nodeErrorPen.setColor(QColor(255, 0, 0, 255));
  nodeErrorPen.setWidth(1);
  nodeContentMargins = 2.0f;
  nodeWidthReduction = 15.0f;
  nodeHeaderContentMargins = 4.0f;
  nodeHeaderSpacing = 5.0f;
  nodeHeaderAlwaysShowPins = true;
  nodeCornerRadius = 5.0f;
  nodeSpaceAbovePorts = 4.0f;
  nodeSpaceBelowPorts = 4.0f;
  nodePinSpacing = 8.0f;
  nodePinStretch = 16.0f;
  nodeShadowEnabled = true;
  nodeShadowColor = QColor(0, 0, 0, 75);
  nodeShadowOffset = QPointF(2.5, 2.5);
  nodeShadowBlurRadius = 10.0;

  nodeHeaderButtonSeparator = 2.0f;
  nodeHeaderButtonIconDir = "${FABRIC_DIR}/Resources/Icons/";

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

  dimConnectionLines = true;
  connectionUsePinColor = true;
  connectionNodePortColorRatio = 0.75;
  connectionColor = QColor(130, 130, 130);
  connectionDefaultPen = QPen(connectionColor, 1.5);
  connectionExposePen = QPen(connectionColor, 1.5);
  connectionExposePen.setStyle(Qt::DashLine);
  connectionExposePen.setDashOffset(6.0);
  connectionExposeRadius = 50.0;
  connectionHoverPen = QPen(QColor(170, 170, 170), 2.0, Qt::SolidLine);
  connectionFixedTangentLength = 10.0f;
  connectionPercentualTangentLength = 45.0f;
  portsCentered = true;

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

  headerMargins = 4;
  headerBackgroundColor = nodeDefaultLabelColor;
  headerPen = nodeDefaultPen;
  headerFont = nodeFont;
  headerFontColor = nodeFontColor;

  selectionRectColor = QColor(100, 100, 100, 50);
  selectionRectPen = QPen(QColor(25, 25, 25), 1.0, Qt::DashLine);

  mouseGrabberRadius = pinRadius * 1.75;
  mouseWheelZoomRate = 0.0f; // disable zoom for now 0.0005f;
  mouseWheelZoomRate = 0.0005f;

  backDropNodeAlpha = 0.45f;
  nodeBubbleMinWidth = 30.0;
  nodeBubbleMinHeight = 13.0;
  nodeBubbleColor = QColor(255, 247, 155);
  nodeBubbleFont = nodeFont;
  nodeBubbleFont.setBold(false);
  nodeBubbleFont.setPointSizeF(7.0);

  infoOverlayMinWidth = 100.0;
  infoOverlayMinHeight = 40.0;
  infoOverlayColor = QColor(178, 224, 253);
  infoOverlayFont = nodeFont;
  infoOverlayFont.setBold(false);
  infoOverlayFont.setPointSizeF(7.0);
}
