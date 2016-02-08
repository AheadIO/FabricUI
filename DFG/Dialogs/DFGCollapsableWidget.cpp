// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <iostream>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>

#include "DFGCollapsableWidget.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGCollapsableWidget::DFGCollapsableWidget(QWidget * parent, QString title, bool useGridLayout, const DFGConfig & dfgConfig)
: QWidget(parent)
, m_collapsed(false)
, m_dfgConfig(dfgConfig)
, m_usesGridLayout(useGridLayout)
{
  QVBoxLayout * layout = new QVBoxLayout();
  setLayout(layout);

  m_header = new DFGCollapsableHeaderWidget(this, title);
  m_content = new QWidget(this);

  setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  m_header->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  m_content->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

  setContentsMargins(0, 0, 0, 0);
  layout->setContentsMargins(1, 1, 1, 1);
  layout->setSpacing(2);

  layout->addWidget(m_header);
  layout->addWidget(m_content);

  if(useGridLayout)
    m_content->setLayout(new QGridLayout());
  else
    m_content->setLayout(new QVBoxLayout());

  m_content->layout()->setContentsMargins(0, 0, 0, 0);
  m_content->layout()->setSpacing(1);

  QObject::connect(m_header, SIGNAL(toggled()), this, SLOT(onToggled()));
}

DFGCollapsableWidget::~DFGCollapsableWidget()
{
}

QString DFGCollapsableWidget::title() const
{
  return m_header->text();
}

void DFGCollapsableWidget::addWidget(QWidget * widget, QString label)
{
  widget->setParent(m_content);  

  QLayout * layout = m_content->layout();

  if(m_usesGridLayout)
  {
    int row = ((QGridLayout*)layout)->rowCount();
    if(label.length() > 0)
    {
      QLabel * l = new QLabel(label, m_content);
      ((QGridLayout*)layout)->addWidget(l, row, 0);
      layout->setAlignment(l, Qt::AlignRight | Qt::AlignVCenter);
    }
    ((QGridLayout*)layout)->addWidget(widget, row, 1);
    // layout->setAlignment(widget, Qt::AlignLeft | Qt::AlignVCenter);
  }
  else
  {
    layout->addWidget(widget);
    // layout->setAlignment(widget, Qt::AlignLeft | Qt::AlignVCenter);
  }
}

void DFGCollapsableWidget::removeWidget(QWidget * widget)
{
  QLayout * layout = m_content->layout();
  if(m_usesGridLayout)
  {
    int index = ((QGridLayout*)layout)->indexOf(widget);

    if(index > 0)
    {
      int row, column, rowSpan, columnSpan;
      ((QGridLayout*)layout)->getItemPosition(index, &row, &column, &rowSpan, &columnSpan);

      QWidget * label = ((QGridLayout*)layout)->itemAtPosition(row, 0)->widget();
      ((QGridLayout*)layout)->removeWidget(label);
      delete label;
      label = 0;

      QWidget * widget2 = ((QGridLayout*)layout)->itemAtPosition(row, 1)->widget();
      ((QGridLayout*)layout)->removeWidget(widget2);
      delete widget2;
      widget2 = 0;
    }   
  }
  else 
  {
    layout->removeWidget(widget);
    widget = 0;
  }
}

void DFGCollapsableWidget::setCollapsed(bool state)
{
  if(state == m_collapsed)
    return;
  m_collapsed = state;

  if(m_collapsed)
    m_content->hide();
  else
    m_content->show();

  m_content->adjustSize();
  adjustSize();
  emit toggled();

  if(m_collapsed)
    emit collapsed();
  else
    emit expanded();
}

void DFGCollapsableWidget::setCollapsable(bool state)
{
  m_collapsable = state;
  if(!m_collapsable)
    setCollapsed(false);
}

void DFGCollapsableWidget::onToggled()
{
  setCollapsed(!m_collapsed);
}
