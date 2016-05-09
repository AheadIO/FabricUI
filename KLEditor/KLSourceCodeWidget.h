// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_KLSourceCodeWidget__
#define __UI_KLEditor_KLSourceCodeWidget__

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QWidget>
#include <QtGui/QPlainTextEdit>

#include "EditorConfig.h"
#include "MetaTypes.h"
#include "Helpers.h"
#include "KLSyntaxHighlighter.h"
#include "CodeCompletionPopup.h"

#include <ASTWrapper/KLASTClient.h>
#include <ASTWrapper/KLASTManager.h>
#include <CodeCompletion/KLCodeAssistant.h>

namespace FabricUI
{

  namespace KLEditor
  {

    class KLSourceCodeWidget : public QPlainTextEdit, public FabricServices::ASTWrapper::KLASTClient
    {
      Q_OBJECT
    
    public:
    
      KLSourceCodeWidget(QWidget * parent, FabricServices::ASTWrapper::KLASTManager * manager, const EditorConfig & config = EditorConfig());
      virtual ~KLSourceCodeWidget();

      virtual bool setASTManager(FabricServices::ASTWrapper::KLASTManager * manager);

      virtual QString filePath();
      virtual void setFilePath(QString text);
      virtual QString fileName();
      virtual QString code();
      virtual void setCodeAndExec(QString text, FabricCore::DFGExec *dfgExec);

      virtual bool event(QEvent * event);
      virtual void keyPressEvent(QKeyEvent * event);
      virtual void mouseDoubleClickEvent(QMouseEvent * event);
      virtual void mousePressEvent(QMouseEvent * event);
      virtual void mouseReleaseEvent(QMouseEvent * event);
      virtual void contextMenuEvent(QContextMenuEvent *event);
      virtual void paintEvent(QPaintEvent * event);

      virtual void updateSourceCode(bool updateAST = true);

      virtual void highlightLocation(const FabricServices::ASTWrapper::KLLocation * location);
      virtual void clearHighlightedLocations();

    protected:

      virtual void focusOutEvent(QFocusEvent * event);

    public slots:

      void contextMenuOpenDocs();
      void contextMenuOpenFile();
      void revertFile();
      void saveFile();

    private slots:
      void onTextChanged();
      void onBlockCountChanged(int);

    signals:

      void documentScrolled(unsigned int);
      void openKLFileRequested(KLDeclPtr);
      void newUnsavedChanged();
      void fileSaved();

    private:

      bool showPopup(bool forParen = false);
      bool hidePopup();

      unsigned int m_lineOffset;
      EditorConfig m_config;
      KLSyntaxHighlighter * m_highlighter;
      FabricServices::CodeCompletion::KLCodeAssistant * m_codeAssistant;
      QString m_filePath;
      const FabricServices::ASTWrapper::KLDecl * m_contextMenuDecl;
      bool m_hasUnsavedChanges;
      bool m_isHighlighting;
      QString m_lastCode;
      CodeCompletionPopup * m_popup;
      FabricCore::DFGExec *m_dfgExec;
    };

  };

};

#endif // __UI_KLEditor_KLSourceCodeWidget__
