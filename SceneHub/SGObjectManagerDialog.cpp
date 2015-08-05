// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#include "SGObjectManagerDialog.h"
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QColorDialog>
#include <iostream>
#include <vector>
#include "macros.h"
#include <Core/Build.h>
using namespace std;
using namespace FabricUI;
using namespace FabricUI::SceneHub;


SGObjectManagerDialog::SGObjectManagerDialog() {
  std::cerr << "SGObjectManagerDialog::SGObjectManagerDialog" << std::endl;
  setMouseTracking( true );
}

bool SGObjectManagerDialog::init(QWidget* parent, FabricCore::Client *client, FabricCore::RTVal testObject) {
  if(!client) return false;
  m_parent = parent;
  m_client = client;
  m_testObject = testObject;

  std::vector<FabricCore::RTVal> klColorRGBA(4); 
  klColorRGBA[0] = FabricCore::RTVal::ConstructFloat32(*m_client, 1.0f);
  klColorRGBA[1] = FabricCore::RTVal::ConstructFloat32(*m_client, 1.0f);
  klColorRGBA[2] = FabricCore::RTVal::ConstructFloat32(*m_client, 1.0f);
  klColorRGBA[3] = FabricCore::RTVal::ConstructFloat32(*m_client, 1.0f);
  m_color = FabricCore::RTVal::Construct(*m_client, "Color", 4, &klColorRGBA[0]);

  FABRIC_TRY_RETURN("SGObjectManagerDialog::setLight", false, m_viewport = m_testObject.callMethod("Viewport2", "getViewport", 0, 0); );
  if(m_parent) move( m_parent->mapToGlobal( m_parent->rect().center() ) );
  setMouseTracking( true );
  show();
  return true;
}

// ***********

void SGObjectManagerDialog::showColorDialog() {
  QColor color = QColorDialog::getColor(Qt::yellow, this);
  FABRIC_TRY("SGObjectManagerDialog::showColorDialog",
    std::vector<FabricCore::RTVal> klColorRGBA(4); 
    klColorRGBA[0] = FabricCore::RTVal::ConstructFloat32(*m_client, float(color.redF()));
    klColorRGBA[1] = FabricCore::RTVal::ConstructFloat32(*m_client, float(color.greenF()));
    klColorRGBA[2] = FabricCore::RTVal::ConstructFloat32(*m_client, float(color.blueF()));
    klColorRGBA[3] = FabricCore::RTVal::ConstructFloat32(*m_client, 1.0f);
    m_color = FabricCore::RTVal::Construct(*m_client, "Color", 4, &klColorRGBA[0]);
  );
}

void SGObjectManagerDialog::mouseMoveEvent( QMouseEvent * e) {}

void SGObjectManagerDialog::mouseReleaseEvent( QMouseEvent * e) {}

void SGObjectManagerDialog::closeEvent( QCloseEvent *e ) {
  e->accept();
  if (!m_parent) return;
  QMouseEvent me( QEvent::MouseButtonRelease, QPoint(0,0), Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
  QCoreApplication::sendEvent( m_parent, &me );
}

 