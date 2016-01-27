// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#include "CodeCompletionPopup.h"

#include <FTL/MapCharToLower.h>
#include <FTL/StrRef.h>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QVBoxLayout>

using namespace FabricServices::ASTWrapper;
using namespace FabricUI::KLEditor;

CodeCompletionPopup::CodeCompletionPopup(
  QWidget * parent, 
  std::string search, 
  KLASTManager * astManager, 
  const KLType * klType, 
  const EditorConfig & config)  
: QWidget(parent)
, KLASTClient(astManager)
{
  m_config = config;
  m_klType = klType;
  m_klFunction = NULL;
  m_search = search;
  m_exactMatch = false;

  init();
  updateSearch();
}

CodeCompletionPopup::CodeCompletionPopup(
  QWidget * parent, 
  KLASTManager * astManager, 
  const KLFunction * klFunction, 
  const EditorConfig & config)  
: QWidget(parent)
, KLASTClient(astManager)
{
  m_config = config;
  m_klType = NULL;
  m_klFunction = klFunction;
  m_search = m_klFunction->getName();
  m_exactMatch = true;

  if(m_klFunction->isOfDeclType(KLDeclType_Method))
  {
    std::string thisType = ((const KLMethod*)m_klFunction)->getThisType();
    m_klType = getASTManager()->getKLTypeByName(thisType.c_str(), m_klFunction);
  }

  init();
  updateSearch();
}

void CodeCompletionPopup::init()
{
  m_maxResults = 15;

  if(parent() == NULL)
  {
    setWindowFlags(Qt::SplashScreen);
    setAttribute(Qt::WA_ShowWithoutActivating);
  }

  int margins = m_config.codeCompletionMargins;
  setContentsMargins(margins, margins, margins, margins);

  QVBoxLayout * layout = new QVBoxLayout(this);
  setLayout(layout);

  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(m_config.codeCompletionSpacing);
}

CodeCompletionPopup::~CodeCompletionPopup()
{
}

bool CodeCompletionPopup::isFunctionParameterPopup() const
{
  return m_klFunction != NULL;
}

std::string CodeCompletionPopup::search() const
{
  return m_search;
}

QString CodeCompletionPopup::currentPrefix() const
{
  QVBoxLayout * vbox = (QVBoxLayout *)layout();
  if(int(m_index) < vbox->count())
  {
    return ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->prefix();
  }
  return "";
}

QString CodeCompletionPopup::currentSuffix() const
{
  QVBoxLayout * vbox = (QVBoxLayout *)layout();
  if(int(m_index) < vbox->count())
  {
    return ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->suffix();
  }
  return "";
}

QString CodeCompletionPopup::currentLabel() const
{
  QVBoxLayout * vbox = (QVBoxLayout *)layout();
  if(int(m_index) < vbox->count())
  {
    return ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->label();
  }
  return "";
}

QString CodeCompletionPopup::currentDesc() const
{
  QVBoxLayout * vbox = (QVBoxLayout *)layout();
  if(int(m_index) < vbox->count())
  {
    return ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->desc();
  }
  return "";
}

const KLDecl * CodeCompletionPopup::currentDecl() const
{
  if(m_index < m_visibleDecls.size())
  {
    m_visibleDecls[m_index];
  }
  return NULL;
}

unsigned int CodeCompletionPopup::index() const
{
  return m_index;
}

void CodeCompletionPopup::setIndex(unsigned int value)
{
  if(isFunctionParameterPopup())
    return;
  if(value == m_index)
    return;
  if(value >= m_visibleDecls.size())
    return;

  QVBoxLayout * vbox = (QVBoxLayout *)layout();
  
  ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->setActive(false);
  m_index = value;
  ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->setActive(true);
}

bool CodeCompletionPopup::setASTManager(KLASTManager * manager)
{
  if(KLASTClient::setASTManager(manager))
  {
    m_klType = NULL;
    m_decls.clear();
    m_visibleDecls.clear();
    m_resolvedSearch = "";
    updateSearch();
    return true;
  }

  return false;
}

void CodeCompletionPopup::charAddedToSearch(char c)
{
  if(isFunctionParameterPopup())
    return;
  if((isalnum(c) || c == '_') && c != '.')
  {
    m_search += c;
    updateSearch();
  }
}

void CodeCompletionPopup::charRemovedFromSearch()
{
  if(isFunctionParameterPopup())
    return;
  if(m_search.length() > 0)
  {
    m_search = m_search.substr(0, m_search.length()-1);
    updateSearch();
  }
}

void CodeCompletionPopup::setPosFromCursor(QPoint cursorGlobalPos)
{
  QFontMetrics metrics(m_config.codeCompletionFont);
  QPoint pos = cursorGlobalPos + QPoint(0, int(metrics.height() * 0.5));
  if(m_search.length() > 0)
  {
    pos -= QPoint(metrics.width(m_search.c_str()), 0);
  }
  if(isFunctionParameterPopup())
  {
    pos -= QPoint(metrics.width("("), 0);
  }

  move(pos);
}

struct MatchPrefixForCodeCompletion
{
  MatchPrefixForCodeCompletion( FTL::StrRef search, bool exact )
    : _search( search )
    , _exact( exact )
  {
  }

