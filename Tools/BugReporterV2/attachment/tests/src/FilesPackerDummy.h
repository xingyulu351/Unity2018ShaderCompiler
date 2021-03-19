#include "reporter/FilesPacker.h"

namespace ureport
{
namespace test
{
    class FilesPackerDummy : public ureport::FilesPacker
    {
        std::unique_ptr<File> Pack(FileSet& files, LongTermOperObserver* observer)
        {
            return std::unique_ptr<File>();
        }
    };
}
}
