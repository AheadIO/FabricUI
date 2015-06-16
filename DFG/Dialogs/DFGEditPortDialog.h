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

      DFGEditPortDialog(QWidget * parent, FabricCore::Client & client, bool showPortType, const DFGConfig & dfgConfig = DFGConfig());
      virtual ~DFGEditPortDialog();

      QString portType() const;
      void setPortType(QString value);
      QString title() const;
      void setTitle(QString value);
      QString dataType() const;
      void setDataType(QString value);
      QString extension() const;
      void setExtension(QString value);
      bool native() const;
      void setNative(bool value);
      bool hidden() const;
      void setHidden();
      bool opaque() const;
      void setOpaque();
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

    private slots:

      void onRangeToggled(int state);
      void onComboToggled(int state);

    private:

      QComboBox * m_portTypeCombo;
      QLineEdit * m_titleEdit;
      DFGRegisteredTypeLineEdit * m_dataTypeEdit;
      DFGExtensionLineEdit * m_extensionEdit;
      QCheckBox * m_native;
      QComboBox * m_visibilityCombo;
      QCheckBox * m_hasRange;
      QLineEdit * m_rangeMin;
      QLineEdit * m_rangeMax;
      QCheckBox * m_hasCombo;
      QLineEdit * m_combo;

    };

  };

};

#endif // __UI_DFG_DFGEditPortDialog__
