// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGKLEditorWidget__
#define __UI_DFG_DFGKLEditorWidget__

#include <QtGui/QWidget>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QStringListModel>
#include <FabricUI/KLEditor/KLEditorWidget.h>
#include "DFGConfig.h"
#include "DFGController.h"
#include "DFGKLEditorPortTableWidget.h"


namespace FabricUI
{

  namespace DFG
  {

    class DFGKLEditorWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGKLEditorWidget(QWidget * parent, DFGController * controller, FabricServices::ASTWrapper::KLASTManager * manager, const DFGConfig & config = DFGConfig());
      virtual ~DFGKLEditorWidget();

      bool hasUnsavedChanges() const { return m_unsavedChanges; }
      KLEditor::KLEditorWidget * klEditor() { return m_klEditor; }

      virtual void closeEvent(QCloseEvent * event);

    public slots:

      void onExecChanged();
      void onExecPortsChanged();
      void save();
      void reload();
      void onNewUnsavedChanges();
      void onExecSplitChanged();

    signals:

      void execChanged();

    protected:

      FTL::StrRef getExecPath()
        { return m_controller->getExecPath(); }
      FabricCore::DFGExec &getExec()
        { return m_controller->getExec(); }

      void updateDiags( bool saving = false );

    private:

      DFGController * m_controller;
      DFGKLEditorPortTableWidget * m_ports;
      KLEditor::KLEditorWidget * m_klEditor;
      QPlainTextEdit *m_diagsText;
      DFGConfig m_config;
      bool m_unsavedChanges;
      bool m_isSettingPorts;
    };

  };

};

#endif // __UI_DFG_DFGKLEditorWidget__
