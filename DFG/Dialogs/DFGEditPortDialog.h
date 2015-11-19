// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGEditPortDialog__
#define __UI_DFG_DFGEditPortDialog__

#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>
#include <QtGui/QCheckBox>
#include <QtGui/QShowEvent>

#include "DFGBaseDialog.h"
#include "DFGRegisteredTypeLineEdit.h"
#include "DFGExtensionLineEdit.h"

namespace FabricUI
{
  namespace DFG
  {

    class DFGEditPortDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGEditPortDialog(
        QWidget * parent, 
        FabricCore::Client & client, 
        bool showPortType, 
        bool topGraphPort, 
        const DFGConfig & dfgConfig = DFGConfig(),
        bool setAlphaNum = false
      );
      virtual ~DFGEditPortDialog();

      QString portType() const;
      void setPortType(QString value);
      QString title() const;
      void setTitle(QString value);
      QString dataType() const;
      void setDataType(QString value);
      QString extension() const;
      void setExtension(QString value);
      bool hidden() const;
      void setHidden();
      bool opaque() const;
      void setOpaque();
      bool persistValue() const;
      void setPersistValue( bool value );
      bool hasRange() const;
      void setHasRange(bool value);
      float rangeMin() const;
      void setRangeMin(float value);
      float rangeMax() const;
      void setRangeMax(float value);
      bool hasCombo() const;
      void setHasCombo(bool value);
      QStringList comboValues() const;
      void setComboValues(QStringList value);

      virtual void showEvent(QShowEvent * event);

      /// Filter the QLineEdit text with the regexFilter
      void alphaNumicStringOnly();
      /// Filter the QLineEdit text with the setRegexFilter
      void setRegexFilter(QString regex);

    private slots:

      void onRangeToggled(int state);
      void onComboToggled(int state);

    private:

      QComboBox * m_portTypeCombo;
      QLineEdit * m_titleEdit;
      DFGRegisteredTypeLineEdit * m_dataTypeEdit;
      DFGExtensionLineEdit * m_extensionEdit;
      QComboBox * m_visibilityCombo;
      QCheckBox * m_persistValue;
      QCheckBox * m_hasRange;
      QLineEdit * m_rangeMin;
      QLineEdit * m_rangeMax;
      QCheckBox * m_hasCombo;
      QLineEdit * m_combo;

    };

  };

};

#endif // __UI_DFG_DFGEditPortDialog__