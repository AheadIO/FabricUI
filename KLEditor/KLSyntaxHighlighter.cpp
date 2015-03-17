// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "KLSyntaxHighlighter.h"
#include "Helpers.h"

using namespace FabricUI::KLEditor;
using namespace FabricServices;

KLSyntaxHighlighter::KLSyntaxHighlighter(QTextDocument * document, ASTWrapper::KLASTManager * manager, const EditorConfig & config)
: QSyntaxHighlighter(document)
, CodeCompletion::KLSyntaxHighlighter(manager)
{
  m_config = config;
}

KLSyntaxHighlighter::~KLSyntaxHighlighter()
{
}

void KLSyntaxHighlighter::highlightBlock(const QString &text)
{
  if(!isEnabled())
    return;

  const std::vector<Format> & formats = getHighlightFormats(QStringToStl(document()->toPlainText()));

  QTextBlock block = currentBlock();
  int start = block.position();
  int length = block.length();

  for(size_t i=0;i<formats.size();i++)
  {
    if(formats[i].start + formats[i].length < start)
      continue;
    if(formats[i].start > start + length)
      continue;

    int localStart = formats[i].start - start;
    int localLength = formats[i].length;
    if(localStart < 0)
    {
      localLength += localStart;
      localStart = 0;
    }

    if(localLength < 0)
      continue;
    else if(localLength + localStart > length)
      localLength = length - localStart;

    QTextCharFormat textFormat;
    switch(formats[i].token)
    {
      case Token_Comment:
      {
        textFormat = m_config.formatForComment;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_String:
      {
        textFormat = m_config.formatForString;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_Number:
      {
        textFormat = m_config.formatForNumber;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_Keyword:
      {
        textFormat = m_config.formatForKeyword;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_Type:
      {
        textFormat = m_config.formatForType;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_Constant:
      {
        textFormat = m_config.formatForConstant;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_Function:
      case Token_Method:
      {
        textFormat = m_config.formatForMethod;
        setFormat(localStart, localLength, textFormat);
        break;
      }
      case Token_Error:
      case Token_Highlight:
      {
        if(formats[i].token == Token_Error)
          textFormat = m_config.formatForError;
        else
          textFormat = m_config.formatForHighlight;

        for(int j=localStart;j<localStart+localLength;j++)
        {
          QTextCharFormat currFormat = format(j);
          currFormat.merge(textFormat);
          setFormat(j, 1, currFormat);
        }
        break;
      }
    }
  }

  setCurrentBlockState(0);

  // int startIndex = 0;
  // if (previousBlockState() != 1)
  //   startIndex = commentStartExpression.indexIn(text);

  // while (startIndex >= 0)
  // {
  //   int endIndex = commentEndExpression.indexIn(text, startIndex);
  //   int commentLength;
  //   if (endIndex == -1)
  //   {
  //     setCurrentBlockState(1);
  //     commentLength = text.length() - startIndex;
  //   }
  //   else
  //   {
  //     commentLength = endIndex - startIndex
  //       + commentEndExpression.matchedLength();
  //   }
  //   setFormat(startIndex, commentLength, multiLineCommentFormat);
  //   startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
  // }
}
