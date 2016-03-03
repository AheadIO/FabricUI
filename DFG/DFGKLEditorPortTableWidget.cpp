// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "DFGKLEditorPortTableWidget.h"

#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QHeaderView>
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QMenu>
#include <QtCore/QCoreApplication>

using namespace FabricServices;
using namespace FabricUI;
using namespace FabricUI::DFG;



DFGKLEditorPortTableWidget::DFGKLEditorPortTableWidget(QWidget * parent, DFGController * controller, const DFGConfig & config, bool setAlphaNum)
: QTableWidget(parent)
{
  m_controller = controller;
  m_config = config;
  m_signalsEnabled = true;

  // [Julien] FE-5276 Validate the names in port creation and avoid illegal characters
  if(setAlphaNum) setItemDelegate(new DFGKLEditorPortTableWidgetDelegate());

  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
  setStyleSheet("QTableWidget::item { padding: 0px }");

  setMinimumHeight(40);

  verticalHeader()->hide();
  verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  // verticalHeader()->setResizeMode(QHeaderView::Fixed);
  // verticalHeader()->setDefaultSectionSize(24);

  // setColumnCount(4);
  setColumnCount(3);
  horizontalHeader()->setClickable(false);
  horizontalHeader()->setDefaultSectionSize(60);
  horizontalHeader()->setResizeMode(0, QHeaderView::Fixed);
  horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
  horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
  // horizontalHeader()->setResizeMode(3, QHeaderView::Stretch);

  QStringList headerLabels;
  headerLabels.push_back("mode");
  headerLabels.push_back("name");
  headerLabels.push_back("type");
  // headerLabels.push_back("extension");
  setHorizontalHeaderLabels(headerLabels);

  // connect all signals / slots
  QObject::connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(onCellChanged(int, int)));
 
  setContextMenuPolicy(Qt::CustomContextMenu);
  QObject::connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onCustomContextMenuRequested(const QPoint &)));
}

DFGKLEditorPortTableWidget::~DFGKLEditorPortTableWidget()
{
}

void DFGKLEditorPortTableWidget::refresh()
{
  int prevCurrentRow = currentRow_safe();
  int prevCurrentColumn = currentColumn();

  m_signalsEnabled = false;

  clearContents();
  while(rowCount() > 0)
    removeRow(0);

  try
  {
    FabricCore::DFGExec &exec = getExec();

    if(exec.getExecPortCount() == 0)
    {
      // add a single port to start off of
      addPort(FabricCore::DFGPortType_In, "", "$TYPE$");
    }
    else
    {
      for(size_t i=0;i<exec.getExecPortCount();i++)
      {
        FabricCore::DFGPortType portType = exec.getExecPortType(i);
        char const * portName = exec.getExecPortName(i);
        char const * portTypeSpec = exec.getExecPortTypeSpec(i);
        addPort(portType, portName, portTypeSpec);
      }
    }
  }
  catch(FabricCore::Exception e)
  {
    m_controller->logError(e.getDesc_cstr());
  }

  if(prevCurrentRow <= rowCount() && prevCurrentColumn <= columnCount())
    setCurrentCell(prevCurrentRow, prevCurrentColumn);

  m_signalsEnabled = true;
}

unsigned int DFGKLEditorPortTableWidget::nbPorts() const
{
  unsigned int nbRows = 0;
  for(int i=0;i<rowCount();i++)
  {
    if(portName(i).length() > 0)
      nbRows++;
  }
  return nbRows;
}

DFGKLEditorPortTableWidget::PortInfo DFGKLEditorPortTableWidget::portInfo(unsigned int index) const
{
  PortInfo info;
  info.portType = portType(index);  
  info.portName = portName(index).toUtf8().constData();  
  info.dataType = dataType(index).toUtf8().constData();  
  info.extension = extension(index).toUtf8().constData();
  return info;
}

FabricCore::DFGPortType DFGKLEditorPortTableWidget::portType(unsigned int index) const
{
  if(rowCount() == 0)
    return FabricCore::DFGPortType_In;

  if(int(index) >= rowCount())
    index = rowCount() - 1;

  int portTypeId = ((QComboBox*)cellWidget(index, 0))->currentIndex();
  if(portTypeId == 0)
    return FabricCore::DFGPortType_In;
  else if(portTypeId == 1)
    return FabricCore::DFGPortType_Out;
  return FabricCore::DFGPortType_IO;
}

