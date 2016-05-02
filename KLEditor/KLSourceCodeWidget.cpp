// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "KLSourceCodeWidget.h"

#include <QtGui/QTextEdit>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QScrollBar>
#include <QtGui/QToolTip>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QDesktopServices>
#include <QtCore/QUrl>

#include <limits.h>

using namespace FabricServices;
using namespace FabricUI::KLEditor;

KLSourceCodeWidget::KLSourceCodeWidget(QWidget * parent, FabricServices::ASTWrapper::KLASTManager * manager, const EditorConfig & config)
: QPlainTextEdit(parent)
, ASTWrapper::KLASTClient(manager)
, m_highlighter(NULL)
, m_codeAssistant(NULL)
{
  m_config = config;
  m_lineOffset = UINT_MAX;
  m_filePath = "empty.kl";
  m_isHighlighting = false;
  // m_popup = NULL;

  document()->setDefaultFont(config.codeFont);
  setWordWrapMode(QTextOption::NoWrap);
  QFontMetrics metrics(config.codeFont);
  setTabStopWidth(metrics.width(QString().fill(' ', config.codeTabWidth)));
    
  QPalette p = palette();
  p.setColor(QPalette::Base, config.codeBackgroundColor);
  p.setColor(QPalette::WindowText, config.codeFontColor);
  p.setColor(QPalette::Text, config.codeFontColor);
  setPalette(p);

  m_highlighter = new KLSyntaxHighlighter(document(), manager);
  m_codeAssistant = new CodeCompletion::KLCodeAssistant(m_highlighter);

  setContentsMargins(0, 0, 0, 0);

  m_hasUnsavedChanges = false;

  QObject::connect(this, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
  QObject::connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(onBlockCountChanged(int)));
}

KLSourceCodeWidget::~KLSourceCodeWidget()
{
  delete(m_highlighter);
  delete(m_codeAssistant);
  // if(m_popup)
  //   delete(m_popup);
}

bool KLSourceCodeWidget::setASTManager(ASTWrapper::KLASTManager * manager)
{
  if(ASTWrapper::KLASTClient::setASTManager(manager))
  {
    m_highlighter->setASTManager(manager);
    m_codeAssistant->setASTManager(manager);
    // if(m_popup)
    //   m_popup->setASTManager(manager);
    return true;
  }
  return false;
}

QString KLSourceCodeWidget::code()
{
  return toPlainText();
}

void KLSourceCodeWidget::setCode(QString text)
{
  m_highlighter->setEnabled(false);
  setPlainText(text);
  m_highlighter->setEnabled(true);
  updateSourceCode();
  m_isHighlighting = true;
  m_highlighter->rehighlight();
  m_isHighlighting = false;
  m_lastCode = code();
  m_hasUnsavedChanges = false;
}

QString KLSourceCodeWidget::filePath()
{
  return m_filePath;
}

void KLSourceCodeWidget::setFilePath(QString text)
{
  m_filePath = text;
}

QString KLSourceCodeWidget::fileName()
{
  QString result = m_filePath;
  result.replace("\\", "/");
  QStringList parts = result.split("/");
  return parts[parts.count()-1];
}

