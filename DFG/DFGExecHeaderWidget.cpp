// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <FabricUI/DFG/DFGController.h>
#include <FabricUI/DFG/DFGExecHeaderWidget.h>

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGExecHeaderWidget::DFGExecHeaderWidget(
  QWidget * parent,
  DFGController *dfgController,
  QString caption,
  const GraphView::GraphConfig &config
  )
  : QWidget( parent )
  , m_dfgController( dfgController )
  , m_caption( caption )
  , m_config( config )
{
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
  setContentsMargins(0, 0, 0, 0);

  QHBoxLayout * layout = new QHBoxLayout();
  layout->setContentsMargins(config.headerMargins, config.headerMargins, config.headerMargins, config.headerMargins);

  m_backgroundColor = config.headerBackgroundColor;
  m_pen = config.headerPen;

  m_goUpButton = new QPushButton("Go up", this);
  QObject::connect(m_goUpButton, SIGNAL(clicked()), this, SIGNAL(goUpPressed()));
  m_goUpButton->setAutoFillBackground(false);

  m_reqExtLineEdit = new QLineEdit;
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(returnPressed()),
    this, SLOT(reqExtReturnPressed())
    );
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(editingFinished()),
    this, SLOT(reqExtEditingFinished())
    );

  layout->addWidget( new QLabel("Required Extensions:") );
  layout->addWidget( m_reqExtLineEdit );
  layout->addStretch(1);
  m_captionLabel = new QLabel();
  layout->addWidget( m_captionLabel );
  layout->addStretch(1);
  layout->addWidget(m_goUpButton);
  layout->setAlignment(m_goUpButton, Qt::AlignHCenter | Qt::AlignVCenter);

  QWidget *regWidget = new QWidget;
  regWidget->setLayout( layout );

  QHBoxLayout *presetSplitLayout = new QHBoxLayout;
  presetSplitLayout->setContentsMargins(config.headerMargins, config.headerMargins, config.headerMargins, config.headerMargins);
  QPushButton *presetSplitButton = new QPushButton( "Split from Preset" );
  connect(
    presetSplitButton, SIGNAL(clicked()),
    this, SLOT(onSplitFromPresetClicked())
    );
  presetSplitLayout->addWidget( presetSplitButton );
  presetSplitLayout->addWidget( new QLabel( "\
This node or one of its parents is an instance of a preset\n\
and cannot be changed unless split from the preset" ) );
  presetSplitLayout->addStretch(1);
  m_presetSplitWidget = new QWidget;
  m_presetSplitWidget->setLayout( presetSplitLayout );
  QPalette presetSplitPalette( palette() );
  presetSplitPalette.setColor( QPalette::Background, QColor("#ccb455") );
  presetSplitPalette.setColor( QPalette::Foreground, QColor("#000000") );
  m_presetSplitWidget->setPalette( presetSplitPalette );
  m_presetSplitWidget->setAutoFillBackground( true );

  QVBoxLayout *vLayout = new QVBoxLayout;
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->addWidget( m_presetSplitWidget );
  vLayout->addWidget( regWidget );
  setLayout(vLayout);

  setFont( config.headerFont );
  setFontColor( config.headerFontColor );

  QObject::connect(
    m_dfgController, SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
  QObject::connect(
    m_dfgController, SIGNAL(execSplitChanged()),
    this, SLOT(onExecChanged())
    );
  onExecChanged();
}

DFGExecHeaderWidget::~DFGExecHeaderWidget()
{
}

void DFGExecHeaderWidget::refresh()
{
  m_caption = m_dfgController->getExecPath().c_str();
  m_captionLabel->setText( m_caption );
  FabricCore::String extDepsDesc = getExec().getExtDeps();
  char const *extDepsDescCStr = extDepsDesc.getCStr();
  m_reqExtLineEdit->setText( extDepsDescCStr? extDepsDescCStr: "" );
  update();
}

void DFGExecHeaderWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  m_reqExtLineEdit->setText( extDeps.c_str() );
  update();
}

