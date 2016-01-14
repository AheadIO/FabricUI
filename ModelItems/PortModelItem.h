#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class PortModelItem : public BaseModelItem
    {
    protected:
      FabricCore::DFGExec m_exec;

      QString m_name;
      // We store a duplicated C-style version of the
      // name for accessing the non-unicode Fabric core
      std::string m_cname;

      ItemMetadata* m_metadata;

    public:
      PortModelItem( const FabricCore::DFGExec& exec, QString portName );
      ~PortModelItem();

      // Every port has exactly 1 child - its RTValue
      size_t NumChildren();

      virtual BaseModelItem* GetChild( int i );

      virtual QString GetName();

      virtual ItemMetadata* GetMetadata();

      virtual int GetInOut();

      virtual QVariant GetValue();

      virtual void onViewValueChanged( QVariant const&, bool ) /*override*/;

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    };
  }
}
