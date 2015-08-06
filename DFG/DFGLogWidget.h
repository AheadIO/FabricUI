// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGLogWidget__
#define __UI_DFG_DFGLogWidget__

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

      DFGLogWidget( const DFGConfig & config = DFGConfig() );
      virtual ~DFGLogWidget();

      static void log(const char * message);

      static void callback(
        void * userData, 
        FabricCore::ReportSource source,
        FabricCore::ReportLevel level,
        char const * stringData,
        uint32_t stringLength
        );
      static void setLogFunc(DFGController::LogFunc func);

    public slots:

      void clear();
      void showContextMenu( QPoint const &pos );

    private:

      QPlainTextEdit * m_text;
      DFGConfig m_config;

      static DFGController::LogFunc s_logFunc;
      static std::vector<std::string> sUnconsumedMessages;
      static std::vector<DFGLogWidget*> sLogWidgets;
    };

  };

};

#endif // __UI_DFG_DFGLogWidget__
