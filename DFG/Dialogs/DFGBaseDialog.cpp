// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QPushButton>
#include <QtGui/QCursor>
#include <QtGui/QLabel>
#include <QtCore/QDebug>
#include <QtGui/QLineEdit>

#include "DFGBaseDialog.h"

using namespace FabricUI;
using namespace FabricUI::DFG;

DFGBaseDialog::DFGBaseDialog(QWidget * parent, bool useGridLayout, const DFGConfig & dfgConfig)
: QDialog(parent)
{
  m_dfgConfig = dfgConfig;
  m_usesGridLayout = useGridLayout;

  setFont(dfgConfig.defaultFont);

  setContentsMargins(0, 0, 0, 0);
  setLayout(new QVBoxLayout());
  layout()->setContentsMargins(0, 0, 0, 0);
  layout()->setSpacing(0);

  m_inputsWidget = new QWidget(this);
  m_inputsWidget->setContentsMargins(0, 0, 0, 0);
  layout()->addWidget(m_inputsWidget);
  
  QLayout * inputsLayout;
  if(m_usesGridLayout)
    inputsLayout = new QGridLayout();
  else
    inputsLayout = new QVBoxLayout();
  m_inputsWidget->setLayout(inputsLayout);
  m_inputsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  inputsLayout->setContentsMargins(2, 2, 2, 2);
  inputsLayout->setSpacing(2);

  m_buttonsWidget = new QWidget(this);
  m_buttonsWidget->setContentsMargins(0, 0, 0, 0);
  layout()->addWidget(m_buttonsWidget);
  
  QHBoxLayout * buttonsLayout = new QHBoxLayout();
  m_buttonsWidget->setLayout(buttonsLayout);
  m_buttonsWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

  buttonsLayout->setContentsMargins(2, 2, 2, 2);
  buttonsLayout->setSpacing(2);

  QPushButton * cancelButton = new QPushButton("Cancel", m_buttonsWidget);
  QPushButton * okButton = new QPushButton("OK", m_buttonsWidget);
  cancelButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
  okButton->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));

  QObject::connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  QObject::connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  buttonsLayout->addStretch(2);
  buttonsLayout->addWidget(cancelButton);
  buttonsLayout->addWidget(okButton);
}

DFGBaseDialog::~DFGBaseDialog()
{
}

void DFGBaseDialog::showEvent(QShowEvent * event)
{
  QPoint pos = QCursor().pos();
  QSize currSize = size();
  pos -= QPoint(currSize.width() * 0.5, currSize.height() * 0.5);
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

void DFGBaseDialog::addInput(QWidget * widget, QString label)
{
  widget->setParent(m_inputsWidget);
  m_inputs.push_back(widget);

  QLineEdit * lineEdit = qobject_cast<QLineEdit*>(widget);
  if(lineEdit)
  {
    QObject::connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(accept()));
  }

  QLayout * layout = m_inputsWidget->layout();

  if(m_usesGridLayout)
  {
    int row = ((QGridLayout*)layout)->rowCount();
    if(label.length() > 0)
    {
      QLabel * l = new QLabel(label, m_inputsWidget);
      ((QGridLayout*)layout)->addWidget(l, row, 0);
      layout->setAlignment(l, Qt::AlignRight | Qt::AlignVCenter);
    }
    ((QGridLayout*)layout)->addWidget(widget, row, 1);
    layout->setAlignment(widget, Qt::AlignLeft | Qt::AlignVCenter);
  }
  else
  {
    layout->addWidget(widget);
    layout->setAlignment(widget, Qt::AlignLeft | Qt::AlignVCenter);
  }

}
