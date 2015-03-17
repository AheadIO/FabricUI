// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_KLEditor_CodeCompletionEntry__
#define __UI_KLEditor_CodeCompletionEntry__

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
#include <QtGui/QWidget>
#include "EditorConfig.h"

namespace FabricUI
{

  namespace KLEditor
  {

    class CodeCompletionEntry : public QWidget
    {
      Q_OBJECT
    
    public:
    
      CodeCompletionEntry(QWidget * parent, QString prefix, QString suffix, QString desc, const EditorConfig & config = EditorConfig());
      virtual ~CodeCompletionEntry();

      virtual QString label() const;
      virtual QString prefix() const;
      virtual QString suffix() const;
      virtual QString desc() const;

      virtual bool active() const;
      virtual void setActive(bool state);

      virtual void paintEvent(QPaintEvent * event);

    private:
      EditorConfig m_config;
      QFontMetrics * m_metrics;
      QString m_prefix;
      QString m_suffix;
      QString m_desc;
      bool m_active;
    };

  };

};

#endif // __UI_KLEditor_CodeCompletionEntry__
