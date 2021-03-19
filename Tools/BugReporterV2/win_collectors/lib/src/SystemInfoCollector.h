#pragma once

#include "attachment/Attachment.h"
#include "attachment/AttachmentFactory.h"
#include "attachment_preview/AttachmentTextPreview.h"
#include "common/StringUtils.h"
#include "reporter/Collector.h"
#include "reporter/ReportDraft.h"
#include "shims/attribute/GetCString.h"
#include "shims/logical/IsEmpty.h"
#include "unity_bridge/SystemInfo.h"

#include <Windows.h>
#include <CommCtrl.h>
#include <d3d9.h>
#include <Gl/Gl.h>
#include <TlHelp32.h>
#include <cstdint>
#include <sstream>
#include <vector>

namespace ureport
{
namespace collectors
{
    struct COMInterfaceReleaser
    {
        void operator()(IUnknown* iface)
        {
            iface->Release();
        }
    };

    struct WindowDestroyer
    {
        void operator()(HWND* window)
        {
            ::DestroyWindow(*window);
        }
    };

    struct OpenGLContextDeleter
    {
        void operator()(HGLRC* context)
        {
            ::wglMakeCurrent(nullptr, nullptr);
            ::wglDeleteContext(*context);
        }
    };

    struct HandleCloser
    {
        void operator()(HANDLE* handle)
        {
            ::CloseHandle(*handle);
        }
    };

    class SystemInfoCollector : public ureport::Collector
    {
        std::shared_ptr<AttachmentFactory> m_AttachmentFactory;

    public:
        SystemInfoCollector(std::shared_ptr<AttachmentFactory> attachmentFactory)
            : m_AttachmentFactory(attachmentFactory)
        {
        }

        void Collect(ReportDraft& report) const override
        {
            std::shared_ptr<Attachment> attachment = m_AttachmentFactory->CreateTextAttachment(
                GetSystemInfo(), "System Info");
            attachment->GetTextPreview()->SetRemovable(false);
            report.Attach(attachment);
        }

    private:
        std::string GetSystemInfo() const
        {
            std::stringstream info;
            WriteSystemInfo(info);
            info << std::endl;
            WriteDirect3dInfo(info);
            info << std::endl;
            WriteOpenGLInfo(info);
            info << std::endl;
            WriteLocaleInfo(info);
            info << std::endl;
            WriteRunningProcessesInfo(info);
            return info.str();
        }

        void WriteSystemInfo(std::ostream& info) const
        {
            using namespace unity_bridge;
            info << "System:\n"
            << "\tOS: " << GetOperatingSystem() << "\n"
            << "\tCPU: " << GetProcessorType() << ", " << GetProcessorCount() << "\n";
            WriteMemoryInfo(info);
        }

        void WriteMemoryInfo(std::ostream& info) const
        {
            MEMORYSTATUSEX memStatus = {0};
            memStatus.dwLength = sizeof(memStatus);
            ::GlobalMemoryStatusEx(&memStatus);
            const size_t megabyte = 1048576;
            info << "\tPhysical RAM: "
            << memStatus.ullTotalPhys / megabyte << " MB\n";
            info << "\tUser-mode addressable RAM: "
            << memStatus.ullTotalVirtual / megabyte << " MB\n";
        }

        void WriteDirect3dInfo(std::ostream& info) const
        {
            std::unique_ptr<IDirect3D9, COMInterfaceReleaser> direct3d(
                Direct3DCreate9(D3D_SDK_VERSION));
            D3DADAPTER_IDENTIFIER9 adapter = {0};
            direct3d->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapter);
            auto const monitor = direct3d->GetAdapterMonitor(D3DADAPTER_DEFAULT);
            auto const memorySize = unity_bridge::GetVideoMemorySizeMB(monitor);
            info << "Direct3D 9:\n"
            << "\tRenderer: " << adapter.Description << "\n"
            << "\tVendorID: " << adapter.VendorId << "\n"
            << "\tDriver: " << adapter.Driver << " " << GetDriverVersionText(adapter) << "\n"
            << "\tVRAM: " << memorySize << "\n";
        }

        std::string GetDriverVersionText(const D3DADAPTER_IDENTIFIER9& adapter) const
        {
            std::stringstream version;
            version << HIWORD(adapter.DriverVersion.HighPart) << "."
            << LOWORD(adapter.DriverVersion.HighPart) << "."
            << HIWORD(adapter.DriverVersion.LowPart) << "."
            << LOWORD(adapter.DriverVersion.LowPart);
            return version.str();
        }

