#include "Setup.Configuration.h"
#include "ComPtr.h"
#include <iostream>
#include <string>

struct CoInitializer
{
    CoInitializer()
    {
        hr = CoInitialize(NULL);
    }

    ~CoInitializer()
    {
        if (SUCCEEDED(hr))
            CoUninitialize();
    }

    operator bool() const
    {
        return SUCCEEDED(hr);
    }

private:
    HRESULT hr;
};

struct SafeArrayHolder
{
    SafeArrayHolder() :
        m_SafeArray(NULL)
    {
    }

    ~SafeArrayHolder()
    {
        if (m_SafeArray != NULL)
            SafeArrayDestroy(m_SafeArray);
    }

    operator SAFEARRAY*() const
    {
        return m_SafeArray;
    }

    SAFEARRAY** operator&()
    {
        if (m_SafeArray != NULL)
        {
            SafeArrayDestroy(m_SafeArray);
            m_SafeArray = NULL;
        }

        return &m_SafeArray;
    }

private:
    SAFEARRAY* m_SafeArray;

    SafeArrayHolder(const SafeArrayHolder&);
    SafeArrayHolder& operator=(const SafeArrayHolder&);
};

template<typename T>
struct SafeArrayAccessHolder
{
    SafeArrayAccessHolder(SAFEARRAY* safeArray) :
        m_SafeArray(safeArray),
        m_Data(NULL)
    {
        m_Hr = SafeArrayAccessData(m_SafeArray, reinterpret_cast<void**>(&m_Data));
    }

    ~SafeArrayAccessHolder()
    {
        if (SUCCEEDED(m_Hr))
            SafeArrayUnaccessData(m_SafeArray);
    }

    operator bool() const
    {
        return SUCCEEDED(m_Hr);
    }

    T operator[](size_t index) const
    {
        return m_Data[index];
    }

private:
    SAFEARRAY* m_SafeArray;
    T* m_Data;
    HRESULT m_Hr;

    SafeArrayAccessHolder(const SafeArrayAccessHolder&);
    SafeArrayAccessHolder& operator=(const SafeArrayAccessHolder&);
};

struct BStrHolder
{
    BStrHolder() :
        m_Str(NULL)
    {
    }

    ~BStrHolder()
    {
        if (m_Str != NULL)
            SysFreeString(m_Str);
    }

    operator BSTR() const
    {
        return m_Str;
    }

    BSTR Get() const
    {
        return m_Str;
    }

    BSTR* operator&()
    {
        if (m_Str != NULL)
        {
            SysFreeString(m_Str);
            m_Str = NULL;
        }

        return &m_Str;
    }

private:
    BSTR m_Str;

    BStrHolder(const BStrHolder&);
    BStrHolder& operator=(const BStrHolder&);
};

#define ReturnIfFailed(hr, returnValue) do { if (FAILED(hr)) { return returnValue; } } while (false)
#define ContinueIfFailed(hr) { if (FAILED(hr)) { continue; } }
#define BreakIfFailed(hr) { if (FAILED(hr)) { break; } }