void DFGExecHeaderWidget::reqExtReturnPressed()
{
  m_reqExtLineEdit->clearFocus();
}

void DFGExecHeaderWidget::reqExtEditingFinished()
{
  std::string extDepDesc = m_reqExtLineEdit->text().toUtf8().constData();
  FabricCore::String currentExtDepDesc = getExec().getExtDeps();
  char const *currentExtDepDescCStr = currentExtDepDesc.getCStr();
  if ( !currentExtDepDescCStr )
    currentExtDepDescCStr = "";
  if ( extDepDesc == currentExtDepDescCStr )
    return;

  FTL::StrRef::Split split = FTL::StrRef( extDepDesc ).split(',');
  std::vector<std::string> nameAndVerStrings;
  while ( !split.first.empty() )
  {
    nameAndVerStrings.push_back( split.first.trim() );
    split = split.second.split(',');
  }
  if ( nameAndVerStrings.empty() )
    return;

  std::vector<FTL::StrRef> nameAndVerStrs;
  nameAndVerStrs.reserve( nameAndVerStrings.size() );
  for ( size_t i = 0; i < nameAndVerStrings.size(); ++i )
    nameAndVerStrs.push_back( nameAndVerStrings[i] );

  m_dfgController->cmdSetExtDeps( nameAndVerStrs );
}

QString DFGExecHeaderWidget::caption() const
{
  return m_caption;
}

QString DFGExecHeaderWidget::captionSuffix() const
{
  return m_captionSuffix;
}

QFont DFGExecHeaderWidget::font() const
{
  return m_font;
}

QColor DFGExecHeaderWidget::fontColor() const
{
  return m_fontColor;
}

bool DFGExecHeaderWidget::italic() const
{
  return m_font.italic();
}

void DFGExecHeaderWidget::setFont(QFont f)
{
  m_font = f;
  m_captionLabel->setFont( f );
  m_goUpButton->setFont(f);
  update();
}

void DFGExecHeaderWidget::setFontColor(QColor c)
{
  m_fontColor = c;

  QPalette captionLabelPalette = palette();
  captionLabelPalette.setColor( QPalette::Foreground, c );
  m_captionLabel->setPalette( captionLabelPalette );

  QPalette p = m_goUpButton->palette();
  p.setColor(QPalette::ButtonText, c);
  p.setColor(QPalette::Button, m_config.nodeDefaultColor);
  m_goUpButton->setPalette(p);

  update();
}

void DFGExecHeaderWidget::onExecChanged()
{
  FabricCore::DFGExec &exec = m_dfgController->getExec();
  if ( !!exec )
    m_presetSplitWidget->setVisible( exec.editWouldSplitFromPreset() );

  if(m_dfgController->isViewingRootGraph())
  {
    m_goUpButton->hide();
  }
  else
  {
    m_goUpButton->show();
  }

  m_caption = m_dfgController->getExecPath().c_str();
  if(exec.isValid() && m_caption.length() > 0)
  {
    FTL::StrRef presetName = exec.getPresetName();
    if(presetName.empty())
      m_captionSuffix = " *";
    else
      m_captionSuffix = "";
  }
  else
    m_captionSuffix = "";

  m_caption += m_captionSuffix;
  m_captionLabel->setText( m_caption );
  m_font.setItalic(m_captionSuffix.length() > 0);
  m_captionLabel->setFont( m_font );
  update();
}

void DFGExecHeaderWidget::paintEvent(QPaintEvent * event)
{
  QRect rect = contentsRect();
  QPainter painter(this);

  painter.fillRect(rect, m_backgroundColor);

  painter.setPen(m_pen);
  painter.drawLine(rect.bottomLeft(), rect.bottomRight());

  QWidget::paintEvent(event);
}

FabricCore::DFGExec &DFGExecHeaderWidget::getExec()
{
  return m_dfgController->getExec();
}

void DFGExecHeaderWidget::onSplitFromPresetClicked()
{
  m_dfgController->cmdSplitFromPreset();
}
