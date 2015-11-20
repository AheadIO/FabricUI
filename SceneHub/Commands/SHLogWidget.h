/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __FABRICUI_SceneHub_LogWidget__
#define __FABRICUI_SceneHub_LogWidget__

#include <string>
#include <FabricCore.h>
#include <QtGui/QWidget>
#include <QtGui/QPlainTextEdit>


namespace FabricUI
{
  namespace SceneHub
  {
    class SHLogWidget : public QWidget
    {
      Q_OBJECT

      public:
        /// Constructor.
        SHLogWidget();

        /// Destructor --> Remove all the SHLogWidget
        virtual ~SHLogWidget();

        /// Log the content of message.
        static void log(const char * message);

        /// Logs a message from the KL core.
        /// \params userData A pointer to specific user data.
        /// \params source The source (file) form where the log is called.
        /// \params level Level (depth) of logging.
        /// \params stringData A pointer to the meassage to log.
        /// \params stringLength The length of the string message.
        static void callback(
          void * userData, 
          FabricCore::ReportSource source,
          FabricCore::ReportLevel level,
          char const * stringData,
          uint32_t stringLength);

        /// Gets the key event.
        /// \param event The Key event
        virtual void keyPressEvent(QKeyEvent * event);

      public slots:
        /// Show the context menu --> clear the content
        void clear();
        /// Clear the window content
        void showContextMenu( QPoint const &pos );

      private:
        QPlainTextEdit * m_text;
        static std::vector<std::string> sUnconsumedMessages;
        static std::vector<SHLogWidget*> sLogWidgets;
    };
  };
};

#endif // __FABRICUI_SceneHub_LogWidget__