bool KLSourceCodeWidget::event(QEvent * event)
{
  if(event->type() == QEvent::ToolTip)
  {
    QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);
    QTextCursor cursor = cursorForPosition(helpEvent->pos());

    uint32_t line, column;
    m_codeAssistant->cursorToLineAndColumn(cursor.position(), line, column);

    std::string toolTipText;
    const ASTWrapper::KLFile * file = m_codeAssistant->getKLFile();
    if(file)
    {
      std::vector<const ASTWrapper::KLError*> errors = file->getErrors();
      for(size_t i=0;i<errors.size();i++)
      {
        if(errors[i]->getLine() == int(line))
        {
          toolTipText = errors[i]->getDesc();
          break;        
        }
      }
    }

    if(toolTipText.length() == 0)
    {
      const ASTWrapper::KLDecl * decl = m_codeAssistant->getDeclAtCursor(cursor.position());
      if(decl)
      {
        if(decl->isOfDeclType(ASTWrapper::KLDeclType_Commented))
        {
          const ASTWrapper::KLCommented * commented = (const ASTWrapper::KLCommented *)decl;
          const ASTWrapper::KLComment * comment = commented->getComments();
          
          std::string brief = comment->removeRstRoles(comment->getBrief().c_str());
          std::string plainText = comment->removeRstRoles(comment->getPlainText().c_str());
          // std::string example = comment->getExample();

          if(brief.length() > 0)
          {
            if(toolTipText.length() > 0)
              toolTipText += "\n";
            toolTipText += brief;
          }
          else if(plainText.length() > 0)
          {
            if(toolTipText.length() > 0)
              toolTipText += "\n";
            toolTipText += plainText;
          }
        }

        if(decl->isOfDeclType(ASTWrapper::KLDeclType_Struct))
        {
          const ASTWrapper::KLStruct * klStruct = (const ASTWrapper::KLStruct *)decl;
          if(klStruct->getMemberCount() > 0)
          {
            if(toolTipText.length() > 0)
              toolTipText += "\n";
            toolTipText += "Members:\n";
            for(uint32_t i=0;i<klStruct->getMemberCount();i++)
            {
              const ASTWrapper::KLMember * member = klStruct->getMember(i);
              if(member->isInternal())
                continue;

              CodeCompletion::KLTypeDesc desc(member->getType());
              toolTipText += "\n  "+desc.getBaseType()+" "+member->getName()+desc.getArrayModifier();

            }
          }                
        }

        if(decl->isOfDeclType(ASTWrapper::KLDeclType_Constant))
        {
          const ASTWrapper::KLConstant * constant = (const ASTWrapper::KLConstant *)decl;
          if(toolTipText.length() > 0)
            toolTipText += "\n";
          toolTipText += "const "+constant->getType()+" "+constant->getName()+";";
        }

        if(decl->isOfDeclType(ASTWrapper::KLDeclType_Function))
        {
          const ASTWrapper::KLFunction * function = (const ASTWrapper::KLFunction *)decl;
          if(toolTipText.length() > 0)
            toolTipText += "\n";
          toolTipText += function->getKLCode();
        }
      }
    }

    if(toolTipText.length() > 0)
      QToolTip::showText(mapToGlobal(helpEvent->pos()), toolTipText.c_str());
  }

  else if (event->type() == QEvent::MouseButtonPress)
  {
    event->accept();
    return true;
  }

  return QPlainTextEdit::event(event);
}

