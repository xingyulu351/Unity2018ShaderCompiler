#pragma once

#include "ZipFile.h"
#include "shims/attribute/GetCString.h"

#include "miniz_zip_ex.h"

namespace ureport
{
namespace details
{
    class BasicZipFile : public ureport::ZipFile
    {
        std::string m_CurrentEntry;

    protected:
        std::shared_ptr<FILE> m_File;
        std::shared_ptr<void> m_Archive;

    protected:

        void CreateDirectory(const std::string& name) override
        {
            std::string dir_name = name;
            if (dir_name.back() != '/')
            {
                dir_name += '/';
            }
            mz_zip_archive* zipArchive = (mz_zip_archive*)m_Archive.get();
            mz_bool result = mz_zip_writer_add_mem(
                (mz_zip_archive*)m_Archive.get(), dir_name.c_str(),
                "", 0,
                MZ_ZIP_FLAG_UTF8_FILENAME);
            ThrowIfFailed(result, zipArchive);
        }

        void OpenNewEntry(const std::string& name)
        {
            m_CurrentEntry = name;
        }

        bool IsDir() const override
        {
            return false;
        }

        std::unique_ptr<std::istream> Read() const
        {
            return std::unique_ptr<std::istream>();
        }

        void Write(std::istream& stream)
        {
            stream.seekg(0, stream.end);
#if defined(_MSC_VER)
# if _MSC_VER > 1600
# pragma message("Try removing seekpos call while calculating istream size for files more than 4GB size")
# endif
            mz_uint64 stream_len = stream.tellg().seekpos();
#else
            mz_uint64 stream_len = stream.tellg();
#endif
            stream.seekg(0, stream.beg);
            assert(stream);

            mz_zip_archive* zipArchive = (mz_zip_archive*)m_Archive.get();
            mz_bool result = mz_zip_writer_add_istream(
                zipArchive, m_CurrentEntry.c_str(), stream,
                stream_len, nullptr,
                nullptr, 0,
                MZ_ZIP_FLAG_UTF8_FILENAME | MZ_BEST_SPEED,
                nullptr, 0,
                nullptr, 0);
            ThrowIfFailed(result, zipArchive);
        }

    private:
        void ThrowIfFailed(mz_bool result, mz_zip_archive* archive)
        {
            std::string errorMessage = "Error occurred while creating an archive.";
            if (archive)
            {
                errorMessage += " Error code: " + std::string(mz_zip_get_error_string(archive->m_last_error));
            }
            if (result == MZ_FALSE)
            {
                throw std::runtime_error(errorMessage);
            }
        }
    };
}
}
