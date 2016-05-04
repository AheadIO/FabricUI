// Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

#ifndef __UI_GraphView_Port__
#define __UI_GraphView_Port__

#include <QtCore/QMimeData>
#include <QtGui/QGraphicsWidget>
#include <QtGui/QColor>
#include <QtGui/QPen>
#include <QtGui/QGraphicsSceneMouseEvent>

#include <FTL/CStrRef.h>

#include "PortType.h"
#include "PortLabel.h"
#include "PinCircle.h"
#include "ConnectionTarget.h"
#include "GraphicItemTypes.h"

namespace FabricUI
{

  namespace GraphView
  {
    // forward decl
    class Graph;
    class Pin;
    class SidePanel;

    class Port : public ConnectionTarget
    {
      Q_OBJECT

      friend class Graph;
      friend class SidePanel;

    public:

      Port(
        SidePanel * parent,
        FTL::StrRef name,
        PortType portType,
        FTL::StrRef dataType,
        QColor color,
        FTL::StrRef label = FTL::StrRef()
        );
      virtual ~Port() {}

      virtual int type() const { return QGraphicsItemType_Port; }

      SidePanel *sidePanel()
        { return m_sidePanel; }
      SidePanel const *sidePanel() const
        { return m_sidePanel; }

      Graph *graph();
      Graph const *graph() const;

      PinCircle *circle()
        { return m_circle; }
      PinCircle const *circle() const
        { return m_circle; }

      FTL::CStrRef name() const
        { return m_name; }
      QString nameQString() const
        { return QString::fromUtf8( m_name.data(), m_name.size() ); }
      void setName( FTL::CStrRef name );

      virtual std::string path() const;

      virtual char const * label() const;
      void setLabel(char const * n);
      virtual QColor color() const;
      void setColor(QColor color);
      virtual PortType portType() const;
      unsigned int index() const { return m_index; }

      virtual FTL::CStrRef dataType() const
        { return m_dataType; }
      virtual void setDataType(FTL::CStrRef dataType);

      virtual bool highlighted() const;
      virtual void setHighlighted(bool state = true);

      virtual bool canConnectTo(
        ConnectionTarget * other,
        std::string &failureReason
        ) const;
      
      virtual TargetType targetType() const { return TargetType_Port; }
      virtual QPointF connectionPos(PortType pType) const;

      bool allowEdits() const
        { return m_allowEdits; }
      void disableEdits()
        { m_allowEdits = false; }

      static QString const MimeType;

      class MimeData : public QMimeData
      {
        typedef QMimeData Parent;

      public:

        MimeData( Port *port )
          : m_port( port )
          {}

        virtual bool hasFormat( QString const &mimeType) const;

        virtual QStringList formats() const;

        Port *port() const
          { return m_port; }

      protected:
        
        virtual QVariant retrieveData(
          QString const &mimeType,
          QVariant::Type type
          ) const;

      private:

        Port *m_port;
      };

    signals:

      void positionChanged();

      void contentChanged();

    protected:

      void setIndex(unsigned id) { m_index = id; }

      virtual void mousePressEvent( QGraphicsSceneMouseEvent * event );

    private:

      void init();

      SidePanel * m_sidePanel;
      std::string m_name;
      PortType m_portType;
      std::string m_labelCaption;
      QColor m_color;
      std::string m_dataType;
      bool m_highlighted;
      TextContainer * m_label;
      PinCircle * m_circle;
      unsigned int m_index;
      bool m_allowEdits;
      QPointF m_dragStartPosition;
    };

  };

};

#endif // __UI_GraphView_Port__

