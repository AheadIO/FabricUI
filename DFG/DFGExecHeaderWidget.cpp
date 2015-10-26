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
  const GraphView::GraphConfig &config
  )
  : QWidget( parent )
  , m_dfgController( dfgController )
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

  m_execPathLabel = new QLabel;

  m_presetNameLabel = new QLabel;

  m_reqExtLabel = new QLabel;
  m_reqExtLineEdit = new QLineEdit;
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(returnPressed()),
    this, SLOT(reqExtReturnPressed())
    );
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(editingFinished()),
    this, SLOT(reqExtEditingFinished())
    );

  layout->addWidget( m_execPathLabel );
  layout->addSpacing( 4 );
  layout->addWidget( m_presetNameLabel );
  layout->addStretch( 1 );
  layout->addWidget( m_reqExtLabel );
  layout->addWidget( m_reqExtLineEdit );
  layout->addStretch( 1 );
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

  QPalette captionLabelPalette = palette();
  captionLabelPalette.setColor( QPalette::Text, QColor("#C7D2DA") );
  captionLabelPalette.setColor( QPalette::WindowText, config.headerFontColor );
  m_execPathLabel->setPalette( captionLabelPalette );
  m_presetNameLabel->setPalette( captionLabelPalette );
  m_reqExtLabel->setPalette( captionLabelPalette );
  m_reqExtLineEdit->setPalette( captionLabelPalette );

  QPalette p = m_goUpButton->palette();
  p.setColor( QPalette::ButtonText, config.headerFontColor );
  p.setColor( QPalette::Button, config.nodeDefaultColor);
  m_goUpButton->setPalette( p );

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
  FTL::CStrRef execPath = getExecPath();
  FabricCore::DFGExec &exec = getExec();
  if ( exec )
  {
    bool isRoot = execPath.empty();
    bool isPreset = exec.isPreset();
    bool wouldSplitFromPreset = exec.editWouldSplitFromPreset();

    m_presetSplitWidget->setVisible( wouldSplitFromPreset );

    m_goUpButton->setVisible( !isRoot );

    m_execPathLabel->setVisible( !isRoot );
    if ( !isRoot )
    {
      QString pathLabelText( "Path: " );
      pathLabelText += QString::fromAscii( execPath.data(), execPath.size() );
      m_execPathLabel->setText( pathLabelText );
    }

    m_presetNameLabel->setVisible( isPreset );
    if ( isPreset )
    {
      FTL::CStrRef title = exec.getTitle();
      QString presetNameText( "Preset Name: ");
      presetNameText += QString::fromAscii( title.data(), title.size() );
      m_presetNameLabel->setText( presetNameText );
    }

    FabricCore::String extDepsDesc = exec.getExtDeps();
    FTL::CStrRef extDepsDescCStr =
      extDepsDesc.getCStr()? extDepsDesc.getCStr() : "";
    QString reqExtLabelText( "Required Extensions:" );
    m_reqExtLineEdit->setVisible( !wouldSplitFromPreset );
    if ( wouldSplitFromPreset )
    {
      reqExtLabelText += ' ';
      reqExtLabelText +=
        QString::fromAscii( extDepsDescCStr.data(), extDepsDescCStr.size() );
    }
    else m_reqExtLineEdit->setText( extDepsDescCStr.c_str() );
    m_reqExtLabel->setText( reqExtLabelText );

    update();
  }
}

void DFGExecHeaderWidget::refreshTitle( FTL::CStrRef title )
{
  refresh();
}

void DFGExecHeaderWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  refresh();
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

void DFGExecHeaderWidget::onExecChanged()
{
  refresh();
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

FTL::CStrRef DFGExecHeaderWidget::getExecPath()
{
  return m_dfgController->getExecPath();
}

FabricCore::DFGExec &DFGExecHeaderWidget::getExec()
{
  return m_dfgController->getExec();
}

void DFGExecHeaderWidget::onSplitFromPresetClicked()
{
  m_dfgController->cmdSplitFromPreset();
}
