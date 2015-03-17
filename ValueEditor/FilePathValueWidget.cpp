// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "FilePathValueWidget.h"
#include "ValueItem.h"

#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor;

FilePathValueWidget::FilePathValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_lineEdit = new QLineEdit(this);
  m_browseButton = new QPushButton("...", this);
  m_browseButton->setMinimumWidth(60);
  m_browseButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  hbox->addWidget(m_lineEdit);
  hbox->addWidget(m_browseButton);

  QObject::connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(onValueChangedInLineEdit()));
  QObject::connect(m_browseButton, SIGNAL(clicked()), this, SLOT(onBrowseClicked()));
}

FilePathValueWidget::~FilePathValueWidget()
{
}

void FilePathValueWidget::setValue(FabricCore::RTVal v)
{
  ValueWidget::setValue(v);
  FabricCore::RTVal stringVal = v.callMethod("String", "string", 0, 0);
  QString s = stringVal.getStringCString();
  m_lineEdit->setText(s);
}

void FilePathValueWidget::setEnabled(bool state)
{
  m_lineEdit->setEnabled(state);
  m_browseButton->setEnabled(state);
}

void FilePathValueWidget::onValueChangedInLineEdit()
{
  QString s = m_lineEdit->text();
  FabricCore::RTVal stringVal = FabricCore::RTVal::ConstructString(*((ValueItem*)item())->client(), s.toUtf8().constData());
  m_value = FabricCore::RTVal::Create(*((ValueItem*)item())->client(), "FilePath", 1, &stringVal);
  emit dataChanged();
}

void FilePathValueWidget::onBrowseClicked()
{
  FabricCore::RTVal stringVal = m_value.callMethod("String", "string", 0, 0);
  QString filePath = stringVal.getStringCString();
  QFileInfo info(filePath);
  QString filter = info.suffix();
  if(filter.length() == 0)
    filter = "All files (*.*)";
  else
    filter = filter + " files (*." + filter + ");;All files (*.*)";

  filePath = QFileDialog::getSaveFileName(this, valueItem()->name(), info.dir().absolutePath(), filter);
  if(filePath.isEmpty())
    return;

  stringVal = FabricCore::RTVal::ConstructString(*((ValueItem*)item())->client(), filePath.toUtf8().constData());
  m_value = FabricCore::RTVal::Create(*((ValueItem*)item())->client(), "FilePath", 1, &stringVal);
  m_lineEdit->setText(filePath);
  emit dataChanged();
}

TreeEditorWidget * FilePathValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  FilePathValueWidget * widget = new FilePathValueWidget(item->label(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool FilePathValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(!ValueWidget::canDisplay(item))
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "FilePath";
}
