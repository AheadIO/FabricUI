// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_KLSyntaxHighlighter__
#define __UI_KLEditor_KLSyntaxHighlighter__

#include <QtCore/QHash>
#include <QtGui/QSyntaxHighlighter>
#include <QtGui/QTextDocument>

#include "EditorConfig.h"
#include "Helpers.h"

#include <ASTWrapper/KLASTManager.h>
#include <CodeCompletion/KLSyntaxHighlighter.h>

namespace FabricUI
{

  namespace KLEditor
  {

    class KLSyntaxHighlighter : public QSyntaxHighlighter, public FabricServices::CodeCompletion::KLSyntaxHighlighter
    {
      Q_OBJECT

    public:
    
      KLSyntaxHighlighter(QTextDocument * document, FabricServices::ASTWrapper::KLASTManager * manager, const EditorConfig & config = EditorConfig());
      virtual ~KLSyntaxHighlighter();

    protected:

      virtual void highlightBlock(const QString &text);

    private:
      EditorConfig m_config;
    };

  };

};

#endif // __UI_KLEditor_KLSyntaxHighlighter__
