// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QLayout>
#include <QtGui/QDoubleValidator>
#include <QtCore/QTimer>

#include "DFGNodePropertiesDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGNodePropertiesDialog::DFGNodePropertiesDialog(QWidget * parent, DFGController * controller, const char * nodeName, const DFGConfig & dfgConfig)
: DFGBaseDialog(parent, true, dfgConfig)
, m_nodeName(nodeName)
, m_controller(controller)
{
  setWindowTitle("Node Properties");

  m_titleEdit   = new QLineEdit("", this);
  m_titleEdit->setMinimumWidth(250);
  m_toolTipEdit = new QPlainTextEdit("", this);
  m_docUrlEdit  = new QLineEdit("", this);

  m_nodeColor   = new ValueEditor::ColorPickerWidget(this);
  m_textColor   = new ValueEditor::ColorPickerWidget(this);
  setColorFromExec(m_nodeColor,   "uiNodeColor",   dfgConfig.graphConfig.nodeDefaultColor);
  setColorFromExec(m_textColor,   "uiTextColor",   dfgConfig.graphConfig.nodeFontColor);
  m_nodeDefaultHeaderColor = dfgConfig.graphConfig.nodeDefaultLabelColor;

  // [Julien] FE-5246 
  // Header color property management
  // Create a checbox that creates the header color property when cliked
  m_allowHeaderColor = new QCheckBox("", this);
  FTL::CStrRef metadata = m_controller->getExec().getNodeMetadata(m_nodeName.c_str(), "uiHeaderColor");
  // If the "uiHeaderColor" metadata already exists, diplays the  header color property
  m_allowHeaderColor->setChecked(!metadata.empty());
  m_allowHeaderColor->setDisabled(!metadata.empty()); 
  QObject::connect(m_allowHeaderColor, SIGNAL(released()), this, SLOT( createHeaderColor() ) );

  try
  {
    FabricCore::DFGExec     exec = m_controller->getExec();
    FabricCore::DFGExec     subExec;
    FabricCore::DFGNodeType nodeType = exec.getNodeType(m_nodeName.c_str());

    if(nodeType == FabricCore::DFGNodeType_Inst)
    {
      subExec = exec.getSubExec(m_nodeName.c_str());
      setTitle(exec.getInstTitle(m_nodeName.c_str()));
    }
    else
    {
      FTL::CStrRef uiTitle = exec.getNodeMetadata(m_nodeName.c_str(), "uiTitle");
      if (uiTitle.empty())  setTitle(m_nodeName.c_str());
      else                  setTitle(uiTitle.c_str());
    }

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

  addInput(m_titleEdit,         "title",            "properties");
  addInput(m_toolTipEdit,       "tooltip",          "properties");
  addInput(m_docUrlEdit,        "doc url",          "properties");
  addInput(m_nodeColor,         "node color",       "properties");
  addInput(m_textColor,         "text color",       "properties");
  addInput(m_allowHeaderColor,  "set header color", "properties");
  
  // Create the header color property
  createHeaderColor();
}

DFGNodePropertiesDialog::~DFGNodePropertiesDialog()
{
}

void DFGNodePropertiesDialog::setTitle(QString value)
{
  m_titleEdit->setText(value);
}

void DFGNodePropertiesDialog::setToolTip(QString value)
{
  m_toolTipEdit->document()->setPlainText(value);
}

void DFGNodePropertiesDialog::setDocUrl(QString value)
{
  m_docUrlEdit->setText(value);
}

void DFGNodePropertiesDialog::showEvent(QShowEvent * event)
{
  QTimer::singleShot(0, m_titleEdit, SLOT(setFocus()));
  DFGBaseDialog::showEvent(event);  
}

// Allows only alpha-numeric text (here the title) only 
void DFGNodePropertiesDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("^[a-zA-Z0-9]*$*"));
}

