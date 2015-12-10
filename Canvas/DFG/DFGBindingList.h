// Copyright 2010-2015 Fabric Software Inc. All rights reserved.

#ifndef __UI_DFG_DFGBindingList__
#define __UI_DFG_DFGBindingList__

#include <QtCore/QObject>
#include <FabricCore.h>
#include <vector>
#include <map>
#include <FTL/CStrRef.h>

namespace FabricUI
{

  namespace DFG
  {

    class DFGBindingList : public QObject
    {
      Q_OBJECT

    public:

      DFGBindingList(
        QObject * parent,
        FabricCore::Client &client,
        FabricCore::DFGHost &host
        );
      virtual ~DFGBindingList();

      unsigned int getCount() const { return (unsigned int)m_bindings.size(); }
      FabricCore::DFGBinding & getBinding(unsigned int index) { return m_bindings[index].binding; }
      char const * getBindingTitle(unsigned int index);

      unsigned int getActiveBindingIndex() const { return m_activeIndex; }

    signals:

      void activeBindingChanged(int index);
      void bindingsChanged();
      void bindingRenamed(int index, const char * name);
      void bindingCreated(int index);

    public slots:

      void clear();
      void setActiveBindingIndex(unsigned int index);
      void setBindingTitle(unsigned int index, FTL::CStrRef name);
      void addBinding(FTL::CStrRef name);
      void addBindingFromFile(FTL::CStrRef filePath);
      void saveBindingToFile(int index, FTL::CStrRef filePath);
      void removeBinding(int index);

      void setRTVal(const char * name, const FabricCore::RTVal & value, bool onlyOnce = false);
      bool execute();

      void prepBindingForExecution(int index);
      void prepAllBindingForExecution();

      std::string persistToJSON();
      void restoreFromJSON(FTL::CStrRef json);
      bool persistToFile(FTL::CStrRef filePath);
      bool restoreFromFile(FTL::CStrRef filePath);

    private:

      std::string getUniqueName(FTL::CStrRef name);

      FabricCore::Client & m_client;
      FabricCore::DFGHost & m_host;

      struct BindingInfo
      {
        bool dirty; // todo: talk to Peter about this. executing the binding shouldn't cause such a slowdown
        FabricCore::DFGBinding binding;
        std::map<std::string, bool> rtValsSet;
        std::map<std::string, bool> containsPort;

        BindingInfo()
        {
          dirty = false;
        }
      };

      std::vector<BindingInfo> m_bindings;
      unsigned int m_activeIndex;
      bool m_blockSignals;
    };

  };

};

#endif // __UI_DFG_DFGBindingList__
