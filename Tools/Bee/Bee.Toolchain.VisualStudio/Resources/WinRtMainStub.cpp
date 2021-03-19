#include <stdio.h>
#include <io.h>
#include <Roapi.h>

// Don't include windows.h in "Windows.Applicationmodel.Core.h" for better compile times.
#define COM_NO_WINDOWS_H
#ifdef _XBOX_ONE
    #include <ole2.h>
#endif
#include <Windows.Applicationmodel.Core.h>

// Both wrl/event and wlr/module (for hstring) would be very useful here,
// but they add a lot of compile time and unlike wrl/implements they are rather easy to emulate here.
#include <wrl/implements.h>

extern "C"
{
    // Extern "C" has the nice sideeffect that we will link to main() properly even if it does not take any parameters.
    // Since __cdecl has always caller cleanup, we won't run into any issues calling main() with parameters even if it doesn't expect any.
    extern int main(int argc, char* argv[]);
}

namespace WinRT
{
    using namespace ABI::Windows::ApplicationModel::Core;
    using namespace ABI::Windows::ApplicationModel::Activation;
    using namespace ABI::Windows::UI::Core;
    using namespace Microsoft::WRL;

    const wchar_t* kStdoutFileName = L"bee_runner_stdout.txt";
    const wchar_t* kStderrFileName = L"bee_runner_stderr.txt";
    const wchar_t* kExitCodeFileName = L"bee_runner_exitcode.txt";

    struct RoInitializer
    {
        inline RoInitializer()
        {
            RoInitialize(RO_INIT_MULTITHREADED);
        }

        inline ~RoInitializer()
        {
            RoUninitialize();
        }
    };

    static inline const wchar_t* GetDirectoryForStandardOutput()
    {
#ifdef _XBOX_ONE
        return L"D:\\";
#else
        // GetTempPathW can return up to MAX_PATH+1 according to documentation
        static wchar_t buffer[MAX_PATH + 1 + sizeof('\0')];
        DWORD tempPathLength = GetTempPathW(sizeof(buffer) / sizeof(buffer[0]), buffer);
        buffer[tempPathLength] = '\0';
        return buffer;
#endif
    }

    static void CreateRedirectedHandle(FILE* stdFile, const wchar_t* fileNameOnDisk)
    {
        auto stdFd = _fileno(stdFile);
        HANDLE stdHandle = reinterpret_cast<HANDLE*>(_get_osfhandle(stdFd));

        if (stdHandle != INVALID_HANDLE_VALUE && !_isatty(stdFd))
            return;

        wchar_t path[MAX_PATH + sizeof('\0')];
        swprintf_s(path, L"%s%s", GetDirectoryForStandardOutput(), fileNameOnDisk);
        auto redirectedFile = _wfreopen(path, L"w+", stdFile);
        _get_osfhandle(_fileno(redirectedFile));
    }

    class Application : public Microsoft::WRL::RuntimeClass<RuntimeClassFlags<WinRtClassicComMix>, IFrameworkViewSource, IFrameworkView>
    {
    private:
        ComPtr<ICoreWindow> m_Window;
        EventRegistrationToken m_eventRegistrationOnActivate;
        int m_MainReturnValue;

        // Using a std::vector costs us way too much compile time here.
        char** m_argv;
        size_t m_argc;
        size_t m_argvCapacity;

        volatile bool m_Done;

    public:
        Application()
            : m_MainReturnValue(0)
            , m_argv(nullptr)
            , m_argc(0)
            , m_argvCapacity(0)
            , m_Done(false)
        {
            char* moduleFileName = (char*)malloc(MAX_PATH);
            GetModuleFileNameA(nullptr, moduleFileName, MAX_PATH);
            PushBackArgv(moduleFileName);
        }

        ~Application()
        {
            for (size_t i = 0; i < m_argc; ++i)
                free(m_argv[i]);
            free(m_argv);
        }

        void PushBackArgv(char* arg)
        {
            ++m_argc;
            if (m_argc >= m_argvCapacity)
            {
                m_argvCapacity *= 2;
                if (m_argvCapacity == 0)
                    m_argvCapacity = 64;
                m_argv = static_cast<char**>(realloc(m_argv, m_argvCapacity * sizeof(char**)));
            }
            m_argv[m_argc - 1] = arg;
        }

