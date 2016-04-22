#ifndef FABRICUI_VALUEEDITOR_TESTS_SAMPLEAPP_SAMPLEAPP_H
#define FABRICUI_VALUEEDITOR_TESTS_SAMPLEAPP_SAMPLEAPP_H
#include <VETreeWidget.h>

using namespace FabricUI::ValueEditor;

class MainWindow : public QMainWindow
{
  Q_OBJECT

  typedef QMainWindow Super;

  VETreeWidget m_treeWidget;
public:

  MainWindow();
  void setModelItem( FabricUI::ValueEditor::BaseModelItem *model );

  virtual void showEvent( QShowEvent *ev ) /*override*/
  {
    Super::showEvent( ev );
    //int elapsed = myTimer.elapsed();
    //printf( "elapsed: %i\n", elapsed );
  }

signals:
  void modelChanged( FabricUI::ValueEditor::BaseModelItem* model );
};

#endif // FABRICUI_VALUEEDITOR_TESTS_SAMPLEAPP_SAMPLEAPP_H
