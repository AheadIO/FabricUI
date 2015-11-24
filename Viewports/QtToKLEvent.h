#ifndef __QT2OKLEVENT_H__
#define __QT2OKLEVENT_H__

#include <QtCore/QObject>
#include <QtCore/QEvent>

#include <FabricCore.h>

FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::Client const& client, FabricCore::RTVal viewport);

#endif
