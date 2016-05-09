// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_CodeCompletionPopup__
#define __UI_KLEditor_CodeCompletionPopup__

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QWidget>
#include "EditorConfig.h"
#include "CodeCompletionEntry.h"

#include <ASTWrapper/KLASTClient.h>
#include <ASTWrapper/KLType.h>

namespace FabricUI
{

  namespace KLEditor
  {

    class CodeCompletionPopup : public QWidget, public FabricServices::ASTWrapper::KLASTClient
    {
      Q_OBJECT
    
    public:
    
      CodeCompletionPopup(
        QWidget * parent, 
        std::string search, 
        FabricServices::ASTWrapper::KLASTManager * astManager, 
        const FabricServices::ASTWrapper::KLType * klType, 
        const EditorConfig & config = EditorConfig()
      );

      CodeCompletionPopup(
        QWidget * parent, 
        FabricServices::ASTWrapper::KLASTManager * astManager, 
        const FabricServices::ASTWrapper::KLFunction * klFunction, 
        const EditorConfig & config = EditorConfig()
      );

      virtual ~CodeCompletionPopup();

      bool isFunctionParameterPopup() const;
      std::string search() const;
      QString currentPrefix() const;
      QString currentSuffix() const;
      QString currentLabel() const;
      QString currentDesc() const;
      const FabricServices::ASTWrapper::KLDecl * currentDecl() const;
      unsigned int index() const;
      void setIndex(unsigned int value);
      void updateSearch();

      virtual bool setASTManager(FabricServices::ASTWrapper::KLASTManager * manager);

    public slots:

      void charAddedToSearch(char c);
      void charRemovedFromSearch();
      void setPosFromCursor(QPoint cursorGlobalPos);

    signals:

      void triggered(CodeCompletionEntry *);

    private:

      void init();
 
      EditorConfig m_config;
      std::string m_search;
      std::string m_resolvedSearch;
      std::vector<const FabricServices::ASTWrapper::KLDecl *> m_decls;
      std::vector<const FabricServices::ASTWrapper::KLDecl *> m_visibleDecls;
      const FabricServices::ASTWrapper::KLType * m_klType;
      const FabricServices::ASTWrapper::KLFunction * m_klFunction;
      unsigned int m_maxResults;
      unsigned int m_index;
    };

  };

};

#endif // __UI_KLEditor_CodeCompletionPopup__
