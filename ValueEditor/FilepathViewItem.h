#pragma once

#include "BaseViewItem.h"
#include <FabricCore.h>

class QWidget;
class QLineEdit;
class ItemMetadata;

class FilepathViewItem : public BaseViewItem
{
  Q_OBJECT

    FabricCore::RTVal m_val;

public:

  static BaseViewItem *CreateItem(
    QString const &name,
    QVariant const &value,
    ItemMetadata* metaData
    );
  static const int Priority;

	FilepathViewItem(
    QString const &name,
    QVariant const &value
    );
	~FilepathViewItem();

	QWidget *getWidget() /*override*/;
  
	void onModelValueChanged( QVariant const &value ) /*override*/;

  void deleteMe() /*override*/ { delete this; }

  void updateMetadata( ItemMetadata* metaData ) /*override*/;


private:

  QWidget *m_widget;
  QLineEdit *m_lineEdit;
  QString m_filter;

private slots:
  void doBrowse();
  void OnTextEdited( const QString& text );
  void OnEditFinished();

};
