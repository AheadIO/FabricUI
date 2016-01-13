#pragma once

#include "BaseViewItem.h"

class QDoubleSpinBox;

class FilepathViewItem : public BaseViewItem
{
  Q_OBJECT

    FabricCore::RTVal m_val;

public:

	FilepathViewItem(
    QString const &name,
    QVariant const &value
    );
	~FilepathViewItem();

	QWidget *getWidget() override;
  
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