        virtual HRESULT STDMETHODCALLTYPE CreateView(IFrameworkView** viewProvider) override
        {
            AddRef();
            *viewProvider = this;
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE Initialize(ICoreApplicationView* applicationView) override
        {
            using OnActivatedHandler = __FITypedEventHandler_2_Windows__CApplicationModel__CCore__CCoreApplicationView_Windows__CApplicationModel__CActivation__CIActivatedEventArgs;
            typedef HRESULT (Application::*OnActivatedCallbackFunc)(ICoreApplicationView*, IActivatedEventArgs*);

            struct OnActivatedCallback : RuntimeClass<RuntimeClassFlags<Delegate>, OnActivatedHandler>
            {
                OnActivatedCallback(Application* handler, OnActivatedCallbackFunc method) : m_Handler(handler), m_Method(method)
                {
                }

                virtual HRESULT STDMETHODCALLTYPE Invoke(ICoreApplicationView* applicationView, IActivatedEventArgs* args) override
                {
                    return (m_Handler->*m_Method)(applicationView, args);
                }

                Application* m_Handler;
                OnActivatedCallbackFunc m_Method;
            };
            return applicationView->add_Activated(Make<OnActivatedCallback>(this, &Application::OnActivated).Get(), &m_eventRegistrationOnActivate);
        }

        virtual HRESULT STDMETHODCALLTYPE SetWindow(ICoreWindow* window) override
        {
            m_Window = window;
            return m_Window->Activate();
        }

        virtual HRESULT STDMETHODCALLTYPE Load(HSTRING entryPoint) override
        {
            return S_OK;
        }

        static DWORD WINAPI MainThreadFunc(LPVOID arg)
        {
            Application* application = static_cast<Application*>(arg);

            int exitcode = main(static_cast<int>(application->m_argc), application->m_argv);

            wchar_t path[MAX_PATH + sizeof('\0')];
            swprintf_s(path, L"%s%s", GetDirectoryForStandardOutput(), kExitCodeFileName);

            FILE* file = _wfopen(path, L"w");
            if (file)
            {
                fprintf(file, "%i", exitcode);
                fclose(file);
            }

            fflush(stdout);
            fflush(stderr);
            application->m_Done = true;

            return 0;
        }

        virtual HRESULT STDMETHODCALLTYPE Run() override
        {
            // Create stdout/err handle, otherwise printf won't work
            CreateRedirectedHandle(stdout, kStdoutFileName);
            CreateRedirectedHandle(stderr, kStderrFileName);

            CreateThread(nullptr, 0, MainThreadFunc, this, 0, nullptr);

            ComPtr<ICoreDispatcher> dispatcher;
            m_Window->get_Dispatcher(&dispatcher);

            while (!m_Done)
            {
                dispatcher->ProcessEvents(CoreProcessEventsOption_ProcessAllIfPresent);
                Sleep(16);
            }
            return S_OK;
        }

        virtual HRESULT STDMETHODCALLTYPE Uninitialize() override
        {
            m_Window = nullptr;
            return S_OK;
        }

        HRESULT OnActivated(ICoreApplicationView* applicationView, IActivatedEventArgs* args)
        {
            applicationView->remove_Activated(m_eventRegistrationOnActivate);

            ActivationKind activationKind;
            HRESULT result = args->get_Kind(&activationKind);
            if (FAILED(result)) return result;

            if (activationKind == ActivationKind_Launch)
            {
                ComPtr<ILaunchActivatedEventArgs> launchArgs;
                args->QueryInterface(launchArgs.GetAddressOf());

                HSTRING argHString;
                result = launchArgs->get_Arguments(&argHString);
                if (FAILED(result)) return result;

                // Need to manually parse arguments since CommandLineToArgvW is not available!
                UINT32 argRawLen = 0;
                const wchar_t* argRaw = WindowsGetStringRawBuffer(argHString, &argRawLen);
                if (argRawLen)
                {
                    argRawLen += sizeof('\0');
                    bool inQuotes = false;
                    int numCharsSinceLastArg = 0;
                    for (UINT32 i = 0; i < argRawLen; ++i)
                    {
                        if ((argRaw[i] == '\0' || (isspace(argRaw[i]) && !inQuotes)) && numCharsSinceLastArg > 0)
                        {
                            const wchar_t* argStart = argRaw + i - numCharsSinceLastArg;
                            int requiredSize = WideCharToMultiByte(CP_UTF8, 0, argStart, numCharsSinceLastArg, nullptr, 0, nullptr, nullptr);
                            char* argRaw = (char*)malloc(requiredSize + 1);
                            WideCharToMultiByte(CP_UTF8, 0, argStart, numCharsSinceLastArg, argRaw, requiredSize, nullptr, nullptr);
                            argRaw[requiredSize] = '\0';
                            PushBackArgv(argRaw);
                            numCharsSinceLastArg = 0;
                        }
                        else if (argRaw[i] == '\"')
                            inQuotes = !inQuotes;
                        else
                            ++numCharsSinceLastArg;
                    }
                }
            }
            return result;
        }

        inline int GetMainReturnValue() const { return m_MainReturnValue; }
    };

    inline int Activate()
    {
        RoInitializer roInit;

        ComPtr<ICoreApplication> coreApplication;

        HSTRING coreApplicationHString;
        WindowsCreateString(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication,
            sizeof(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication) / sizeof(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication[0]) - sizeof('\0'),
            &coreApplicationHString);
        RoGetActivationFactory(coreApplicationHString, __uuidof(ICoreApplication), &coreApplication);
        WindowsDeleteString(coreApplicationHString);

        auto application = Make<Application>();
        coreApplication->Run(application.Get());

        return application->GetMainReturnValue();
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    return WinRT::Activate();
}
