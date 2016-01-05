// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "FilePathValueWidget.h"
#include "ValueItem.h"
#include "ValueEditorEventFilters.h"
#include <FabricUI/Util/QString_Conversion.h>

#include <QtGui/QFileDialog>
#include <QtCore/QFileInfo>

using namespace FabricUI::TreeView;
using namespace FabricUI::ValueEditor_Legacy;

FilePathValueWidget::FilePathValueWidget(QString label, QWidget * parent)
: ValueWidget(label, parent, true)
{
  QHBoxLayout * hbox = (QHBoxLayout *)layout();

  m_lineEdit = new LineEdit(this);
  m_browseButton = new QPushButton("...", this);
  m_browseButton->setMinimumWidth(60);
  m_browseButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  hbox->addWidget(m_lineEdit);
  hbox->addWidget(m_browseButton);

  m_lineEdit->setFocusPolicy(Qt::StrongFocus);
  m_browseButton->setFocusPolicy(Qt::NoFocus);
  m_browseButton->installEventFilter(new BackspaceDeleteEventFilter(this));

  QObject::connect(m_lineEdit, SIGNAL(lineEditTextEdited(const QString &)), this, SLOT(onValueChangedInLineEdit()));
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
  m_lineEdit->setLineEditText(s);
}

void FilePathValueWidget::setEnabled(bool state)
{
  m_lineEdit->setEnabled(state);
  m_browseButton->setEnabled(state);
}

void FilePathValueWidget::onValueChangedInLineEdit()
{
  QString s = m_lineEdit->text();
  FabricCore::RTVal stringVal = FabricCore::RTVal::ConstructString(*((ValueItem*)item())->client(), QSTRING_TO_CONST_CHAR_VALUE(s));
  ValueWidget::setValue(FabricCore::RTVal::Create(*((ValueItem*)item())->client(), "FilePath", 1, &stringVal));
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

  QFileDialog fileDialog(NULL, valueItem()->name().c_str(), info.dir().absolutePath(), filter);
  fileDialog.setFileMode(QFileDialog::AnyFile);
  fileDialog.setOption(QFileDialog::DontConfirmOverwrite, true);
  fileDialog.setLabelText(QFileDialog::Accept, "Accept");

  QObject::connect(&fileDialog, SIGNAL(fileSelected(const QString &)), valueItem(), SLOT(onFilePathChosen(const QString &)));
  fileDialog.exec();
}

TreeEditorWidget * FilePathValueWidget::creator(QWidget * parent, WidgetTreeItem * item)
{
  FilePathValueWidget * widget = new FilePathValueWidget(item->label().c_str(), parent);
  widget->setItem(item);
  widget->setValue(((ValueItem*)item)->value());
  return widget;
}

bool FilePathValueWidget::canDisplay(WidgetTreeItem * item)
{
  if(item->type() != "ValueItem")
    return false;

  QString typeName = ((ValueItem*)item)->valueTypeName();
  return typeName == "FilePath";
}
