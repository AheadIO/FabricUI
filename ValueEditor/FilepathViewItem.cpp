#include "stdafx.h"
#include "FilepathViewItem.h"
#include "BaseViewItemCreator.h"
#include "ValueEditor\ItemMetadata.h"

inline QString ToQString( const QVariant& var );
FabricCore::RTVal ToFilePath( FabricCore::RTVal& val, const QString& text );

FilepathViewItem::FilepathViewItem(
  QString const &name,
  QVariant const &value
  )
  : BaseViewItem(name)
  , m_val(value.value<FabricCore::RTVal>())
{
  // Our widget shows the value and a "Browse" button
  m_widget = new QWidget;
  m_widget->setObjectName( "FilePathItem" );
  QHBoxLayout *layout = new QHBoxLayout( m_widget );
  QPushButton* browseButton = new QPushButton( m_widget );
  browseButton->setText( "..." );
  m_lineEdit = new QLineEdit(m_widget);
  layout->addWidget( m_lineEdit );
  layout->addWidget( browseButton );

  // Connect button signal to appropriate slot
  connect(
    m_lineEdit, SIGNAL( textEdited( const QString& ) ),
    this, SLOT( OnTextEdited( const QString& ) )
    );
  connect(
    m_lineEdit, SIGNAL( editingFinished() ),
    this, SLOT( OnEditFinished() )
    );
  connect( 
    browseButton, SIGNAL( clicked() ), 
    this, SLOT( doBrowse() ) );
  onModelValueChanged( value );
}

FilepathViewItem::~FilepathViewItem()
{
}

QWidget *FilepathViewItem::getWidget()
{
  return m_widget;
}


void FilepathViewItem::updateMetadata( ItemMetadata* metaData)
{
  if (metaData->has( "filter" ))
    m_filter = metaData->getString( "filter" );
  else
    m_filter = QString();
}



void FilepathViewItem::onModelValueChanged( QVariant const &v )
{
  m_val = v.value<FabricCore::RTVal>();
  QString str = ToQString( v );
  m_lineEdit->setText( str );
}

void FilepathViewItem::doBrowse()
{
  QString fileName = QFileDialog::getOpenFileName( m_widget,
                                           tr( "Open File" ), 
                                           QString(), 
                                           m_filter);
  if (!fileName.isEmpty())
  {
    // Update the string widget
    m_lineEdit->setText( fileName );
    // Update the model
    OnEditFinished();
  }
}



void FilepathViewItem::OnTextEdited( const QString& text )
{
  m_val = ToFilePath( m_val, text );
  emit viewValueChanged(
    QVariant::fromValue<FabricCore::RTVal>( m_val ),
    0
    );
}

void FilepathViewItem::OnEditFinished()
{
  m_val = ToFilePath( m_val, m_lineEdit->text() );
  emit viewValueChanged(
    QVariant::fromValue<FabricCore::RTVal>( m_val ),
    1
    );
}

//////////////////////////////////////////////////////////////////////////
//

inline QString ToQString( const QVariant& var )
{
  FabricCore::RTVal val = var.value<FabricCore::RTVal>();
  const FabricCore::RTVal pathStr = val.callMethod( "String", "string", 0, NULL );
  const char* asCStr = pathStr.getStringCString();
  return asCStr;
}

FabricCore::RTVal ToFilePath( FabricCore::RTVal& val, const QString& text )
{
  FabricCore::RTVal asString = FabricCore::RTVal::ConstructString(
    val.getContext(),
    text.toAscii().data() );

  return FabricCore::RTVal::Create(
    val.getContext(),
    "FilePath",
    1,
    &asString );
}


//////////////////////////////////////////////////////////////////////////
// 
static FilepathViewItem* CreateItem(
  QString const &name,
  QVariant const &value,
  ItemMetadata* /*metaData*/
  )
{
  if (isRTVal(value))
  {
    const FabricCore::RTVal& val = value.value<FabricCore::RTVal>( );
    if (val.isValid())
    {
      const char* rtype = val.getTypeNameCStr();
      if (strcmp( rtype, "FilePath" ) == 0)
        return new FilepathViewItem( name, value );
    }
  }
  return NULL;
}

EXPOSE_VIEW_ITEM( FilepathViewItem, CreateItem, 5 );

// Include MOC'ed file here, in order
// to support PCH on windows.
#include "moc_FilepathViewItem.cpp"