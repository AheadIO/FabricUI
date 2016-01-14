#pragma once

#include <FabricUI/ValueEditor/BaseModelItem.h>
#include <FabricUI/ValueEditor/QVariantRTVal.h>

#include <vector>

class ItemMetadata;
class PortItemMetadata;
//////////////////////////////////////////////////////////////////////////
// The Root-level model item for 
// 

namespace FabricUI
{

  namespace ModelItems
  {
    typedef std::vector<BaseModelItem*> ChildVec;
    class ExecModelItem : public BaseModelItem
    {
    protected:

      FabricCore::DFGExec m_exec;
      ChildVec m_children;

    public:
      ExecModelItem( FabricCore::DFGExec exec );
      ~ExecModelItem();

      size_t NumChildren();

      ChildVec::iterator GetChildItrBegin() { return m_children.begin(); }
      ChildVec::iterator GetChildItrEnd() { return m_children.end(); }

      virtual BaseModelItem* GetChild( QString childName );
      virtual BaseModelItem* GetChild( int index );

      virtual int ChildIndex( QString name );
      virtual QString ChildName( int i );

      virtual QString GetName();;

      virtual ItemMetadata* GetMetadata();

      virtual QVariant GetValue();;

      virtual void argInserted( int index, const char* name, const char* type );

      void argTypeChanged( int index, const char* name, const char* newType );

      void argRemoved( int index, const char* name );

      virtual void onViewValueChanged(
        QVariant const& var,
        bool commit
        );

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

    };

    //////////////////////////////////////////////////////////////////////////
    // Specialize Exec for special case exterior
    class BindingModelItem : public ExecModelItem
    {
    private:
      FabricCore::DFGBinding m_binding;

    public:
      BindingModelItem( FabricCore::DFGBinding binding );

      BaseModelItem* GetChild( QString childName );
      virtual void argInserted( int index, const char* name, const char* type );
    };

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

      PortItemMetadata* m_metadata;

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

    //////////////////////////////////////////////////////////////////////////
    // Specialization for accessing ports that are also args
    class ArgModelItem : public PortModelItem
    {
    private:
      FabricCore::DFGBinding m_binding;

    public:

      ArgModelItem( const FabricCore::DFGBinding& binding, QString portName );

      virtual QVariant GetValue();

      virtual void onViewValueChanged( QVariant const&, bool ) /*override*/;

      virtual ItemMetadata* GetMetadata() /*override*/;
    };

    //////////////////////////////////////////////////////////////////////////
    // Access graph variables
    class VarModelItem : public ExecModelItem
    {
    private:
      QString m_name;
      std::string m_cname;

    public:
      VarModelItem( FabricCore::DFGExec& exec, const char* name );
      ~VarModelItem();

      virtual size_t NumChildren() /*override*/;

      virtual BaseModelItem* GetChild( int childIndex ) /*override*/;

      virtual QString GetName() /*override*/;

      virtual int GetInOut() /*override*/;

      virtual QVariant GetValue() /*override*/;

      virtual bool hasDefault() /*override*/;

      virtual void resetToDefault() /*override*/;

      virtual void onViewValueChanged( QVariant const&, bool ) /*override*/;

    };
  }
}
