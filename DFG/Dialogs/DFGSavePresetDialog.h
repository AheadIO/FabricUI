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

    class DFGSavePresetDialog : public DFGBaseDialog
    {
      Q_OBJECT

    public:

      DFGSavePresetDialog(
        QWidget * parent,
        FabricCore::DFGHost & host,
        QString name = "",
        const DFGConfig & dfgConfig = DFGConfig()
        );
      virtual ~DFGSavePresetDialog();

      QString name() const;
      QString location() const;

    public slots:
      void onItemSelected(FabricUI::TreeView::TreeItem * item);
      void onCustomContextMenuRequested(QPoint globalPos, FabricUI::TreeView::TreeItem * item);
      void onContextMenuAction(QAction * action);

    private:

      FabricCore::DFGHost m_dfgHost;
      QLineEdit * m_nameEdit;
      PresetTreeWidget * m_presetTree;
      QString m_contextPath;
      QString m_location;

    };

  };

};

#endif // __UI_DFG_DFGSavePresetDialog__
