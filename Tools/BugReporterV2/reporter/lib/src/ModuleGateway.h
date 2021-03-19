#pragma once

#include <memory>
#include <vector>

namespace ureport
{
    class Module;

    class ModuleGateway
    {
    public:
        typedef std::shared_ptr<Module> ModuleRef;
        typedef std::vector<ModuleRef> Modules;

        virtual void AddModule(ModuleRef module) = 0;

        virtual Modules GetAllModules() const = 0;

        virtual ~ModuleGateway() {}
    };

    ModuleGateway& GetModuleGateway();
}
