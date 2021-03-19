#include "DefaultHardwareInfoProvider.h"
#include "system_interplay/Shell.h"
#include "system_interplay/Gate.h"

#include <QOpenGLFunctions>
#include <QOffscreenSurface>

#include <fstream>
#include <sstream>
#include <string>

static std::string Trim(const std::string &input)
{
    const std::string ws(" \t");
    size_t startpos = input.find_first_not_of(ws); // Find the first character position after excluding leading blank spaces
    size_t endpos = input.find_last_not_of(ws); // Find the first character position from reverse af

    // if all spaces or empty return an empty string
    if ((std::string::npos == startpos) || (std::string::npos == endpos))
        return std::string(); // empty string
    return input.substr(startpos, endpos - startpos + 1);
}

// parse lines of files into key-value pairs
static void parseInfoFile(std::istream &stream, std::vector<std::pair<std::string, std::string> > &values)
{
    std::string line;
    while (std::getline(stream, line))
    {
        std::string key, value;
        std::stringstream stream2(line);
        if (std::getline(stream2, key, ':'))
            if (std::getline(stream2, value))
                values.push_back(std::make_pair(Trim(key), Trim(value)));
    }
}

namespace ureport
{
namespace details
{
    class LinuxHardwareInfoProvider : public DefaultHardwareInfoProvider
    {
        std::shared_ptr<Shell> m_Shell;

    public:
        LinuxHardwareInfoProvider(std::shared_ptr<Shell> shell)
            : m_Shell(shell)
        {
        }

        std::string GetBasicHardwareType() const
        {
            // Yoinked from SystemInfo
            std::string retval;

            std::ifstream stream("/proc/cpuinfo", std::ios_base::in);
            if (stream.is_open())
            {
                std::vector<std::pair<std::string, std::string> > values;
                parseInfoFile(stream, values);

                for (size_t i = 0; i < values.size(); ++i)
                {
                    if (values[i].first == "model name")
                    {
                        retval = values[i].second;
                        break;
                    }
                }
            }
            return retval;
        }

        std::string GetGraphicsHardwareType() const
        {
            /* set-up OpenGL context */
            QOffscreenSurface *surface = new QOffscreenSurface();
            surface->create();

            QOpenGLContext *context = new QOpenGLContext();
            context->create();
            context->makeCurrent(surface);

            QOpenGLFunctions glFuncs(context);
            glFuncs.initializeOpenGLFunctions();

            /* generate hardware description */
            std::string hardware_desc =
                std::string((char*)glFuncs.glGetString(GL_VENDOR)) + ", " +
                (char*)glFuncs.glGetString(GL_RENDERER) + ", " +
                (char*)glFuncs.glGetString(GL_VERSION);

            /* clean-up */
            delete context;
            surface->destroy();
            delete surface;

            return hardware_desc;
        }
    };
}

    std::unique_ptr<HardwareInfoProvider> CreateHardwareInfoProvider()
    {
        return std::unique_ptr<HardwareInfoProvider>(
            new details::LinuxHardwareInfoProvider(CreateShell()));
    }
}
