
#ifndef _DFGCOMBINEDWIDGET_H_
#define _DFGCOMBINEDWIDGET_H_

#include <QtGui/QWidget>
#include <QtGui/QSplitter>

#include <map>

#include <FabricUI/DFG/DFGUI.h>
#include <FabricUI/DFG/DFGValueEditor.h>
#include <FabricUI/DFG/DFGLogWidget.h>
#include <FabricUI/DFG/Dialogs/DFGBaseDialog.h>
#include <Commands/CommandStack.h>

using namespace FabricServices;
using namespace FabricUI;

namespace FabricUI
{

  namespace DFG
  {

    class DFGUICmdHandler;
    
    class DFGCombinedWidget : public QSplitter
    {

      Q_OBJECT
      
    public:
      DFGCombinedWidget(QWidget * parent);

      ~DFGCombinedWidget();

      virtual void init(      
        FabricCore::Client const &client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricCore::DFGHost const &host,
        FabricCore::DFGBinding const &binding,
        FTL::StrRef execPath,
        FabricCore::DFGExec const &exec,
        DFGUICmdHandler *cmdHandler,
        FabricServices::Commands::CommandStack * stack,
        bool overTakeBindingNotifications = true,
        DFGConfig config = DFGConfig()
        );

      static void setLogFunc(DFGController::LogFunc func);

      FabricCore::Client &getClient()
        { return m_client; }
      FabricCore::DFGHost &getCoreDFGHost()
        { return m_host; }
      FabricCore::DFGBinding &getCoreDFGBinding()
        { return m_binding; }
      FabricCore::DFGExec &getCoreDFGExec()
        { return m_exec; }

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

    private slots:
      void onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog);

    signals:

      void valueChanged();
      void portEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
      void portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog);

    private:

      void log(const char * message);

      QSplitter * m_hSplitter;
      FabricCore::Client m_client;
      ASTWrapper::KLASTManager * m_manager;
      FabricCore::DFGHost m_host;
      FabricCore::DFGBinding m_binding;
      std::string m_execPath;
      FabricCore::DFGExec m_exec;
      PresetTreeWidget * m_treeWidget;
      DFGWidget * m_dfgWidget;
      DFGValueEditor * m_dfgValueEditor;
      DFGLogWidget * m_dfgLogWidget;
      FabricUI::GraphView::Graph * m_setGraph;
    };

  };

};

#endif 
