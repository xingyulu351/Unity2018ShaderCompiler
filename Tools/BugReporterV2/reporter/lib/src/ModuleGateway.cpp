#include "ModuleGateway.h"
#include "NullModuleException.h"
#include "shims/logical/IsNull.h"

namespace ureport
{
namespace details
{
    class ModuleGateway : public ureport::ModuleGateway
    {
        Modules m_Modules;

        void AddModule(ModuleRef module)
        {
            if (IsNull(module))
                throw NullModuleException();
            m_Modules.push_back(module);
        }

        Modules GetAllModules() const
        {
            return m_Modules;
        }
    };
}

    ModuleGateway& GetModuleGateway()
    {
        static details::ModuleGateway gateway;
        return gateway;
    }
}
