#include <iosfwd>
#include <miniz_zip.h>


mz_bool mz_zip_writer_add_istream(mz_zip_archive *pZip, const char *pArchive_name,
    std::istream &src_file, mz_uint64 size_to_add, const MZ_TIME_T *pFile_time,
    const void *pComment, mz_uint16 comment_size,
    mz_uint level_and_flags,
    const char *user_extra_data, mz_uint user_extra_data_len,
    const char *user_extra_data_central, mz_uint user_extra_data_central_len);
