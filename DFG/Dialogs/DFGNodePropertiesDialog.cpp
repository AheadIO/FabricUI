// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <iostream>
#include <QtGui/QLayout>
#include <QtGui/QDoubleValidator>
#include <QtCore/QTimer>

#include "DFGNodePropertiesDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

/// Constructor
DFGNodePropertiesDialog::DFGNodePropertiesDialog(
  QWidget * parent, 
  DFGController * controller, 
  const char * nodeName, 
  const DFGConfig & dfgConfig
  )
: DFGBaseDialog(parent, true, dfgConfig)
, m_nodeName(nodeName)
, m_controller(controller)
{
  setWindowTitle("Node Properties");

  DFGExec &exec = controller.getExec();
  DFGExec subExec = exec.getSubExec( nodeName );
  m_isPreset = subExec.editWouldSplitFromPreset();

  if ( m_isPreset )
    m_titleWidget = new QLabel( "", this );
  else
    m_titleWidget = new QLineEdit( "", this );
  m_nameLabel = new QLabel( "", this );
  if ( m_isPreset )
    m_toolTipWidget = new QLabel( "", this );
  else
    m_toolTipWidget = new QPlainTextEdit( "", this );
  if ( m_isPreset )
    m_docUrlEdit = new QLabel( "", this );
  else
    m_docUrlEdit  = new QLineEdit("", this);
  m_nodeColorPushButton = new QPushButton( this );
  m_nodeColorPushButton->setWidth( 60 ); 
  setColorFromExec(
    m_nodeColorPushButton,
    "uiNodeColor",
    dfgConfig.graphConfig.nodeDefaultColor
    );
  m_textColorPushButton = new QPushButton( this );
  m_textColorPushButton->setWidth( 60 ); 
  setColorFromExec(
    m_textColorPushButton,
    "uiTextColor",
    dfgConfig.graphConfig.nodeFontColor
    );

  // [Julien] FE-5246 
  // Header color property management
  // Create a checbox that creates the header color property when cliked
  m_headerColor = 0;
  m_nodeDefaultHeaderColor = dfgConfig.graphConfig.nodeDefaultLabelColor;
  m_allowHeaderColor = new QCheckBox("", this);
  m_allowHeaderColor->setEnabled( !m_isPreset );
  // If the "uiHeaderColor" metadata already exists, diplays the  header color property
  FTL::CStrRef metadata = m_controller->getExec().getNodeMetadata(m_nodeName.c_str(), "uiHeaderColor");
  m_allowHeaderColor->setChecked(!metadata.empty());
  QObject::connect(
    m_allowHeaderColor, SIGNAL(clicked()),
    this, SLOT(onAllowHeaderColorCheckBoxClicked())
    );

  try
  {
    FabricCore::DFGExec     exec = m_controller->getExec();
    FabricCore::DFGExec     subExec;
    FabricCore::DFGNodeType nodeType = exec.getNodeType(m_nodeName.c_str());

    if(nodeType == FabricCore::DFGNodeType_Inst)
    {
      subExec = exec.getSubExec( m_nodeName.c_str() );
      setTitle( subExec.getTitle() );
    }
    else
    {
      FTL::CStrRef uiTitle = exec.getNodeMetadata(m_nodeName.c_str(), "uiTitle");
      if (uiTitle.empty())  setTitle(m_nodeName.c_str());
      else                  setTitle(uiTitle.c_str());
    }

    m_nameEdit->setText( m_nodeName.c_str() );

    FTL::CStrRef uiTooltip = exec.getNodeMetadata(m_nodeName.c_str(), "uiTooltip");
    if(uiTooltip.empty() && subExec.isValid())
      uiTooltip = subExec.getMetadata("uiTooltip");
    if(!uiTooltip.empty())
      setToolTip(uiTooltip.c_str());

    FTL::CStrRef uiDocUrl = exec.getNodeMetadata(m_nodeName.c_str(), "uiDocUrl");
    if(uiDocUrl.empty() && subExec.isValid())
      uiDocUrl = subExec.getMetadata("uiDocUrl");
    if(!uiDocUrl.empty())
      setDocUrl(uiDocUrl.c_str());
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }

  addInput( m_titleWidget, "title", "properties" );
  addInput( m_nameLabel, "script name", "properties" );
  addInput( m_toolTipWidget, "tooltip", "properties" );
  addInput( m_docUrlWidget, "doc url", "properties" );
  addInput( m_nodeColorPushButton, "node color", "properties" );
  addInput( m_textColorPushButton, "text color", "properties" );
  addInput( m_allowHeaderColorCheckBox, "custom header color", "properties" );
    
  // Create pr remove the header color property
  onAllowHeaderColorCheckBoxClicked( m_allowHeaderColor->checked() );
}

