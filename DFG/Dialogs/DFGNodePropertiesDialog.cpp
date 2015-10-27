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
  const DFGConfig & dfgConfig,
  bool setAlphaNum )
: DFGBaseDialog(parent, true, dfgConfig)
, m_nodeName(nodeName)
, m_controller(controller)
{
  setWindowTitle("Node Properties");

  m_presetNameLabel = 0;
  m_textEdit = 0;
  m_nameEdit    = new QLineEdit("", this);
  m_nameEdit->setMinimumWidth(250);
  m_toolTipEdit = new QPlainTextEdit("", this);
  m_docUrlEdit  = new QLineEdit("", this);

  m_nodeColorButton = new ColorButton(
    getColorFromExec(
      FTL_STR("uiNodeColor"),
      dfgConfig.graphConfig.nodeDefaultColor
      ),
    this
    );
  connect(
    m_nodeColorButton, SIGNAL(clicked()),
    this, SLOT(onNodeColorButtonClicked())
    );

  m_textColorButton = new ColorButton(
    getColorFromExec(
      FTL_STR("uiTextColor"),
      dfgConfig.graphConfig.nodeFontColor
      ),
    this
    );
  connect(
    m_textColorButton, SIGNAL(clicked()),
    this, SLOT(onTextColorButtonClicked())
    );

  // [Julien] FE-5246 
  // Header color property management
  // Create a checbox that creates the header color property when cliked
  m_headerColorButton = 0;
  m_nodeDefaultHeaderColor = dfgConfig.graphConfig.nodeDefaultLabelColor;
  m_allowHeaderColorCheckBox = new QCheckBox("", this);
  // If the "uiHeaderColor" metadata already exists, diplays the  header color property
  FTL::CStrRef metadata = m_controller->getExec().getNodeMetadata(m_nodeName.c_str(), "uiHeaderColor");
  m_allowHeaderColorCheckBox->setChecked(!metadata.empty());
  QObject::connect(
    m_allowHeaderColorCheckBox, SIGNAL(clicked()),
    this, SLOT(onAllowHeaderColorCheckBoxClicked())
    );

  try
  {
    FabricCore::DFGExec     exec = m_controller->getExec();
    FabricCore::DFGExec     subExec;
    FabricCore::DFGNodeType nodeType = exec.getNodeType(m_nodeName.c_str());

    if ( nodeType == FabricCore::DFGNodeType_Inst )
    {
      subExec = exec.getSubExec( m_nodeName.c_str() );
      if ( subExec.isPreset() )
        m_presetNameLabel = new QLabel( subExec.getTitle(), this );
    }
    else if ( nodeType == FabricCore::DFGNodeType_User )
    {
      FTL::CStrRef uiTitle =
        exec.getNodeMetadata( m_nodeName.c_str(), "uiTitle" );
      m_textEdit = new QLineEdit( uiTitle.c_str(), this );
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

  if ( m_presetNameLabel )
    addInput( m_presetNameLabel, "preset name", "properties" );
  if ( m_textEdit )
    addInput( m_textEdit, "text", "properties" );
  addInput( m_nameEdit, "node name", "properties" );
  addInput(m_toolTipEdit,       "tooltip",              "properties");
  addInput(m_docUrlEdit,        "doc url",              "properties");
  addInput( m_nodeColorButton, "node color", "properties" );
  addInput( m_textColorButton, "text color", "properties" );
  addInput( m_allowHeaderColorCheckBox, "custom header color", "properties" );
    
  // [Julien] FE-5188, FE-5276
  if(setAlphaNum) alphaNumicStringOnly();
    
  // Create pr remove the header color property
  onAllowHeaderColorCheckBoxClicked();
}

/// Destructor
DFGNodePropertiesDialog::~DFGNodePropertiesDialog()
{
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

/// Allows only alpha-numeric text (here the title) only 
void DFGNodePropertiesDialog::alphaNumicStringOnly() {
  setRegexFilter(QString("[a-zA-Z][_a-zA-Z0-9]*"));
}

/// Filters the QLineEdit text (here the title) with the regexFilter
void DFGNodePropertiesDialog::setRegexFilter(QString regexFilter) {
  if(m_nameEdit)
  {
    QRegExp regex(regexFilter);
    QValidator *validator = new QRegExpValidator(regex, 0);
    m_nameEdit->setValidator(validator);
  }
}

/// Gets the user selected node's title
QString DFGNodePropertiesDialog::getScriptName()
{
  return m_nameEdit->text();
}

/// Gets the user selected node's tool tip 
QString DFGNodePropertiesDialog::getToolTip()
{
  return m_toolTipEdit->toPlainText();
}

/// Gets the user selected node's url doc 
QString DFGNodePropertiesDialog::getDocUrl()
{
  return m_docUrlEdit->text();
}

/// Gets the user selected node's body color 
QColor DFGNodePropertiesDialog::getNodeColor()
{
  return m_nodeColorButton->color();
}

/// Gets the user selected node's header color 
bool DFGNodePropertiesDialog::getHeaderColor(QColor &color)
{
  // [Julien] FE-5246 
  if ( !!m_headerColorButton
    && m_allowHeaderColorCheckBox->isChecked() )
  {
    color = m_headerColorButton->color();
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
  return m_textColorButton->color();
}

/// Creates the node header color property
void DFGNodePropertiesDialog::onAllowHeaderColorCheckBoxClicked()
{
  // [Julien] FE-5246 
  // Custom header colors can have contrast mistmatches with the body's color
  // Thus, the option is disable by default 
  try
  {
    if ( !m_headerColorButton
      && m_allowHeaderColorCheckBox->isChecked() )
    {
      m_headerColorButton = new ColorButton(
        getColorFromExec(
          FTL_STR("uiHeaderColor"),
          m_nodeDefaultHeaderColor
          ),
        this
        );
      connect(
        m_headerColorButton, SIGNAL(clicked()),
        this, SLOT(onHeaderColorButtonClicked())
        );
      addInput(m_headerColorButton, "header color", "properties");
    }
    else if ( !!m_headerColorButton
      && !m_allowHeaderColorCheckBox->isChecked() )
    {
      removeSection(m_headerColorButton);
      m_headerColorButton = 0;
    }
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }
}

/// \internal
/// Gets the color property header color metadata
QColor DFGNodePropertiesDialog::getColorFromExec(
  FTL::CStrRef key,
  QColor const &defaultCol
  )
{
  QColor color = defaultCol;

  try
  {
    FabricCore::DFGExec exec = m_controller->getExec();

    FTL::CStrRef metadata =
      exec.getNodeMetadata( m_nodeName.c_str(), key.c_str() );
    if ( metadata.empty()
      && exec.getNodeType( m_nodeName.c_str() ) == FabricCore::DFGNodeType_Inst )
    {
      FabricCore::DFGExec subExec = exec.getSubExec(m_nodeName.c_str());
      metadata = subExec.getMetadata( key.c_str() );
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

void DFGNodePropertiesDialog::onNodeColorButtonClicked()
{
  QColorDialog colorDialog( m_nodeColorButton->color(), this );
  if ( colorDialog.exec() == QDialog::Accepted )
    m_nodeColorButton->setColor( colorDialog.currentColor() );
}

void DFGNodePropertiesDialog::onHeaderColorButtonClicked()
{
  QColorDialog colorDialog( m_headerColorButton->color(), this );
  if ( colorDialog.exec() == QDialog::Accepted )
    m_headerColorButton->setColor( colorDialog.currentColor() );
}

void DFGNodePropertiesDialog::onTextColorButtonClicked()
{
  QColorDialog colorDialog( m_textColorButton->color(), this );
  if ( colorDialog.exec() == QDialog::Accepted )
    m_textColorButton->setColor( colorDialog.currentColor() );
}

void DFGNodePropertiesDialog::ColorButton::paintEvent(
  QPaintEvent *event
  )
{
  QPainter painter( this );
  painter.fillRect( event->rect(), QBrush( m_color ) );
  event->accept();
}
