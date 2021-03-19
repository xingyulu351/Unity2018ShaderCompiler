#include <QString>
#include <string>

namespace ureport
{
namespace ui
{
    class Phrases
    {
    public:
        static QString Translate(const std::string& key);

    private:
        Phrases();
    };
}
}
