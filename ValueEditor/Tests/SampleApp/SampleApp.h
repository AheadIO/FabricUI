#pragma once
#include <VETreeWidget.h>

class MainWindow : public QMainWindow
{
  Q_OBJECT

  typedef QMainWindow Super;

  VETreeWidget m_treeWidget;
public:

  MainWindow();
  void setModelItem( BaseModelItem *model );

  virtual void showEvent( QShowEvent *ev ) /*override*/
  {
    Super::showEvent( ev );
    //int elapsed = myTimer.elapsed();
    //printf( "elapsed: %i\n", elapsed );
  }

signals:
  void modelChanged( BaseModelItem* model );
};
