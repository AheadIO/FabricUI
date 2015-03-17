// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_TabbedKLEditorWidget__
#define __UI_KLEditor_TabbedKLEditorWidget__

#include "MetaTypes.h"
#include "KLEditorWidget.h"

#include <QtGui/QTabWidget>

namespace FabricUI
{

  namespace KLEditor
  {

    class TabbedKLEditorWidget : public QTabWidget, public FabricServices::ASTWrapper::KLASTClient
    {
      Q_OBJECT
    
    public:
    
      TabbedKLEditorWidget(QWidget * parent, FabricServices::ASTWrapper::KLASTManager * manager, const EditorConfig & config = EditorConfig());
      virtual ~TabbedKLEditorWidget();

      bool setASTManager(FabricServices::ASTWrapper::KLASTManager * manager);

      unsigned int editorCount() const;
      KLEditorWidget * editor(unsigned int index);
      KLEditorWidget * openEditor(QString klFilePath = "");
      void closeEditor(unsigned int index);

    public slots:

      void receiveOpenFileRequest(KLDeclPtr);

    private:
      EditorConfig m_config;
    };

  };

};

#endif // __UI_KLEditor_TabbedKLEditorWidget__
