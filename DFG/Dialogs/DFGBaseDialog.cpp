// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QCursor>
#include <QtGui/QLabel>
#include <QtCore/QDebug>
#include <QtGui/QLineEdit>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>

#include "DFGBaseDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGBaseDialog::DFGBaseDialog(QWidget * parent, bool useGridLayout, const DFGConfig & dfgConfig)
: QDialog(parent)
{
  m_dfgConfig = dfgConfig;
  m_usesGridLayout = useGridLayout;

  setMinimumHeight(10);
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));

  setFont(dfgConfig.defaultFont);

  setContentsMargins(0, 0, 0, 0);
  setLayout(new QVBoxLayout());
  layout()->setContentsMargins(0, 0, 0, 0);
  layout()->setSpacing(0);

  m_inputsWidget = new QWidget(this);
  m_inputsWidget->setContentsMargins(0, 0, 0, 0);
  layout()->addWidget(m_inputsWidget);
  
  QLayout * sectionsLayout = new QVBoxLayout();
  m_inputsWidget->setLayout(sectionsLayout);
  m_inputsWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
  sectionsLayout->setContentsMargins(2, 2, 2, 2);
  sectionsLayout->setSpacing(2);

  m_buttonsWidget = new QWidget(this);
  m_buttonsWidget->setContentsMargins(0, 0, 0, 0);
  layout()->addWidget(m_buttonsWidget);
  
  QHBoxLayout * buttonsLayout = new QHBoxLayout();
  m_buttonsWidget->setLayout(buttonsLayout);
  m_buttonsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

  buttonsLayout->setContentsMargins(2, 2, 2, 2);
  buttonsLayout->setSpacing(2);

  QPushButton * okButton = new QPushButton("OK", m_buttonsWidget);
  QPushButton * cancelButton = new QPushButton("Cancel", m_buttonsWidget);
  okButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  cancelButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

  QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  buttonsLayout->addStretch(2);
  buttonsLayout->addWidget(cancelButton);
  buttonsLayout->addWidget(okButton);
}

DFGBaseDialog::~DFGBaseDialog()
{
}

DFGWidget * DFGBaseDialog::getDFGWidget()
{
  DFGWidget * widget = qobject_cast<DFGWidget*>(parent());
  return widget;
}

DFGController * DFGBaseDialog::getDFGController()
{
  DFGWidget * widget = getDFGWidget();
  if(widget)
    return widget->getUIController();
  return NULL;
}

void DFGBaseDialog::showEvent(QShowEvent * event)
{
  QPoint pos = QCursor().pos();
  QSize currSize = size();
  pos -= QPoint(int(currSize.width() * 0.5), int(currSize.height() * 0.5));

  QRect desktop = QApplication::desktop()->screenGeometry();
  if(pos.x() < 5)
    pos.setX(5);
  else if(pos.x() + currSize.width() + 5 >= desktop.width())
    pos.setX(desktop.width() - currSize.width() - 5);
  if(pos.y() < 5)
    pos.setY(5);
  else if(pos.y() + currSize.height() + 5>= desktop.height())
    pos.setY(desktop.height() - currSize.height() - 5);

  setGeometry(pos.x(), pos.y(), currSize.width(), currSize.height());

  if(m_inputs.size() > 0)
    m_inputs[0]->setFocus();
}

QWidget * DFGBaseDialog::inputsWidget()
{
  return m_inputsWidget;
}

QWidget * DFGBaseDialog::buttonsWidget()
{
  return m_buttonsWidget;
}

void DFGBaseDialog::addInput(QWidget * widget, QString label, QString section)
{
  unsigned int index = m_inputs.size();

  if(section.length() == 0)
    section = "parameters";

  if(m_sections.size() == 0)
  {
    DFGCollapsableWidget * collapsableSection = new DFGCollapsableWidget(m_inputsWidget, section, m_usesGridLayout, m_dfgConfig);
    m_sections.push_back(collapsableSection);
    m_inputsWidget->layout()->addWidget(collapsableSection);
    QObject::connect(collapsableSection, SIGNAL(toggled()), this, SLOT(onSectionToggled()));
    collapsableSection->setCollapsable(false);
  }

  if(m_sections[m_sections.size()-1]->title() != section)
  {
    DFGCollapsableWidget * collapsableSection = new DFGCollapsableWidget(m_inputsWidget, section, m_usesGridLayout, m_dfgConfig);
    m_sections.push_back(collapsableSection);
    m_inputsWidget->layout()->addWidget(collapsableSection);
    QObject::connect(collapsableSection, SIGNAL(toggled()), this, SLOT(onSectionToggled()));
    collapsableSection->setCollapsed(true);
  }

  m_sections[m_sections.size()-1]->addWidget(widget, label);

  widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
  m_inputs.push_back(widget);

  QLineEdit * lineEdit = qobject_cast<QLineEdit*>(widget);
  if(lineEdit)
  {
    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(accept()));
  }

  if(m_usesGridLayout)
  {
    if(label.length() > 0)
    {
      m_labelToIndex.insert(std::pair<std::string,unsigned int>(label.toUtf8().constData(), index));
    }
  }

}

void DFGBaseDialog::setSectionCollapsed(QString section, bool state)
{
  if(section.length() == 0)
    section = "parameters";

  for(size_t i=0;i<m_sections.size();i++)
  {
    if(m_sections[i]->title() == section)
    {
      m_sections[i]->setCollapsed(state);
    }
  }
}

void DFGBaseDialog::setSectionCollapsable(QString section, bool state)
{
  if(section.length() == 0)
    section = "parameters";

  for(size_t i=0;i<m_sections.size();i++)
  {
    if(m_sections[i]->title() == section)
    {
      m_sections[i]->setCollapsable(state);
    }
  }
}

unsigned int DFGBaseDialog::inputCount() const
{
  return (unsigned int)m_inputs.size();
}

QWidget * DFGBaseDialog::input(unsigned int index)
{
  return m_inputs[index];
}

QWidget * DFGBaseDialog::input(QString label)
{
  std::map<std::string, unsigned int>::iterator it = m_labelToIndex.find(label.toUtf8().constData());
  if(it != m_labelToIndex.end())
  {
    return m_inputs[it->second];
  }
  return NULL;
}

void DFGBaseDialog::onSectionToggled()
{
  m_inputsWidget->adjustSize();
  adjustSize();
}

