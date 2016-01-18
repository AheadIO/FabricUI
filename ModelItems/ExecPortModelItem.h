#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

namespace FabricUI
{

  namespace ModelItems
  {
    //////////////////////////////////////////////////////////////////////////
    // Basic ModelItem for accessing ports
    class ExecPortModelItem : public BaseModelItem
    {
    protected:
      FabricCore::DFGExec m_exec;

      QString m_name;
      // We store a duplicated C-style version of the
      // name for accessing the non-unicode Fabric core
      std::string m_cname;

      ItemMetadata* m_metadata;

    public:
      ExecPortModelItem( const FabricCore::DFGExec& exec, QString portName );
      ~ExecPortModelItem();

      // Every port has exactly 1 child - its RTValue
      size_t NumChildren();

      virtual BaseModelItem* GetChild( int i );

      virtual QString GetName();
      virtual void RenameItem( const char* name );
      virtual void OnItemRenamed( QString newName );

      virtual ItemMetadata* GetMetadata();
      virtual void SetMetadata( const char* key,
                                const char* value,
                                bool canUndo )/*override*/;

      virtual int GetInOut();

      virtual QVariant GetValue();

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    protected:

      virtual void SetValue(
        QVariant var,
        bool commit,
        QVariant valueAtInteractionBegin
        ) /*override*/;


    };
  }
}
