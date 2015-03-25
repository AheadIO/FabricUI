
#ifndef _DFGCOMBINEDWIDGET_H_
#define _DFGCOMBINEDWIDGET_H_

#include <QtGui/QWidget>
#include <QtGui/QSplitter>

#include <map>

#include <DFG/DFGUI.h>
#include <DFG/DFGValueEditor.h>
#include <Commands/CommandStack.h>
#include <DFGWrapper/DFGWrapper.h>

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
        FabricCore::Client * client,
        FabricServices::ASTWrapper::KLASTManager * manager,
        FabricServices::DFGWrapper::Host * host,
        FabricServices::DFGWrapper::Binding binding,
        FabricServices::DFGWrapper::GraphExecutable graph,
        FabricServices::Commands::CommandStack * stack,
        bool overTakeBindingNotifications = true
      );

      void setLogFunc(DFGController::LogFunc func);

      virtual FabricCore::Client * getClient() { return m_client; }
      virtual ASTWrapper::KLASTManager * getManager() { return m_manager; }
      virtual DFGWrapper::Host * getHost() { return m_host; }
      virtual PresetTreeWidget * getTreeWidget() { return m_treeWidget; }
      virtual DFGWidget * getDfgWidget() { return m_dfgWidget; }
      virtual DFGValueEditor * getDfgValueEditor() { return m_dfgValueEditor; }


    public slots:
      virtual void onValueChanged();
      virtual void onStructureChanged();
      virtual void onRecompilation() = 0;
      virtual void onPortRenamed(QString path, QString newName) = 0;
      virtual void hotkeyPressed(Qt::Key, Qt::KeyboardModifier, QString);
      virtual void onNodeDoubleClicked(FabricUI::GraphView::Node * node);

    private:

      void log(const char * message);

      DFGController::LogFunc m_logFunc;
      FabricCore::Client * m_client;
      ASTWrapper::KLASTManager * m_manager;
      DFGWrapper::Host * m_host;
      PresetTreeWidget * m_treeWidget;
      DFGWidget * m_dfgWidget;
      DFGValueEditor * m_dfgValueEditor;
    };

  };

};

#endif 