void KLSourceCodeWidget::keyPressEvent(QKeyEvent * event)
{
  // bool needNewPopup = false;
  // bool newPopupForParen = false;

  if((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return || event->key() == Qt::Key_Tab) /*&& m_popup*/)
  {
    // if(!m_popup->isFunctionParameterPopup())
    // {
    //   QString prefix = m_popup->currentPrefix();
    //   QString suffix = m_popup->currentSuffix();

    //   QTextCursor cursor = textCursor();
    //   cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor, prefix.length());
    //   cursor.removeSelectedText();
    //   insertPlainText(prefix+suffix);
    //   hidePopup();
    //   return;
    // }
    // else
    //   hidePopup();
  }

  if(event->key() == Qt::Key_Tab)
  {
    if(!event->modifiers().testFlag(Qt::ShiftModifier))
      insertPlainText("  ");
    return;
  }
  if(event->key() == Qt::Key_Backtab)
  {
    std::string klCode = QStringToStl(code());
    if(klCode.length() > 3)
    {
      int pos = textCursor().position();
      std::string posStr;

      // remove all previous lines
      size_t find = klCode.rfind('\n', pos);
      if(find != std::string::npos && find < klCode.length()-3)
      {
        if(klCode[find+1] == ' ' && klCode[find+2] == ' ')
        {
          klCode = klCode.substr(0, find+1) + klCode.substr(find+3, klCode.length());
          setPlainText(klCode.c_str());
          QTextCursor cursor = textCursor();
          cursor.setPosition(pos-2);
          setTextCursor(cursor);
        }
      }
    }
    return;
  }
  else if(event->key() == Qt::Key_Period && m_config.editorAlwaysShowCodeCompletion)
  {
    // needNewPopup = true;
    // newPopupForParen = false;
  }
  else if(event->key() == Qt::Key_ParenLeft)
  {
    // hidePopup();
    if(m_config.editorAlwaysShowCodeCompletion)
    {
      // needNewPopup = true;
      // newPopupForParen = true;
    }
  }
  else if(event->key() == Qt::Key_ParenRight)
  {
    // hidePopup();
  }
  else if(event->key() == Qt::Key_Escape)
  {
    // if(hidePopup())
    //   return;
  }
  else if(event->key() == Qt::Key_Backspace)
  {
    // if(m_popup)
    // {
    //   if(m_popup->search().length() == 0)
    //   {
    //     m_popup->hide();
    //     m_popup->deleteLater();
    //     m_popup = NULL;        
    //   }
    //   else
    //     m_popup->charRemovedFromSearch();
    // }
  }
  else if(event->key() == Qt::Key_Up)
  {
    // if(m_popup)
    // {
    //   unsigned int index = m_popup->index();
    //   if(index > 0)
    //     m_popup->setIndex(index-1);
    //   return;
    // }
  }
  else if(event->key() == Qt::Key_Down)
  {
    // if(m_popup)
    // {
    //   unsigned int index = m_popup->index();
    //   m_popup->setIndex(index+1);
    //   return;
    // }
  }
  else if(event->key() == Qt::Key_Left || event->key() == Qt::Key_Right)
  {
    // hidePopup();
  }
  else if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return)
  {
    std::string klCode = QStringToStl(code());
    if(klCode.length() > 0)
    {
      int pos = textCursor().position();
      std::string posStr;

      // remove all previous lines
      int find = klCode.find('\n');
      while(find < pos && find >= 0)
      {
        klCode = klCode.substr(find + 1);
        pos -= find + 1;
        find = klCode.find('\n');
      }

      // figure out how many spaces to insert
      unsigned int initialPos = pos;
      pos = 0;
      std::string prefix;
      while((unsigned int) pos < klCode.length())
      {
        if(klCode.at(pos) != ' ')
          break;
        prefix += " ";
        pos++;
      }

      // remove unrequired spaces
      while(prefix.length() > 0 && initialPos < klCode.length() && klCode.at(initialPos) == ' ')
      {
        prefix = prefix.substr(1);
        initialPos++;
      }

      prefix = "\n" + prefix;
      insertPlainText(prefix.c_str());
      return;
    }
  }
  else if(event->key() == Qt::Key_F5)
  {
    updateSourceCode();
  }
  else if(event->key() == Qt::Key_S)
  {
    if(event->modifiers().testFlag(Qt::ControlModifier))
    {
      saveFile();
    }
  }
  else if(event->key() == Qt::Key_Space)
  {
    // if(event->modifiers().testFlag(Qt::ControlModifier))
    // {
    //   int pos = textCursor().position();
    //   std::string charAtCursor = m_codeAssistant->getCharAtCursor(pos);
    //   bool forParen = false;
    //   if(charAtCursor.length() > 0)
    //     forParen = charAtCursor[0] == '(';
    //   if(showPopup(forParen))
    //     return;
    // }
    // else if(m_popup)
    // {
    //   if(!m_popup->isFunctionParameterPopup())
    //     hidePopup();
    // }
  }

  // if(m_popup)
  // {
  //   if(needNewPopup)
  //     hidePopup();
  //   else
  //   {
  //     QString text = event->text();
  //     if(text.length() > 0)
  //       m_popup->charAddedToSearch(QStringToStl(text)[0]);
  //   }
  // }

  QPlainTextEdit::keyPressEvent(event);

  m_isHighlighting = true;
  m_highlighter->rehighlight();
  m_isHighlighting = false;

  // if(needNewPopup)
  //   showPopup(newPopupForParen);
}

void KLSourceCodeWidget::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = createStandardContextMenu();

  QTextCursor cursor = cursorForPosition(event->pos());
  m_contextMenuDecl = m_codeAssistant->getDeclAtCursor(cursor.position());
  if(m_contextMenuDecl)
  {
    menu->addSeparator();

    QString docPrefix;
    if(m_contextMenuDecl->isOfDeclType(ASTWrapper::KLDeclType_Constant))
    {
      const ASTWrapper::KLConstant* decl = (const ASTWrapper::KLConstant*)m_contextMenuDecl;
      docPrefix = decl->getName().c_str();
    }

    if(m_contextMenuDecl->isOfDeclType(ASTWrapper::KLDeclType_Type))
    {
      const ASTWrapper::KLType* decl = (const ASTWrapper::KLType*)m_contextMenuDecl;
      docPrefix = decl->getName().c_str();
    }

    if(m_contextMenuDecl->isOfDeclType(ASTWrapper::KLDeclType_Function))
    {
      const ASTWrapper::KLFunction* decl = (const ASTWrapper::KLFunction*)m_contextMenuDecl;
      docPrefix = decl->getName().c_str();
    }

    if(docPrefix.length() > 0)
    {
      QObject::connect(menu->addAction("Open "+docPrefix+"'s Documentation"), SIGNAL(triggered()), this, SLOT(contextMenuOpenDocs()));
    }

    QString klFile = m_contextMenuDecl->getKLFile()->getFileName();
    QObject::connect(menu->addAction("Open "+klFile), SIGNAL(triggered()), this, SLOT(contextMenuOpenFile()));
  }
  menu->exec(event->globalPos());
  delete menu;
}