        void WriteOpenGLInfo(std::ostream& info) const
        {
            std::unique_ptr<HWND, WindowDestroyer> window(
                new HWND(::CreateWindowEx(0, WC_STATIC, nullptr,
                    (WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS),
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                    nullptr, nullptr, nullptr, nullptr)));
            PIXELFORMATDESCRIPTOR pixelFormatDescr = {0};
            pixelFormatDescr.nSize = sizeof(pixelFormatDescr);
            pixelFormatDescr.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
            pixelFormatDescr.nVersion = 1;
            pixelFormatDescr.iPixelType = PFD_TYPE_RGBA;
            pixelFormatDescr.cColorBits = 32;
            pixelFormatDescr.cDepthBits = 16;
            pixelFormatDescr.iLayerType = PFD_MAIN_PLANE;
            auto const deviceContext = ::GetDC(*(window.get()));
            auto const pixelFormat = ::ChoosePixelFormat(deviceContext, &pixelFormatDescr);
            auto const setPixelResult = ::SetPixelFormat(deviceContext, pixelFormat, &pixelFormatDescr);
            auto const glContext = std::unique_ptr<HGLRC, OpenGLContextDeleter>(new HGLRC(
                ::wglCreateContext(deviceContext)));
            auto const switchCntxResult  = ::wglMakeCurrent(deviceContext, *(glContext.get()));
            auto const memorySize = unity_bridge::GetVideoMemorySizeMB(
                ::MonitorFromWindow(::GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY));
            info << "OpenGL:\n"
            << "\tVersion: " << reinterpret_cast<const char*>(::glGetString(GL_VERSION)) << "\n"
            << "\tVendor: " << reinterpret_cast<const char*>(::glGetString(GL_VENDOR)) << "\n"
            << "\tRenderer: " << reinterpret_cast<const char*>(::glGetString(GL_RENDERER)) << "\n"
            << "\tDriver: " << unity_bridge::GetDisplayDriverName() << " " <<
            unity_bridge::GetDisplayDriverVersion() << "\n"
            << "\tVRAM: " << memorySize << "\n"
            << "\tExtensions: " << reinterpret_cast<const char*>(::glGetString(GL_EXTENSIONS)) << "\n";
        }

        void WriteLocaleInfo(std::ostream& info) const
        {
            info << "Locales:\n";
            info << "\tUser locale: " << GetLocaleName(::GetUserDefaultLCID()) << "\n";
            info << "\tSystem locale: " << GetLocaleName(::GetSystemDefaultLCID()) << "\n";
        }

        std::string GetLocaleName(const LCID& locale) const
        {
            const uint32_t bufferSize = 32;
            std::vector<wchar_t> language(bufferSize);
            ::GetLocaleInfoW(locale, LOCALE_SISO639LANGNAME, &language[0], bufferSize);
            std::vector<wchar_t> country(bufferSize);
            ::GetLocaleInfoW(locale, LOCALE_SISO3166CTRYNAME, &country[0], bufferSize);
            std::stringstream localeName;
            localeName << Narrow(GetCString(language)) << "-" << Narrow(GetCString(country));
            return localeName.str();
        }

        void WriteRunningProcessesInfo(std::ostream& info) const
        {
            auto const snapshot = std::unique_ptr<HANDLE, HandleCloser>(new HANDLE(
                ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)));
            info << "Running processes:\n";
            PROCESSENTRY32W process = {0};
            process.dwSize = sizeof(process);
            for (::Process32FirstW(*snapshot.get(), &process);
                 ::Process32Next(*snapshot.get(), &process);)
            {
                auto const module = GetProcessModule(process.th32ProcessID);
                if (IsEmpty(module.szModule))
                    continue;
                info << "\t" << Narrow(module.szModule) << " (" << process.th32ProcessID << "):" << "\n"
                << "\t\tpath: " << Narrow(module.szExePath) << "\n"
                << "\t\tcompany: " << unity_bridge::GetCompanyName(module.szExePath) << "\n"
                << "\t\tproduct: " << unity_bridge::GetProductName(module.szExePath) << "\n"
                << "\t\tdescription: " << unity_bridge::GetFileDescription(module.szExePath) << "\n"
                << "\t\tproduct version: " << unity_bridge::GetProductVersion(module.szExePath) << "\n"
                << "\t\tfile version: " << unity_bridge::GetFileVersion(module.szExePath) << "\n";
            }
        }

        MODULEENTRY32W GetProcessModule(const DWORD process) const
        {
            auto const snapshot = std::unique_ptr<HANDLE, HandleCloser>(new HANDLE(
                ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process)));
            MODULEENTRY32W module = {0};
            module.dwSize = sizeof(module);
            ::Module32First(*snapshot.get(), &module);
            return module;
        }
    };
}
}
