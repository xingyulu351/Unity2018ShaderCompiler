#include "ZipFile.h"
#include "ZipUtils.h"
#include "TemporaryFile.h"
#include "reporter/FilesPacker.h"
#include "shims/logical/IsNull.h"
#include "shims/logical/IsEmpty.h"
#include "file_system/FileGateway.h"
#include "system_interplay/SystemEnvironment.h"
#include "common/StringUtils.h"
#include "long_term_operation/LongTermOperObserver.h"

#include <algorithm>
#include <memory>
#include <sstream>

namespace ureport
{
namespace details
{
    std::unique_ptr<ureport::File> MakeTemporary(std::unique_ptr<ureport::File> file)
    {
        return std::unique_ptr<ureport::File>(new TemporaryFile(std::move(file)));
    }

    std::string MakeTemporaryFileName()
    {
        return std::string("com.unity3d.bug_reporter.") + MakeRandomString(8, "0123456789abcdef");
    }

    class ZipFilesPacker : public FilesPacker
    {
        std::shared_ptr<FileGateway> m_Files;
        std::shared_ptr<SystemEnvironment> m_Environment;

    public:
        ZipFilesPacker(std::shared_ptr<FileGateway> files,
                       std::shared_ptr<SystemEnvironment> environment)
            : m_Files(files)
            , m_Environment(environment)
        {
        }

    private:
        std::unique_ptr<File> Pack(
            FileSet& files, LongTermOperObserver* observer) override
        {
            if (IsEmpty(files))
                return nullptr;
            auto zip = CreateNewZip(MakeTemporaryFilePath());
            LongTermBlockedOperObserver helper(observer);
            helper.ReportProgress(0, files.size());
            for (FileSet::size_type i = 0; i < files.size(); ++i)
            {
                if (helper.IsOperationCanceled())
                    break;
                ureport::Pack(*zip, *files[i]);
                helper.ReportProgress(i + 1, files.size());
            }
            const auto zipPath = zip->GetPath();
            zip.reset();
            return MakeTemporary(m_Files->GetFile(zipPath));
        }

        std::string MakeTemporaryFilePath()
        {
            std::stringstream path;
            path << m_Environment->GetTempDirectory() << "/" << MakeTemporaryFileName();
            return path.str();
        }
    };
}

    std::unique_ptr<FilesPacker> CreateZipFilesPacker(std::shared_ptr<FileGateway> files,
        std::shared_ptr<SystemEnvironment> environment)
    {
        return std::unique_ptr<FilesPacker>(new details::ZipFilesPacker(files, environment));
    }
}