void KLSourceCodeWidget::paintEvent(QPaintEvent * event)
{
  if(int(m_lineOffset) != verticalScrollBar()->value())
  {
    m_lineOffset = verticalScrollBar()->value();
    emit documentScrolled(m_lineOffset);
  }

  QPlainTextEdit::paintEvent(event);
}

void KLSourceCodeWidget::updateSourceCode(bool updateAST)
{
  if(!hasASTManager())
    return;
  if(m_isHighlighting)
    return;
  if(m_codeAssistant == NULL)
    return;
  std::string klCode = QStringToStl(code());
  std::string klFile = QStringToStl(filePath());

  if(m_codeAssistant->getKLFile() == NULL)
  {
    std::vector<const ASTWrapper::KLExtension *> extensions = m_codeAssistant->getASTManager()->getExtensions();
    for(size_t i=0;i<extensions.size();i++)
    {
      std::vector<const ASTWrapper::KLFile *> files = extensions[i]->getFiles();
      for(size_t j=0;j<files.size();j++)
      {
        if(files[j]->getAbsoluteFilePath() == klFile)
        {
          m_codeAssistant->updateCurrentKLFile(files[j]);
          m_isHighlighting = true;
          m_highlighter->rehighlight();
          m_isHighlighting = false;
          return;
        }
      }
    }      
  }

  // [andrew 20150910] ignore 'updateAST' for now and always pass false since we
  // can't properly parse AST in DFG KL code, FE-5214
  if(m_codeAssistant->updateCurrentCodeAndFile(klCode, klFile, false))
  {
    m_isHighlighting = true;
    m_highlighter->rehighlight();
    m_isHighlighting = false;
  }
}

void KLSourceCodeWidget::contextMenuOpenDocs()
{
  if(!m_contextMenuDecl)
    return;

  QString urlSuffix;
  if(m_contextMenuDecl->isOfDeclType(ASTWrapper::KLDeclType_Constant))
  {
    const ASTWrapper::KLConstant* constant = (const ASTWrapper::KLConstant*)m_contextMenuDecl;
    urlSuffix = "KLExtensionsGuide/";
    urlSuffix += m_contextMenuDecl->getExtension()->getName();
    urlSuffix += "/constants.html?highlight=";
    urlSuffix += constant->getName().c_str();
  }
  else if(m_contextMenuDecl->isOfDeclType(ASTWrapper::KLDeclType_Constant))
  {
    urlSuffix = "KLExtensionsGuide/";
    urlSuffix += m_contextMenuDecl->getExtension()->getName();
    urlSuffix += "/constants.html";
  }
  else if(m_contextMenuDecl->isOfDeclType(ASTWrapper::KLDeclType_Type))
  {
    const ASTWrapper::KLType* type = (const ASTWrapper::KLType*)m_contextMenuDecl;
    urlSuffix = "KLExtensionsGuide/";
    urlSuffix += m_contextMenuDecl->getExtension()->getName();
    urlSuffix += "/";
    urlSuffix += type->getName().c_str();
    urlSuffix += ".html";
  }
  else if(m_contextMenuDecl->getDeclType() == ASTWrapper::KLDeclType_Function)
  {
    const ASTWrapper::KLFunction* function = (const ASTWrapper::KLFunction*)m_contextMenuDecl;
    urlSuffix = "KLExtensionsGuide/";
    urlSuffix += m_contextMenuDecl->getExtension()->getName();
    urlSuffix += "/functions.html?highlight=";
    urlSuffix += function->getName().c_str();
  }
  else if(m_contextMenuDecl->getDeclType() == ASTWrapper::KLDeclType_Method)
  {
    const ASTWrapper::KLMethod* method = (const ASTWrapper::KLMethod*)m_contextMenuDecl;
    const ASTWrapper::KLType* type = m_codeAssistant->getASTManager()->getKLTypeByName(method->getThisType().c_str(), method);
    if(type)
    {
      urlSuffix = "KLExtensionsGuide/";
      urlSuffix += type->getExtension()->getName();
      urlSuffix += "/";
      urlSuffix += type->getName().c_str();
      urlSuffix += ".html?highlight=";
      urlSuffix += method->getName().c_str();
    }
  }

  if(urlSuffix.length() > 0)
  {
    QUrl url = (m_config.docUrlPrefix + urlSuffix);
    QDesktopServices::openUrl(url);
  }
}

