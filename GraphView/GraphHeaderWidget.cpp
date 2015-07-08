// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include "GraphHeaderWidget.h"

using namespace FabricUI::GraphView;

GraphHeaderWidget::GraphHeaderWidget(
  QWidget * parent,
  QString caption,
  FabricCore::DFGBinding const &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec const &exec,
  const GraphConfig &config
  )
  : QWidget(parent)
  , m_binding( binding )
  , m_execPath( execPath )
  , m_exec( exec )
{
  m_config = config;
  m_caption = caption;

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
}

GraphHeaderWidget::~GraphHeaderWidget()
{
}

void GraphHeaderWidget::setExec(
  FabricCore::DFGBinding const &binding,
  FTL::StrRef execPath,
  FabricCore::DFGExec const &exec
  )
{
  m_binding = binding;
  m_execPath = execPath;
  m_exec = exec;
  setExecExtDeps( m_exec.getExtDeps().getCString() );
}

void GraphHeaderWidget::setExecExtDeps( FTL::CStrRef extDeps )
{
  m_reqExtLineEdit->setText( extDeps.c_str() );
}

void GraphHeaderWidget::reqExtEditingFinished()
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

  try
  {
    m_exec.setExtDeps(
      nameAndVerCStrs.size(),
      &nameAndVerCStrs[0]
      );
  }
  catch ( FabricCore::Exception e )
  {
    printf( "%s\n", e.getDesc_cstr() );
  }
}

QString GraphHeaderWidget::caption() const
{
  return m_caption;
}

QFont GraphHeaderWidget::font() const
{
  return m_font;
}

QColor GraphHeaderWidget::fontColor() const
{
  return m_fontColor;
}

void GraphHeaderWidget::setCaption(QString text)
{
  m_caption = text;
  update();
}

void GraphHeaderWidget::setFont(QFont f)
{
  m_font = f;
  m_goUpButton->setFont(f);
  update();
}

void GraphHeaderWidget::setFontColor(QColor c)
{
  m_fontColor = c;

  QPalette p = m_goUpButton->palette();
  p.setColor(QPalette::ButtonText, c);
  p.setColor(QPalette::Button, m_config.nodeDefaultColor);
  m_goUpButton->setPalette(p);
  update();
}

void GraphHeaderWidget::paintEvent(QPaintEvent * event)
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
