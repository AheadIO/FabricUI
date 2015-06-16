
#ifndef _DFGCOMBINEDWIDGET_H_
#define _DFGCOMBINEDWIDGET_H_

#include <QtGui/QWidget>
#include <QtGui/QSplitter>

#include <map>

#include <FabricUI/DFG/DFGUI.h>
#include <FabricUI/DFG/DFGValueEditor.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <Commands/CommandStack.h>

using namespace FabricServices;
using namespace FabricUI;

namespace FabricUI
{

  namespace DFG
  {
    class DFGCombinedWidget : public QSplitter
    {

      Q_OBJECT
      
    public:
      DFGCombinedWidget(QWidget * parent);

      ~DFGCombinedWidget();

      virtual void init(      
        FabricCore::Client client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricCore::DFGHost host,
        FabricCore::DFGBinding binding,
        FabricCore::DFGExec exec,
        FabricServices::Commands::CommandStack * stack,
        bool overTakeBindingNotifications = true,
        DFGConfig config = DFGConfig()
      );

      static void setLogFunc(DFGController::LogFunc func);

      FabricCore::Client const &getClient()
        { return m_coreClient; }
      FabricCore::DFGHost const &getCoreDFGHost()
        { return m_coreDFGHost; }
      FabricCore::DFGBinding const &getCoreDFGBinding()
        { return m_coreDFGBinding; }
      FabricCore::DFGExec const &getCoreDFGExec()
        { return m_coreDFGExec; }

      virtual ASTWrapper::KLASTManager * getManager() { return m_manager; }
      virtual PresetTreeWidget * getTreeWidget() { return m_treeWidget; }
      virtual DFGWidget * getDfgWidget() { return m_dfgWidget; }
      virtual DFGValueEditor * getDfgValueEditor() { return m_dfgValueEditor; }
      virtual DFGLogWidget * getDfgLogWidget() { return m_dfgLogWidget; }

      virtual void keyPressEvent(QKeyEvent * event);

    public slots:
      virtual void onValueChanged();
      virtual void onStructureChanged();
      virtual void onRecompilation() = 0;
      virtual void hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString);
      virtual void onGraphSet(FabricUI::GraphView::Graph * graph);
      virtual void onNodeDoubleClicked(FabricUI::GraphView::Node * node);

    signals:

      void valueChanged();

    private:

      void log(const char * message);

      QSplitter * m_hSplitter;
      FabricCore::Client m_coreClient;
      ASTWrapper::KLASTManager * m_manager;
      FabricCore::DFGHost m_coreDFGHost;
      FabricCore::DFGBinding m_coreDFGBinding;
      FabricCore::DFGExec m_coreDFGExec;
      PresetTreeWidget * m_treeWidget;
      DFGWidget * m_dfgWidget;
      DFGValueEditor * m_dfgValueEditor;
      DFGLogWidget * m_dfgLogWidget;
    };

  };

};

#endif 
