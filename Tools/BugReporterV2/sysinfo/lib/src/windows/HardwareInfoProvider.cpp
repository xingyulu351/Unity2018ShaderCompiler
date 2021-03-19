#include "common/StringUtils.h"
#include "DefaultHardwareInfoProvider.h"
#include "unity_bridge/SystemInfo.h"

#include <d3d9.h>
#include <string>
#include <memory>
#include <algorithm>

namespace ureport
{
namespace details
{
    class WindowsHardwareInfoProvider : public DefaultHardwareInfoProvider
    {
        struct COMInterfaceReleaser
        {
            void operator()(IUnknown* iface)
            {
                iface->Release();
            }
        };

        std::string GetBasicHardwareType() const
        {
            return unity_bridge::GetProcessorType();
        }

        std::string GetGraphicsHardwareType() const
        {
            const auto adapters = GetDisplayAdapters();
            return Join(adapters.begin(), adapters.end(), " ");
        }

        std::vector<std::string> GetDisplayAdapters() const
        {
            std::unique_ptr<IDirect3D9, COMInterfaceReleaser> direct3d(Direct3DCreate9(D3D_SDK_VERSION));
            std::vector<std::string> adapters(direct3d->GetAdapterCount());
            int adapterIndex = D3DADAPTER_DEFAULT;
            std::generate(adapters.begin(), adapters.end(), [&direct3d, &adapterIndex] () -> std::string
            {
                D3DADAPTER_IDENTIFIER9 identifier;
                direct3d->GetAdapterIdentifier(adapterIndex++, 0, &identifier);
                return identifier.Description;
            });
            return adapters;
        }
    };
}

    std::unique_ptr<HardwareInfoProvider> CreateHardwareInfoProvider()
    {
        return std::unique_ptr<HardwareInfoProvider>(new details::WindowsHardwareInfoProvider());
    }
}
