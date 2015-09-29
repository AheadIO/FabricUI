// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_ValueEditor_ColorValueWidget__
#define __UI_ValueEditor_ColorValueWidget__

#include <QtGui/QMouseEvent>
#include <QtGui/QColorDialog>

#include "ValueWidget.h"
#include "LineEdit.h"

namespace FabricUI
{

  namespace ValueEditor
  {
    class ColorValueWidget;

    class ColorPickerWidget : public QWidget
    {
      Q_OBJECT

    public:
      ColorPickerWidget(QWidget * parent);

      void setValueItem(ValueItem * item) { m_valueItem = item; }

      virtual void  mousePressEvent ( QMouseEvent * event );
      virtual void  paintEvent ( QPaintEvent * event );

      void set(float r, float g, float b, float a, bool silent = false)
      {
        m_colorR = r;
        m_colorG = g;
        m_colorB = b;
        m_colorA = a;
        update();
        if(!silent)
          emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
      }

      void setR(float value)
      {
        m_colorR = value; 
        update(); 
        emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
      }

      void setG(float value)
      {
        m_colorG = value; 
        update(); 
        emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
      }

      void setB(float value)
      {
        m_colorB = value; 
        update(); 
        emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
      }

      void setA(float value)
      {
        m_colorA = value; 
        update(); 
        emit colorChanged(m_colorR, m_colorG, m_colorB, m_colorA);
      }

      float getR() const { return m_colorR; }
      float getG() const { return m_colorG; }
      float getB() const { return m_colorB; }
      float getA() const { return m_colorA; }

      int getR_as8bit() const { return (int)(std::max(0.0f, std::min(255.0f, 255.0f * m_colorR))); }
      int getG_as8bit() const { return (int)(std::max(0.0f, std::min(255.0f, 255.0f * m_colorG))); }
      int getB_as8bit() const { return (int)(std::max(0.0f, std::min(255.0f, 255.0f * m_colorB))); }
      int getA_as8bit() const { return (int)(std::max(0.0f, std::min(255.0f, 255.0f * m_colorA))); }

      void setEnabled(bool flag) { m_enabled = flag; }

    signals:
      void colorChanged(float r, float g, float b, float a);

    // private slots:
    //   void colorDialogRejected();

    private:
      bool m_enabled;
      float m_colorR;
      float m_colorG;
      float m_colorB;
      float m_colorA;
      float m_prevColorR;
      float m_prevColorG;
      float m_prevColorB;
      float m_prevColorA;

      QColorDialog * m_dialog;
      bool m_dialogConnected;
      ValueItem * m_valueItem;
    };

    class ColorValueWidget : public ValueWidget
    {
      Q_OBJECT

    public:

      ColorValueWidget(QString label, QWidget * parent);
      virtual ~ColorValueWidget();

      ColorPickerWidget * colorPicker() { return m_colorPicker; }

      virtual void setValue(FabricCore::RTVal v);
      virtual void setEnabled(bool state);

      static TreeView::TreeEditorWidget * creator(QWidget * parent, TreeView::WidgetTreeItem * item);
      static bool canDisplay(TreeView::WidgetTreeItem * item);

      static FabricCore::RTVal genRtVal(ValueItem * item, const std::string & typeName, float r, float g, float b, float a);

    public slots:

      void onValueChangedInLineEdit();
      void onValueChangedColorPicker(float r, float g, float b, float a);
      void onBeginInteraction();
      void onEndInteraction();
      virtual void setFocusToFirstInput() { m_lineEditR->setFocus(); }

    private:

      FabricCore::RTVal genRtVal(float r, float g, float b, float a);

      ColorPickerWidget * m_colorPicker;
      std::string m_typeName;
      LineEdit * m_lineEditR;
      LineEdit * m_lineEditG;
      LineEdit * m_lineEditB;
      LineEdit * m_lineEditA;
    };

  };

};

#endif // __UI_ValueEditor_ColorValueWidget__