enum VisualStudioInstallState
{
    kVisualStudioNotInstalled = 0,
    kVisualStudioAndWorkloadInstalled = 1,
    kVisualStudioCommunityInstalled = 2,
    kVisualStudioProfessionalInstalled = 3,
    kVisualStudioEnterpriseInstalled = 4,
};

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 3)
    {
        std::cout << "Error:Expected 2 arguments, got " << (argc - 1) << std::endl;
        return -10;
    }

    const int visualStudioVersion = _wtoi(argv[1]); // 15 for Visual Studio 2017
    const wchar_t* const requiredWorkload = argv[2]; //  "Microsoft.VisualStudio.Workload.ManagedGame"

    CoInitializer coInit;

    if (!coInit)
        return -1;

    win::ComPtr<ISetupConfiguration> setupConfiguration;
    HRESULT hr = CoCreateInstance(__uuidof(SetupConfiguration), NULL, CLSCTX_ALL, __uuidof(ISetupConfiguration), &setupConfiguration);
    ReturnIfFailed(hr, kVisualStudioNotInstalled);

    win::ComPtr<ISetupHelper> setupHelper;
    hr = setupConfiguration.As(&setupHelper);
    ReturnIfFailed(hr, -2);

    win::ComPtr<IEnumSetupInstances> instanceEnumerator;
    hr = setupConfiguration->EnumInstances(&instanceEnumerator);
    ReturnIfFailed(hr, -3);

    ULONG instanceCount;
    win::ComPtr<ISetupInstance> instance;

    bool visualStudioEnterpriseInstalled = false;
    bool visualStudioProfessionalInstalled = false;
    bool visualStudioCommunityInstalled = false;

    std::wstring visualStudioEnterprisePath;
    std::wstring visualStudioProfessionalPath;
    std::wstring visualStudioCommunityPath;

    while (SUCCEEDED(instanceEnumerator->Next(1, &instance, &instanceCount)) && instanceCount > 0)
    {
        win::ComPtr<ISetupInstance2> instance2;
        hr = instance.As(&instance2);
        ContinueIfFailed(hr);

        ISetupPackageReference* productPackage;
        hr = instance2->GetProduct(&productPackage);
        ContinueIfFailed(hr);

        BStrHolder productString;
        hr = productPackage->GetId(&productString);
        ContinueIfFailed(hr);

        // Is instance Visual Studio?
        if (_wcsnicmp(L"Microsoft.VisualStudio.Product", productString, 30) != 0)
            continue;

        BStrHolder versionString;
        hr = instance2->GetInstallationVersion(&versionString);
        ContinueIfFailed(hr);

        ULONGLONG version;
        hr = setupHelper->ParseVersion(versionString, &version);
        ContinueIfFailed(hr);

        const ULONGLONG majorVersion = version >> 48;

        // Does Visual Studio version match requested version?
        if (majorVersion != visualStudioVersion)
            continue;

        InstanceState instanceState;
        hr = instance2->GetState(&instanceState);
        ContinueIfFailed(hr);

        // Does the instance installation path exists?
        if ((instanceState & eLocal) == eNone)
            continue;

        BStrHolder bstrInstallationPath;
        hr = instance2->GetInstallationPath(&bstrInstallationPath);
        ContinueIfFailed(hr);

        BStrHolder bstrProductPath;
        hr = instance2->GetProductPath(&bstrProductPath);
        ContinueIfFailed(hr);

        std::wstring visualStudioPath = bstrInstallationPath.Get();
        visualStudioPath += L"\\";
        visualStudioPath += bstrProductPath.Get();

        if (_wcsicmp(L"Microsoft.VisualStudio.Product.Enterprise", productString) == 0)
        {
            visualStudioEnterpriseInstalled = true;
            visualStudioEnterprisePath = visualStudioPath;
        }
        else if (_wcsicmp(L"Microsoft.VisualStudio.Product.Professional", productString) == 0)
        {
            visualStudioProfessionalInstalled = true;
            visualStudioProfessionalPath = visualStudioPath;
        }
        else if (_wcsicmp(L"Microsoft.VisualStudio.Product.Community", productString) == 0)
        {
            visualStudioCommunityInstalled = true;
            visualStudioCommunityPath = visualStudioPath;
        }
        else
            continue;

        SafeArrayHolder packagesArray;
        hr = instance2->GetPackages(&packagesArray);
        ContinueIfFailed(hr);

        UINT elementCount = 1;
        UINT dimensions = SafeArrayGetDim(packagesArray);
        if (dimensions == 0)
            continue;

        for (UINT i = 0; i < dimensions; i++)
        {
            LONG lBound;
            LONG uBound;

            hr = SafeArrayGetLBound(packagesArray, i + 1, &lBound);
            BreakIfFailed(hr);

            hr = SafeArrayGetUBound(packagesArray, i + 1, &uBound);
            BreakIfFailed(hr);

            elementCount *= (uBound - lBound + 1);
        }

        ContinueIfFailed(hr);

        SafeArrayAccessHolder<IUnknown*> packagesData(packagesArray);
        if (!packagesData)
            continue;

        const WCHAR wzType[] = L"Workload";
        const size_t cchType = sizeof(wzType) / sizeof(WCHAR) - 1;

        for (UINT i = 0; i < elementCount; i++)
        {
            win::ComPtr<ISetupPackageReference> package;
            hr = packagesData[i]->QueryInterface(__uuidof(package), &package);
            ContinueIfFailed(hr);

            BStrHolder bstrType;
            hr = package->GetType(&bstrType);
            ContinueIfFailed(hr);

            // Is this a workload?
            if (_wcsnicmp(wzType, bstrType, cchType) != 0)
                continue;

            BStrHolder packageId;
            hr = package->GetId(&packageId);
            ContinueIfFailed(hr);

            // std::wcout << "PackageID: " << packageId.Get() << std::endl;

            // Is this package our requested workload?
            if (_wcsicmp(packageId, requiredWorkload) != 0)
                continue;

            // Print Visual Studio devnenv.exe path to stdout
            std::wcout << visualStudioPath;

            return kVisualStudioAndWorkloadInstalled;
        }
    }

    // Handle cases where a Visual Studio version is installed, but the Unity workload for it is not.
    if (visualStudioEnterpriseInstalled)
    {
        std::wcout << visualStudioEnterprisePath;
        return kVisualStudioEnterpriseInstalled;
    }

    if (visualStudioProfessionalInstalled)
    {
        std::wcout << visualStudioProfessionalPath;
        return kVisualStudioProfessionalInstalled;
    }

    if (visualStudioCommunityInstalled)
    {
        std::wcout << visualStudioCommunityPath;
        return kVisualStudioCommunityInstalled;
    }

    return kVisualStudioNotInstalled;
}
