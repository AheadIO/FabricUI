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
  setLayout(layout);

  m_backgroundColor = config.headerBackgroundColor;
  m_pen = config.headerPen;

  m_goUpButton = new QPushButton("Go up", this);
  QObject::connect(m_goUpButton, SIGNAL(clicked()), this, SIGNAL(goUpPressed()));
  m_goUpButton->setAutoFillBackground(false);

  setFont(config.headerFont);
  setFontColor(config.headerFontColor);

  m_reqExtLineEdit = new QLineEdit;
  QObject::connect(
    m_reqExtLineEdit, SIGNAL(editingFinished()),
    this, SLOT(reqExtEditingFinished())
    );

  layout->addWidget( new QLabel("Required Extensions:") );
  layout->addWidget( m_reqExtLineEdit );
  layout->addStretch(2);
  layout->addWidget(m_goUpButton);
  layout->setAlignment(m_goUpButton, Qt::AlignHCenter | Qt::AlignVCenter);

  QObject::connect(
    m_dfgController, SIGNAL(execChanged()),
    this, SLOT(onExecChanged())
    );
  onExecChanged();
}

DFGExecHeaderWidget::~DFGExecHeaderWidget()
{
}

void DFGExecHeaderWidget::refresh()
{
  m_caption = getExec().getTitle();
  m_reqExtLineEdit->setText( getExec().getExtDeps().getCString() );
  update();
}

void DFGExecHeaderWidget::refreshExtDeps( FTL::CStrRef extDeps )
{
  m_reqExtLineEdit->setText( extDeps.c_str() );
  update();
}

void DFGExecHeaderWidget::reqExtEditingFinished()
{
  m_reqExtLineEdit->clearFocus();
  
  std::string extDepDesc = m_reqExtLineEdit->text().toUtf8().constData();
  FTL::StrRef::Split split = FTL::StrRef( extDepDesc ).split(',');
  std::vector<std::string> nameAndVerStrings;
  while ( !split.first.empty() )
  {
    nameAndVerStrings.push_back( split.first.trim() );
    split = split.second.split(',');
  }

  std::vector<char const *> nameAndVerCStrs;
  nameAndVerCStrs.reserve( nameAndVerStrings.size() );
  for ( size_t i = 0; i < nameAndVerStrings.size(); ++i )
    nameAndVerCStrs.push_back( nameAndVerStrings[i].c_str() );

  if(nameAndVerCStrs.size() == 0)
    return;

  try
  {
    getExec().setExtDeps(
      nameAndVerCStrs.size(),
      &nameAndVerCStrs[0]
      );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "%s\n", e.getDesc_cstr() );
  }
}

QString DFGExecHeaderWidget::caption() const
{
  return m_caption;
}

QFont DFGExecHeaderWidget::font() const
{
  return m_font;
}

QColor DFGExecHeaderWidget::fontColor() const
{
  return m_fontColor;
}

void DFGExecHeaderWidget::setFont(QFont f)
{
  m_font = f;
  m_goUpButton->setFont(f);
  update();
}

void DFGExecHeaderWidget::setFontColor(QColor c)
{
  m_fontColor = c;

  QPalette p = m_goUpButton->palette();
  p.setColor(QPalette::ButtonText, c);
  p.setColor(QPalette::Button, m_config.nodeDefaultColor);
  m_goUpButton->setPalette(p);
  update();
}

void DFGExecHeaderWidget::onExecChanged()
{
  if(m_dfgController->isViewingRootGraph())
  {
    m_goUpButton->hide();
  }
  else
  {
    m_goUpButton->show();
  }

  m_caption = m_dfgController->getExecPath().c_str();
  update();
}

void DFGExecHeaderWidget::paintEvent(QPaintEvent * event)
{
  QRect rect = contentsRect();
  QPainter painter(this);

  painter.fillRect(rect, m_backgroundColor);

  painter.setPen(m_pen);
  painter.drawLine(rect.bottomLeft(), rect.bottomRight());

  QFontMetrics metrics(m_font);

  painter.setFont(m_font);
  painter.setPen(m_fontColor);

  int left = int(size().width() * 0.5 - metrics.width(m_caption) * 0.5);
  int top = int(size().height() * 0.5 + metrics.height() * 0.5);
  painter.drawText(left, top, m_caption);

  QWidget::paintEvent(event);
}

FabricCore::DFGExec &DFGExecHeaderWidget::getExec()
{
  return m_dfgController->getExec();
}
