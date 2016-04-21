#ifndef FABRICUI_UTIL_QTSIGNALBLOCKER_H
#define FABRICUI_UTIL_QTSIGNALBLOCKER_H

#include <QtCore/QObject>

namespace FabricUI {
  namespace Util {
    class QTSignalBlocker
    {
    public:
      QTSignalBlocker( QObject *obj )
        : m_obj( obj )
        , m_old( obj->blockSignals( true ) )
      {
      }

      ~QTSignalBlocker()
      {
        m_obj->blockSignals( m_old );
      }

    private:
      QObject *m_obj;
      bool m_old;
    };
  }
}

#endif // FABRICUI_UTIL_QTSIGNALBLOCKER_H
