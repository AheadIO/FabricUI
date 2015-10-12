#ifndef __QT2OKLEVENT_H__
#define __QT2OKLEVENT_H__

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QPoint>

#include <FabricCore.h>

FabricCore::RTVal QtToKLMousePosition(QPoint pos, FabricCore::Client const& client, FabricCore::RTVal viewport);

FabricCore::RTVal QtToKLEvent(QEvent *event, FabricCore::Client const& client, FabricCore::RTVal viewport);

#endif
