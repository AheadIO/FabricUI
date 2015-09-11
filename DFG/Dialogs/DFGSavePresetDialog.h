// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGSavePresetDialog__
#define __UI_DFG_DFGSavePresetDialog__

#include <QtGui/QLineEdit>
#include "DFGBaseDialog.h"
#include "../PresetTreeWidget.h"

namespace FabricUI
{
  namespace DFG
  {
    class DFGController;

    class DFGSavePresetDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGSavePresetDialog(
        QWidget * parent,
        DFGController *dfgController,
        QString name = "",
        const DFGConfig & dfgConfig = DFGConfig()
        );
      virtual ~DFGSavePresetDialog();

      QString name() const;
      QString location() const;

      // Filter the QLineEdit text with the regexFilter
      void alphaNumicStringOnly();
      // Filter the QLineEdit text with the setRegexFilter
      void setRegexFilter(QString regex);

    public slots:
      void onItemSelected(FabricUI::TreeView::TreeItem * item);
      void onCustomContextMenuRequested(QPoint globalPos, FabricUI::TreeView::TreeItem * item);
      void onContextMenuAction(QAction * action);

    private:

      DFGController *m_dfgController;
      QLineEdit * m_nameEdit;
      PresetTreeWidget * m_presetTree;
      QString m_contextPath;
      QString m_location;

    };

  };

};

#endif // __UI_DFG_DFGSavePresetDialog__
