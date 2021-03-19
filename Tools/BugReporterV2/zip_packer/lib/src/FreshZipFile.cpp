#include "FreshZipFile.h"

namespace ureport
{
namespace details
{
    std::shared_ptr<void> FreshZipFile::OpenZipFile(FILE* file)
    {
        auto const deleter = [] (mz_zip_archive* archive) {
                mz_bool finalized = mz_zip_writer_finalize_archive(archive);
                assert(finalized == MZ_TRUE);
                mz_bool ended = mz_zip_writer_end(archive);
                assert(ended == MZ_TRUE);
                delete archive;
            };
        mz_zip_archive* archive = new mz_zip_archive();
        mz_bool open_succeeded = mz_zip_writer_init_cfile(
            archive, file, MZ_ZIP_FLAG_WRITE_ZIP64);
        assert(open_succeeded == MZ_TRUE);
        return std::shared_ptr<void>(archive, deleter);
    }
}
}
