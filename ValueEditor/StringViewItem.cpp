#include "stdafx.h"
#include "StringViewItem.h"
#include "BaseViewItemCreator.h"

StringViewItem::StringViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
{
  m_widget = new QWidget;
  m_widget->setObjectName( "StringItem" );
  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  QPushButton* inspectButton = new QPushButton( m_widget );
  QIcon inspectIcon( "icons/mag_glass.png" );
  inspectButton->setIcon( inspectIcon );
  m_edit = new QLineEdit( m_widget );

  layout->addWidget( m_edit );
  layout->addWidget( inspectButton );

  connect(
    m_edit, SIGNAL( textEdited( const QString& ) ),
    this, SLOT( OnTextEdited( const QString& ) )
    );
  connect(
    m_edit, SIGNAL( editingFinished() ),
    this, SLOT( OnEditFinished() )
    );
  connect(
    inspectButton, SIGNAL( clicked() ),
    this, SLOT( onInspect() ) );
  onModelValueChanged( value );
}

StringViewItem::~StringViewItem()
{
}

QWidget *StringViewItem::getWidget()
{
  return m_widget;
}

void StringViewItem::onModelValueChanged( QVariant const &v )
{
  m_edit->setText( v.value<QString>() );
}

void StringViewItem::OnTextEdited( const QString& text )
{
  emit viewValueChanged(
    QVariant::fromValue<QString>( text ),
    0
    );
}

void StringViewItem::OnEditFinished()
{
  emit viewValueChanged(
    QVariant::fromValue<QString>( m_edit->text() ),
    1
    );
}

void StringViewItem::onInspect()
{
  // Show a multi-line dialog to display 
  // the full string contents
  QDialog dlg( m_widget, Qt::Window | Qt::Dialog /*| Qt::FramelessWindowHint*/ );
  dlg.setModal( true );
  // Add text editor
  QVBoxLayout *layout = new QVBoxLayout( &dlg );
  QPlainTextEdit* txtEdit = new QPlainTextEdit;
  txtEdit->setPlainText( m_edit->text() );
  layout->addWidget( txtEdit );

  // Ok/cancel
  QDialogButtonBox* buttonBox = new QDialogButtonBox( 
                                      QDialogButtonBox::Ok
                                    | QDialogButtonBox::Cancel );

  connect( buttonBox, SIGNAL( accepted() ), &dlg, SLOT( accept() ) );
  connect( buttonBox, SIGNAL( rejected() ), &dlg, SLOT( reject() ) );
  layout->addWidget( buttonBox );

 
  // Post dialog under mouse
  QPoint pos = QCursor::pos();
  dlg.move( pos.x(), pos.y() );

  if (dlg.exec() == QDialog::Accepted)
  {
    OnTextEdited( txtEdit->toPlainText() );
  }
}

//////////////////////////////////////////////////////////////////////////
// 
static StringViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  if (RTVariant::isType<QString>(value))
  {
    return new StringViewItem( name, value );
  }
  return 0;
}

EXPOSE_VIEW_ITEM( StringViewItem, CreateItem, 3 );

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_StringViewItem.cpp"