void KLSourceCodeWidget::contextMenuOpenFile()
{
  if(!m_contextMenuDecl)
    return;
  KLDeclPtr ptr;
  ptr.decl = m_contextMenuDecl;
  emit openKLFileRequested(ptr);
}

void KLSourceCodeWidget::revertFile()
{
  // see if the file exists
  std::string fileName = QStringToStl(m_filePath);
  FILE * file = fopen(fileName.c_str(), "rb");
  if(!file)
    return;

  fseek( file, 0, SEEK_END );
  int fileSize = ftell( file );
  rewind( file );

  char * codeBuffer = (char*) malloc(fileSize + 1);
  codeBuffer[fileSize] = '\0';

  fread(codeBuffer, fileSize, 1, file);
  fclose(file);

  QString klCode;
  klCode += codeBuffer;
  free(codeBuffer);

  setCode(klCode);
}

void KLSourceCodeWidget::saveFile()
{
  // see if the file exists
  std::string fileName = QStringToStl(m_filePath);
  FILE * file = fopen(fileName.c_str(), "rb");
  if(!file)
    return;
  fclose(file);

  // see if the file is writable
  file = fopen(fileName.c_str(), "wb");
  if(!file)
    return;

  std::string klCode = QStringToStl(code());
  fwrite(klCode.c_str(), klCode.length(), 1, file);
  fclose(file);

  m_hasUnsavedChanges = false;
  emit fileSaved();
}

void KLSourceCodeWidget::onTextChanged()
{
  if(m_isHighlighting)
    return;

  if(!m_hasUnsavedChanges)
  {
    QString currentCode = code();
    if(m_lastCode != currentCode && !m_lastCode.isEmpty())
    {
      emit newUnsavedChanged();
      m_hasUnsavedChanges = true;
      m_lastCode = currentCode;
    }
  }
  if(m_config.editorAutoRebuildAST)
  {
    updateSourceCode(true);
  }
}

void KLSourceCodeWidget::onBlockCountChanged(int)
{
  // if(m_config.editorAutoRebuildAST)
  // {
  //   updateSourceCode(true);
  // }
}

void KLSourceCodeWidget::highlightLocation(const ASTWrapper::KLLocation * location)
{
  uint32_t startPos, endPos;
  m_codeAssistant->lineAndColumnToCursor(location->getLine(), location->getColumn(), startPos);
  m_codeAssistant->lineAndColumnToCursor(location->getEndLine(), location->getEndColumn(), endPos);

  m_highlighter->highlight(startPos, endPos - startPos + 1);

  m_isHighlighting = true;
  m_highlighter->rehighlight();
  m_isHighlighting = false;
}

void KLSourceCodeWidget::clearHighlightedLocations()
{
  m_highlighter->clearHighlighting();

  m_isHighlighting = true;
  m_highlighter->rehighlight();
  m_isHighlighting = false;
}

// bool KLSourceCodeWidget::showPopup(bool forParen)
// {
//   updateSourceCode(false);

//   int pos = textCursor().position();

//   hidePopup();

//   if(forParen)
//   {
//     const ASTWrapper::KLDecl * decl = m_codeAssistant->getDeclAtCursor(pos);
//     if(decl == NULL)
//       return false;
//     if(!decl->isOfDeclType(ASTWrapper::KLDeclType_Function))
//       return false;

//     m_popup = new CodeCompletionPopup(NULL, getASTManager(), (const ASTWrapper::KLFunction*)decl, m_config);
//   }
//   else
//   {
//     std::string wordAtCursor = m_codeAssistant->getWordAtCursor(pos, forParen);
//     const ASTWrapper::KLType * klType = m_codeAssistant->getTypeAtCursor(pos, true);
//     if(wordAtCursor.length() == 0 && klType == NULL)
//       return false;

//     m_popup = new CodeCompletionPopup(NULL, wordAtCursor, getASTManager(), klType, m_config);
//   }

//   QPoint cursorPos = mapToGlobal(cursorRect().bottomLeft());
//   m_popup->setPosFromCursor(cursorPos);
//   m_popup->show();

//   return true;
// }

// bool KLSourceCodeWidget::hidePopup()
// {
//   if(m_popup)
//   {
//     m_popup->hide();
//     m_popup->deleteLater();
//     m_popup = NULL;
//     return true;
//   }
//   return false;
// }