// Filters the QLineEdit text (here the title) with the regexFilter
void DFGNodePropertiesDialog::setRegexFilter(QString regexFilter) {
  if(m_titleEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_titleEdit->setValidator(validator);
  }
}

QString DFGNodePropertiesDialog::getTitle()
{
  return m_titleEdit->text();
}

QString DFGNodePropertiesDialog::getToolTip()
{
  return m_toolTipEdit->toPlainText();
}

QString DFGNodePropertiesDialog::getDocUrl()
{
  return m_docUrlEdit->text();
}

QColor DFGNodePropertiesDialog::getNodeColor()
{
  ValueEditor::ColorPickerWidget *cpw = m_nodeColor;
  return QColor(cpw->getR_as8bit(),
                cpw->getG_as8bit(),
                cpw->getB_as8bit(),
                cpw->getA_as8bit());
}

QColor DFGNodePropertiesDialog::getHeaderColor()
{
  // [Julien] FE-5246  
  if( m_allowHeaderColor->isChecked() )
  {
    ValueEditor::ColorPickerWidget *cpw = m_headerColor;
    return QColor(cpw->getR_as8bit(),
                  cpw->getG_as8bit(),
                  cpw->getB_as8bit(),
                  cpw->getA_as8bit());
  }
  return m_nodeDefaultHeaderColor;
}

QColor DFGNodePropertiesDialog::getTextColor()
{
  ValueEditor::ColorPickerWidget *cpw = m_textColor;
  return QColor(cpw->getR_as8bit(),
                cpw->getG_as8bit(),
                cpw->getB_as8bit(),
                cpw->getA_as8bit());
}

// Creates the node header color property
void DFGNodePropertiesDialog::createHeaderColor() {
  // [Julien] FE-5246 
  // Custom header colors can have contrast mistmatches with the body's color
  // Thus, the option is disable by default 
  try
  {
    FTL::CStrRef metadata = m_controller->getExec().getNodeMetadata(m_nodeName.c_str(), "uiHeaderColor");

    if( m_allowHeaderColor->isChecked() )
    {
      m_headerColor = new ValueEditor::ColorPickerWidget(this);
      setColorFromExec(m_headerColor, "uiHeaderColor", m_nodeDefaultHeaderColor);
      addInput(m_headerColor, "header color", "properties");
      m_allowHeaderColor->setChecked(true);
      m_allowHeaderColor->setDisabled(true); 
    }
  }

  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}

void DFGNodePropertiesDialog::setColorFromExec(ValueEditor::ColorPickerWidget * widget, const char * key, QColor defaultCol)
{
  widget->set(defaultCol.redF(), defaultCol.greenF(), defaultCol.blueF(), defaultCol.alphaF());

  try
  {
    FabricCore::DFGExec exec = m_controller->getExec();

    FTL::CStrRef metadata = exec.getNodeMetadata(m_nodeName.c_str(), key);
    if(metadata.empty() && exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
    {
      FabricCore::DFGExec subExec = exec.getSubExec(m_nodeName.c_str());
      metadata = subExec.getMetadata(key);
    }
    if(metadata.empty())
      return;

    FTL::JSONStrWithLoc jsonStrWithLoc( metadata.c_str() );
    FTL::OwnedPtr<FTL::JSONValue const> jsonValue(
      FTL::JSONValue::Decode( jsonStrWithLoc )
      );
    if ( jsonValue )
    {
      FTL::JSONObject const *jsonObject = jsonValue->cast<FTL::JSONObject>();
      float r = float(jsonObject->getFloat64OrDefault( FTL_STR("r"), 0.0 )) / 255.0f;
      float g = float(jsonObject->getFloat64OrDefault( FTL_STR("g"), 0.0 )) / 255.0f;
      float b = float(jsonObject->getFloat64OrDefault( FTL_STR("b"), 0.0 )) / 255.0f;
      float a = float(jsonObject->getFloat64OrDefault( FTL_STR("a"), 255.0 )) / 255.0f;

      widget->set(r, g, b, a);
    }
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}