/// Destructor
DFGNodePropertiesDialog::~DFGNodePropertiesDialog()
{
}

/// Sets the node's title
void DFGNodePropertiesDialog::setTitle(QString value)
{
  m_titleLabel->setText(value);
}

/// Sets the node's tool tip
void DFGNodePropertiesDialog::setToolTip(QString value)
{
  m_toolTipEdit->document()->setPlainText(value);
}

/// Sets the node's documentation url 
void DFGNodePropertiesDialog::setDocUrl(QString value)
{
  m_docUrlEdit->setText(value);
}

/// Shows this dialog widgets
void DFGNodePropertiesDialog::showEvent(QShowEvent * event)
{
  QTimer::singleShot(0, m_nameEdit, SLOT(setFocus()));
  DFGBaseDialog::showEvent(event);  
}

/// Gets the user selected node's body color 
QColor DFGNodePropertiesDialog::getNodeColor()
{
  ValueEditor::ColorPickerWidget *cpw = m_nodeColor;
  return QColor(cpw->getR_as8bit(),
                cpw->getG_as8bit(),
                cpw->getB_as8bit(),
                cpw->getA_as8bit());
}

/// Gets the user selected node's header color 
bool DFGNodePropertiesDialog::getHeaderColor(QColor &color)
{
  // [Julien] FE-5246 
  if( (m_headerColor != 0) && m_allowHeaderColor->isChecked() )
  {
    ValueEditor::ColorPickerWidget *cpw = m_headerColor;
    color = QColor(cpw->getR_as8bit(),
                  cpw->getG_as8bit(),
                  cpw->getB_as8bit(),
                  cpw->getA_as8bit());
    return true;
  }
  else 
  {
    color = getColorFromExec("uiHeaderColor", m_nodeDefaultHeaderColor);
    color.setAlphaF(0.0);
    return false;
  }
}

/// Gets the user selected node's text color 
QColor DFGNodePropertiesDialog::getTextColor()
{
  ValueEditor::ColorPickerWidget *cpw = m_textColor;
  return QColor(cpw->getR_as8bit(),
                cpw->getG_as8bit(),
                cpw->getB_as8bit(),
                cpw->getA_as8bit());
}

/// Creates the node header color property
void DFGNodePropertiesDialog::onAllowHeaderColorCheckBoxClicked() {
  // [Julien] FE-5246 
  // Custom header colors can have contrast mistmatches with the body's color
  // Thus, the option is disable by default 
  try
  {
    if( (m_headerColor == 0) && m_allowHeaderColor->isChecked())
    {
      m_headerColor = new ValueEditor::ColorPickerWidget(this);
      setColorFromExec(m_headerColor, "uiHeaderColor", m_nodeDefaultHeaderColor);
      addInput(m_headerColor, "header color", "properties");
    }
    else if( (m_headerColor != 0) && !m_allowHeaderColor->isChecked())
    {
      removeSection(m_headerColor); 
      m_headerColor = 0;
    }
  }

  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}

/// \internal
/// Gets the color property header color metadata
QColor DFGNodePropertiesDialog::getColorFromExec(const char * key, QColor defaultCol)
{
  QColor color = defaultCol;

  try
  {
    FabricCore::DFGExec exec = m_controller->getExec();

    FTL::CStrRef metadata = exec.getNodeMetadata(m_nodeName.c_str(), key);
    if(metadata.empty() && exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
    {
      FabricCore::DFGExec subExec = exec.getSubExec(m_nodeName.c_str());
      metadata = subExec.getMetadata(key);
    }
    if(!metadata.empty()) 
    {
      FTL::JSONStrWithLoc jsonStrWithLoc( metadata.c_str() );
      FTL::OwnedPtr<FTL::JSONValue const> jsonValue(FTL::JSONValue::Decode( jsonStrWithLoc ) );
      if ( jsonValue )
      {
        FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
        color.setRedF(float(jsonObject->getFloat64OrDefault( FTL_STR("r"), 0.0 )) / 255.0f);
        color.setGreenF(float(jsonObject->getFloat64OrDefault( FTL_STR("g"), 0.0 )) / 255.0f);
        color.setBlueF(float(jsonObject->getFloat64OrDefault( FTL_STR("b"), 0.0 )) / 255.0f);
        color.setAlphaF(float(jsonObject->getFloat64OrDefault( FTL_STR("a"), 255.0 )) / 255.0f);
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }

  return color;
}

/// \internal
/// Sets the color property of the ColorPickerWidget widget
void DFGNodePropertiesDialog::setColorFromExec(ValueEditor::ColorPickerWidget * widget, const char * key, QColor defaultCol)
{
  QColor color = getColorFromExec(key, defaultCol);
  widget->set(color.redF(), color.greenF(), color.blueF(), color.alphaF());  
}