QString DFGKLEditorPortTableWidget::portName(unsigned int index) const
{
  if(rowCount() == 0)
    return "";
  
  if(int(index) >= rowCount())
    index = rowCount() - 1;

  return item(index, 1)->text();
}

QString DFGKLEditorPortTableWidget::dataType(unsigned int index) const
{
  if(rowCount() == 0)
    return "";
  
  if(int(index) >= rowCount())
    index = rowCount() - 1;

  return item(index, 2)->text();
}

QString DFGKLEditorPortTableWidget::extension(unsigned int index) const
{
  if(rowCount() == 0)
    return "";
  
  if(int(index) >= rowCount())
    index = rowCount() - 1;

  if(columnCount() <= 3)
    return "";
  return item(index, 3)->text();
}

void DFGKLEditorPortTableWidget::keyPressEvent(QKeyEvent * event)
{
  QTableWidget::keyPressEvent(event);

  if(event->modifiers().testFlag(Qt::ControlModifier))
  {
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    {
      if(rowCount() == 0)
        return;
      if(currentRow() == rowCount()-1)
      {
        // we need to process the events here, since we need to 
        // ensure that the item has been filled after the enter press
        QCoreApplication::processEvents();

        int index = currentRow_safe();
        if(portName(index).length() > 0)
        {
          index = addPort(portType(index), "", dataType(index), extension(index));
          setCurrentCell(index, 0);
          return;
        }
      }
    }
    else if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
    {
      if(rowCount() <= 1)
        return;

      int index = currentRow_safe();
      removeRow(index);
      if(index >= rowCount())
        setCurrentCell(rowCount()-1, 0);
      else
        setCurrentCell(index, 0);
      if(m_signalsEnabled)
        emit execPortsChanged();
    }
  }
  else if(event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete)
  {
    if(currentColumn() > 0)
    {
      item(currentRow(), currentColumn())->setText("");
    }
  }

}

void DFGKLEditorPortTableWidget::onCellChanged(int row, int col)
{
  if(m_signalsEnabled)
    emit execPortsChanged();
}

void DFGKLEditorPortTableWidget::onComboBoxChanged(int index)
{
  if(m_signalsEnabled)
    emit execPortsChanged();
}

int DFGKLEditorPortTableWidget::currentRow_safe() const
{
  int result = currentRow();
  if(result < 0)
    result = 0;
  return result;
}

int DFGKLEditorPortTableWidget::addPort(FabricCore::DFGPortType portType, QString portName, QString dataType, QString extension)
{
  int index = rowCount();
  insertRow(index);

  QComboBox * modeBox = new QComboBox(this);
  modeBox->addItem("in");
  modeBox->addItem("out");
  modeBox->addItem("io");

  if(portType == FabricCore::DFGPortType_In)
    modeBox->setCurrentIndex(0);
  else if(portType == FabricCore::DFGPortType_Out)
    modeBox->setCurrentIndex(1);
  else
    modeBox->setCurrentIndex(2);

  QObject::connect(modeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxChanged(int)));

  setCellWidget(index, 0, modeBox);
  setItem(index, 1, new QTableWidgetItem(portName));
  setItem(index, 2, new QTableWidgetItem(dataType));

  return index;
}

void DFGKLEditorPortTableWidget::onCustomContextMenuRequested(const QPoint & pos)
{
  QPoint globalPos = mapToGlobal(pos);

  QMenu *menu=new QMenu(this);
  menu->addAction(new QAction("Delete Port (Ctrl+BackSpace)", this));
  menu->addAction(new QAction("Add New Port (Ctrl+Enter)", this));
  QObject::connect(menu, SIGNAL(triggered(QAction *)), this, SLOT(contextMenuTriggered(QAction *)));
  menu->exec(globalPos);
}

void DFGKLEditorPortTableWidget::contextMenuTriggered(QAction * action)
{
  if(action->text() == "Delete Port (Ctrl+BackSpace)")
  {
    if(rowCount() == 0)
      return;
    if(rowCount() == 1)
    {
      if(portName(0) == "" && dataType(0) != "$TYPE$")
        return;
    }

    int index = currentRow_safe();
    removeRow(index);
    if(index >= rowCount())
      setCurrentCell(rowCount()-1, 0);
    else
      setCurrentCell(index, 0);
    if(m_signalsEnabled)
      emit execPortsChanged();
  }
  else if(action->text() == "Add New Port (Ctrl+Enter)")
  {
    int index = currentRow_safe();
    index = addPort(portType(index), "", dataType(index), extension(index));
    setCurrentCell(index, 0);
    return;
  }
}
