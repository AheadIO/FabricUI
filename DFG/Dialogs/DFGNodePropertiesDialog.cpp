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

  m_titleEdit = new QLineEdit("", this);
  m_titleEdit->setMinimumWidth(250);
  m_toolTipEdit = new QPlainTextEdit("", this);
  m_docUrlEdit = new QLineEdit("", this);

  m_nodeColor = new ValueEditor::ColorPickerWidget(this);
  m_headerColor = new ValueEditor::ColorPickerWidget(this);
  m_textColor = new ValueEditor::ColorPickerWidget(this);

  setColorFromExec(m_nodeColor, "uiNodeColor", dfgConfig.graphConfig.nodeDefaultColor);
  setColorFromExec(m_headerColor, "uiHeaderColor", dfgConfig.graphConfig.nodeDefaultLabelColor);
  setColorFromExec(m_textColor, "uiTextColor", dfgConfig.graphConfig.nodeFontColor);

  try
  {
    FabricCore::DFGExec exec = m_controller->getExec();
    FabricCore::DFGExec subExec;
    if(exec.getNodeType(m_nodeName.c_str()) == FabricCore::DFGNodeType_Inst)
      subExec = exec.getSubExec(m_nodeName.c_str());

    setTitle(m_nodeName.c_str());
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

  addInput(m_titleEdit, "title", "properties");
  addInput(m_toolTipEdit, "tooltip", "properties");
  addInput(m_docUrlEdit, "doc url", "properties");
  addInput(m_nodeColor, "node color", "properties");
  addInput(m_headerColor, "header color", "properties");
  addInput(m_textColor, "text color", "properties");

  QObject::connect(m_titleEdit, SIGNAL(editingFinished()), this, SLOT(onTitleChanged()));
  QObject::connect(m_toolTipEdit, SIGNAL(textChanged()), this, SLOT(onToolTipChanged()));
  QObject::connect(m_docUrlEdit, SIGNAL(editingFinished()), this, SLOT(onDocUrlChanged()));
  QObject::connect(m_nodeColor, SIGNAL(colorChanged(float, float, float, float)), this, SLOT(onNodeColorChanged(float, float, float, float)));
  QObject::connect(m_headerColor, SIGNAL(colorChanged(float, float, float, float)), this, SLOT(onHeaderColorChanged(float, float, float, float)));
  QObject::connect(m_textColor, SIGNAL(colorChanged(float, float, float, float)), this, SLOT(onTextColorChanged(float, float, float, float)));
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

void DFGNodePropertiesDialog::onTitleChanged()
{
  m_controller->cmdSetNodeTitle(
    m_nodeName.c_str(),
    m_titleEdit->text().toUtf8().constData()
    );
}

void DFGNodePropertiesDialog::onToolTipChanged()
{
  m_controller->setNodeToolTip(
    m_nodeName.c_str(),
    m_toolTipEdit->toPlainText().toUtf8().constData()
    );
}

void DFGNodePropertiesDialog::onDocUrlChanged()
{
  m_controller->setNodeDocUrl(
    m_nodeName.c_str(),
    m_docUrlEdit->text().toUtf8().constData()
    );
}

void DFGNodePropertiesDialog::onNodeColorChanged(float r, float g, float b, float a)
{
  QColor col(r * 255.0f, g * 255.0f, b * 255.0f, a * 255.0f);
  m_controller->setNodeBackgroundColor(m_nodeName.c_str(), col);
}

void DFGNodePropertiesDialog::onHeaderColorChanged(float r, float g, float b, float a)
{
  QColor col(r * 255.0f, g * 255.0f, b * 255.0f, a * 255.0f);
  m_controller->setNodeHeaderColor(m_nodeName.c_str(), col);
}

void DFGNodePropertiesDialog::onTextColorChanged(float r, float g, float b, float a)
{
  QColor col(r * 255.0f, g * 255.0f, b * 255.0f, a * 255.0f);
  m_controller->setNodeTextColor(m_nodeName.c_str(), col);
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
