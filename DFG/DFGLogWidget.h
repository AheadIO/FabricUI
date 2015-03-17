// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGLogWidget__
#define __UI_DFG_DFGLogWidget__

#include <DFGWrapper/Host.h>
#include <DFGWrapper/Executable.h>

#include <QtGui/QWidget>
#include <QtGui/QPlainTextEdit>

#include "DFGConfig.h"
#include "DFGController.h"

namespace FabricUI
{

  namespace DFG
  {

    class DFGLogWidget : public QWidget
    {
      Q_OBJECT

    public:

      DFGLogWidget(QWidget * parent, const DFGConfig & config = DFGConfig());
      virtual ~DFGLogWidget();

      static void callback(void * userData, char const * stringData, uint32_t stringLength);

    public slots:

      void copy();

    private:

      QPlainTextEdit * m_text;
      DFGConfig m_config;

      static std::vector<std::string> sUnconsumedMessages;
      static std::vector<DFGLogWidget*> sLogWidgets;
    };

  };

};

#endif // __UI_DFG_DFGLogWidget__