  bool operator()( FTL::StrRef::IT &it, FTL::StrRef::IT itEnd ) const
  {
    FTL::StrRef::IT itOrig = it;

    FTL::StrRef::IT searchIT = _search.begin();
    FTL::StrRef::IT const searchITEnd = _search.end();
    while ( searchIT != searchITEnd && it != itEnd )
    {
      char chToMatch = *searchIT++;
      if ( (isalnum( chToMatch ) || chToMatch == '_')
        && _toLower( *it++ ) != _toLower( chToMatch ) )
      {
        it = itOrig;
        return false;
      }
    }

    if ( it != itEnd && !_exact )
    {
      it = itOrig;
      return false;
    }

    return true;
  }
private:
  FTL::StrRef _search;
  bool _exact;
  FTL::MapCharToLower _toLower;
};

void CodeCompletionPopup::updateSearch()
{
  if(m_search == "" && m_klType == NULL)
    return;
  if(m_resolvedSearch == m_search && m_search != "")
    return;

  const KLDecl * prevDecl = NULL;
  if(m_visibleDecls.size() > m_index)
    prevDecl = m_visibleDecls[m_index];
  m_index = 0;

  // clear all previous items
  QVBoxLayout * vbox = (QVBoxLayout *)layout();
  QLayoutItem* item;
  while ( ( item = vbox->takeAt( 0 ) ) != NULL )
  {
    delete item->widget();
    delete item;
  }

  if(!hasASTManager())
    return;

  // update our search even
  if(m_resolvedSearch == "")
  {
    m_decls.clear();
    m_visibleDecls.clear();
  }

  if(m_decls.size() == 0)
  {
    // two main searches, either within a type, or in global space
    if(m_klType)
    {
      if(m_klType->isOfDeclType(KLDeclType_Struct))
      {
        const KLStruct * klStruct = (const KLStruct *)m_klType;

        for(size_t i=0;i<klStruct->getMemberCount();i++)
        {
          const KLMember * member = klStruct->getMember(i);
          if(member->isInternal())
            continue;
          m_decls.push_back(member);
        }

      }

      std::vector<const KLMethod *> methods = m_klType->getMethods(true, false);
      for(size_t i=0;i<methods.size();i++)
        m_decls.push_back(methods[i]);
    }
    else
    {
      // global namespace, so all types, constants, aliases and functions
      std::vector<const KLConstant*> constants = getASTManager()->getConstants();
      std::vector<const KLType*> types = getASTManager()->getTypes();
      std::vector<const KLAlias*> aliases = getASTManager()->getAliases();
      std::vector<const KLFunction*> functions = getASTManager()->getFunctions();

      for(size_t i=0;i<constants.size();i++)
        m_decls.push_back(constants[i]);

      for(size_t i=0;i<types.size();i++)
        m_decls.push_back(types[i]);

      for(size_t i=0;i<aliases.size();i++)
        m_decls.push_back(aliases[i]);

      for(size_t i=0;i<functions.size();i++)
        m_decls.push_back(functions[i]);

    }
  }

  unsigned int numResults = 0;
  m_visibleDecls.clear();
  for(size_t i=0;i<m_decls.size();i++)
  {
    std::string label;
    std::string desc;
    if(m_decls[i]->isOfDeclType(KLDeclType_Constant))
    {
      const KLConstant * decl = (const KLConstant *)m_decls[i];
      label = decl->getName();
      desc = "(constant)";
    }
    else if(m_decls[i]->isOfDeclType(KLDeclType_Type))
    {
      const KLType * decl = (const KLType *)m_decls[i];
      label = decl->getName();
      desc = "(";
      desc += decl->getKLType();
      desc += ")";
    }
    else if(m_decls[i]->isOfDeclType(KLDeclType_Alias))
    {
      const KLAlias * decl = (const KLAlias *)m_decls[i];
      label = decl->getNewUserName();
      desc = "(alias of ";
      desc += decl->getOldUserName();
      desc += ")";
    }
    else if(m_decls[i]->isOfDeclType(KLDeclType_Function))
    {
      const KLFunction * decl = (const KLFunction *)m_decls[i];
      label = decl->getName();

      desc = decl->getKLCode(false, false, false, false);
      if(decl->isOfDeclType(KLDeclType_Method))
        desc += " (method)";
      else
        desc += " (function)";
    }
    else if(m_decls[i]->isOfDeclType(KLDeclType_Member))
    {
      const KLMember * decl = (const KLMember *)m_decls[i];
      label = decl->getName();
      desc += "("+decl->getType()+" member)";
    }

    FTL::StrRef labelStr = label;
    FTL::StrRef::IT s = labelStr.begin();
    FTL::StrRef::IT e = labelStr.end();
    MatchPrefixForCodeCompletion matcher( m_search, m_exactMatch );

    if(m_search == "" || matcher( s, e ))
    {
      m_visibleDecls.push_back(m_decls[i]);

      CodeCompletionEntry * entry =
        new CodeCompletionEntry(
          this,
          label.substr(0, m_search.length()).c_str(),
          label.substr(m_search.length(), 10000).c_str(),
          desc.c_str(),
          m_config
          );
      vbox->addWidget(entry);

      numResults++;
      if(numResults == m_maxResults)
        break;
    }
  }

  m_resolvedSearch = m_search;

  for(size_t i=0;i<m_visibleDecls.size();i++)
  {
    if(m_visibleDecls[i] == prevDecl)
    {
      m_index = i;
      break;
    }
  }

  if(numResults == 0)
    hide();
  else
    show();

  if(numResults > 0 && !isFunctionParameterPopup())
    ((CodeCompletionEntry*)vbox->itemAt(m_index)->widget())->setActive(true);

  adjustSize();
}
