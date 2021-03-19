#include <iostream>

#include "miniz_zip_ex.h"


// Include all the library code here directly to be access static functions without need to patch library code
#include <miniz.c>
#include <miniz_tdef.c>
#include <miniz_tinfl.c>
#include <miniz_zip.c>


// This a modified copy of mz_zip_writer_add_cfile, it accepts istream& instead of FILE*
mz_bool mz_zip_writer_add_istream(mz_zip_archive *pZip, const char *pArchive_name, std::istream &src_file, mz_uint64 size_to_add, const MZ_TIME_T *pFile_time, const void *pComment, mz_uint16 comment_size, mz_uint level_and_flags,
    const char *user_extra_data, mz_uint user_extra_data_len, const char *user_extra_data_central, mz_uint user_extra_data_central_len)
{
    mz_uint16 gen_flags = MZ_ZIP_LDH_BIT_FLAG_HAS_LOCATOR;
    mz_uint uncomp_crc32 = MZ_CRC32_INIT, level, num_alignment_padding_bytes;
    mz_uint16 method = 0, dos_time = 0, dos_date = 0, ext_attributes = 0;
    mz_uint64 local_dir_header_ofs, cur_archive_file_ofs = pZip->m_archive_size, uncomp_size = size_to_add, comp_size = 0;
    size_t archive_name_size;
    mz_uint8 local_dir_header[MZ_ZIP_LOCAL_DIR_HEADER_SIZE];
    mz_uint8 *pExtra_data = NULL;
    mz_uint32 extra_size = 0;
    mz_uint8 extra_data[MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE];
    mz_zip_internal_state *pState;

    if (level_and_flags & MZ_ZIP_FLAG_UTF8_FILENAME)
        gen_flags |= MZ_ZIP_GENERAL_PURPOSE_BIT_FLAG_UTF8;

    if ((int)level_and_flags < 0)
        level_and_flags = MZ_DEFAULT_LEVEL;
    level = level_and_flags & 0xF;

    /* Sanity checks */
    if ((!pZip) || (!pZip->m_pState) || (pZip->m_zip_mode != MZ_ZIP_MODE_WRITING) || (!pArchive_name) || ((comment_size) && (!pComment)) || (level > MZ_UBER_COMPRESSION))
        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

    pState = pZip->m_pState;

    if ((!pState->m_zip64) && (uncomp_size > MZ_UINT32_MAX))
    {
        /* Source file is too large for non-zip64 */
        /*return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE); */
        pState->m_zip64 = MZ_TRUE;
    }

    /* We could support this, but why? */
    if (level_and_flags & MZ_ZIP_FLAG_COMPRESSED_DATA)
        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_PARAMETER);

    if (!mz_zip_writer_validate_archive_name(pArchive_name))
        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_FILENAME);

    if (pState->m_zip64)
    {
        if (pZip->m_total_files == MZ_UINT32_MAX)
            return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES);
    }
    else
    {
        if (pZip->m_total_files == MZ_UINT16_MAX)
        {
            pState->m_zip64 = MZ_TRUE;
            /*return mz_zip_set_error(pZip, MZ_ZIP_TOO_MANY_FILES); */
        }
    }

    archive_name_size = strlen(pArchive_name);
    if (archive_name_size > MZ_UINT16_MAX)
        return mz_zip_set_error(pZip, MZ_ZIP_INVALID_FILENAME);

    num_alignment_padding_bytes = mz_zip_writer_compute_padding_needed_for_file_alignment(pZip);

    /* miniz doesn't support central dirs >= MZ_UINT32_MAX bytes yet */
    if (((mz_uint64)pState->m_central_dir.m_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + MZ_ZIP64_MAX_CENTRAL_EXTRA_FIELD_SIZE + comment_size) >= MZ_UINT32_MAX)
        return mz_zip_set_error(pZip, MZ_ZIP_UNSUPPORTED_CDIR_SIZE);

    if (!pState->m_zip64)
    {
        /* Bail early if the archive would obviously become too large */
        if ((pZip->m_archive_size + num_alignment_padding_bytes + MZ_ZIP_LOCAL_DIR_HEADER_SIZE + archive_name_size + MZ_ZIP_CENTRAL_DIR_HEADER_SIZE + archive_name_size + comment_size + pState->m_central_dir.m_size + MZ_ZIP_END_OF_CENTRAL_DIR_HEADER_SIZE + 1024) > 0xFFFFFFFF)
        {
            pState->m_zip64 = MZ_TRUE;
            /*return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE); */
        }
    }

#ifndef MINIZ_NO_TIME
    if (pFile_time)
    {
        mz_zip_time_t_to_dos_time(*pFile_time, &dos_time, &dos_date);
    }
#endif

    if (uncomp_size <= 3)
        level = 0;

    if (!mz_zip_writer_write_zeros(pZip, cur_archive_file_ofs, num_alignment_padding_bytes))
    {
        return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
    }

    cur_archive_file_ofs += num_alignment_padding_bytes;
    local_dir_header_ofs = cur_archive_file_ofs;

    if (pZip->m_file_offset_alignment)
    {
        MZ_ASSERT((cur_archive_file_ofs & (pZip->m_file_offset_alignment - 1)) == 0);
    }

    if (uncomp_size && level)
    {
        method = MZ_DEFLATED;
    }

    MZ_CLEAR_OBJ(local_dir_header);
    if (pState->m_zip64)
    {
        if (uncomp_size >= MZ_UINT32_MAX || local_dir_header_ofs >= MZ_UINT32_MAX)
        {
            pExtra_data = extra_data;
            extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (uncomp_size >= MZ_UINT32_MAX) ? &uncomp_size : NULL,
                (uncomp_size >= MZ_UINT32_MAX) ? &comp_size : NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
        }

        if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, extra_size + user_extra_data_len, 0, 0, 0, method, gen_flags, dos_time, dos_date))
            return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

        cur_archive_file_ofs += sizeof(local_dir_header);

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
        {
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
        }

        cur_archive_file_ofs += archive_name_size;

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, extra_data, extra_size) != extra_size)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

        cur_archive_file_ofs += extra_size;
    }
    else
    {
        if ((comp_size > MZ_UINT32_MAX) || (cur_archive_file_ofs > MZ_UINT32_MAX))
            return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);
        if (!mz_zip_writer_create_local_dir_header(pZip, local_dir_header, (mz_uint16)archive_name_size, user_extra_data_len, 0, 0, 0, method, gen_flags, dos_time, dos_date))
            return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_header, sizeof(local_dir_header)) != sizeof(local_dir_header))
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

        cur_archive_file_ofs += sizeof(local_dir_header);

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pArchive_name, archive_name_size) != archive_name_size)
        {
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);
        }

        cur_archive_file_ofs += archive_name_size;
    }

    if (user_extra_data_len > 0)
    {
        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, user_extra_data, user_extra_data_len) != user_extra_data_len)
            return mz_zip_set_error(pZip, MZ_ZIP_FILE_WRITE_FAILED);

        cur_archive_file_ofs += user_extra_data_len;
    }

    if (uncomp_size)
    {
        mz_uint64 uncomp_remaining = uncomp_size;
        void *pRead_buf = pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, MZ_ZIP_MAX_IO_BUF_SIZE);
        if (!pRead_buf)
        {
            return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
        }

        if (!level)
        {
            while (uncomp_remaining)
            {
                mz_uint n = (mz_uint)MZ_MIN((mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE, uncomp_remaining);
                if ((!src_file.read((char*)pRead_buf, n)) || (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, pRead_buf, n) != n))
                {
                    pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                    return mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                }
                uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8*)pRead_buf, n);
                uncomp_remaining -= n;
                cur_archive_file_ofs += n;
            }
            comp_size = uncomp_size;
        }
        else
        {
            mz_bool result = MZ_FALSE;
            mz_zip_writer_add_state state;
            tdefl_compressor *pComp = (tdefl_compressor*)pZip->m_pAlloc(pZip->m_pAlloc_opaque, 1, sizeof(tdefl_compressor));
            if (!pComp)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                return mz_zip_set_error(pZip, MZ_ZIP_ALLOC_FAILED);
            }

            state.m_pZip = pZip;
            state.m_cur_archive_file_ofs = cur_archive_file_ofs;
            state.m_comp_size = 0;

            if (tdefl_init(pComp, mz_zip_writer_add_put_buf_callback, &state, tdefl_create_comp_flags_from_zip_params(level, -15, MZ_DEFAULT_STRATEGY)) != TDEFL_STATUS_OKAY)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                return mz_zip_set_error(pZip, MZ_ZIP_INTERNAL_ERROR);
            }

            for (;;)
            {
                size_t in_buf_size = (mz_uint32)MZ_MIN(uncomp_remaining, (mz_uint64)MZ_ZIP_MAX_IO_BUF_SIZE);
                tdefl_status status;

                if (!src_file.read((char*)pRead_buf, in_buf_size))
                {
                    mz_zip_set_error(pZip, MZ_ZIP_FILE_READ_FAILED);
                    break;
                }

                uncomp_crc32 = (mz_uint32)mz_crc32(uncomp_crc32, (const mz_uint8*)pRead_buf, in_buf_size);
                uncomp_remaining -= in_buf_size;

                status = tdefl_compress_buffer(pComp, pRead_buf, in_buf_size, uncomp_remaining ? TDEFL_NO_FLUSH : TDEFL_FINISH);
                if (status == TDEFL_STATUS_DONE)
                {
                    result = MZ_TRUE;
                    break;
                }
                else if (status != TDEFL_STATUS_OKAY)
                {
                    mz_zip_set_error(pZip, MZ_ZIP_COMPRESSION_FAILED);
                    break;
                }
            }

            pZip->m_pFree(pZip->m_pAlloc_opaque, pComp);

            if (!result)
            {
                pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
                return MZ_FALSE;
            }

            comp_size = state.m_comp_size;
            cur_archive_file_ofs = state.m_cur_archive_file_ofs;
        }

        pZip->m_pFree(pZip->m_pAlloc_opaque, pRead_buf);
    }

    {
        mz_uint8 local_dir_footer[MZ_ZIP_DATA_DESCRIPTER_SIZE64];
        mz_uint32 local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE32;

        MZ_WRITE_LE32(local_dir_footer + 0, MZ_ZIP_DATA_DESCRIPTOR_ID);
        MZ_WRITE_LE32(local_dir_footer + 4, uncomp_crc32);
        if (pExtra_data == NULL)
        {
            if (comp_size > MZ_UINT32_MAX)
                return mz_zip_set_error(pZip, MZ_ZIP_ARCHIVE_TOO_LARGE);

            MZ_WRITE_LE32(local_dir_footer + 8, comp_size);
            MZ_WRITE_LE32(local_dir_footer + 12, uncomp_size);
        }
        else
        {
            MZ_WRITE_LE64(local_dir_footer + 8, comp_size);
            MZ_WRITE_LE64(local_dir_footer + 16, uncomp_size);
            local_dir_footer_size = MZ_ZIP_DATA_DESCRIPTER_SIZE64;
        }

        if (pZip->m_pWrite(pZip->m_pIO_opaque, cur_archive_file_ofs, local_dir_footer, local_dir_footer_size) != local_dir_footer_size)
            return MZ_FALSE;

        cur_archive_file_ofs += local_dir_footer_size;
    }

    if (pExtra_data != NULL)
    {
        extra_size = mz_zip_writer_create_zip64_extra_data(extra_data, (uncomp_size >= MZ_UINT32_MAX) ? &uncomp_size : NULL,
            (uncomp_size >= MZ_UINT32_MAX) ? &comp_size : NULL, (local_dir_header_ofs >= MZ_UINT32_MAX) ? &local_dir_header_ofs : NULL);
    }

    if (!mz_zip_writer_add_to_central_dir(pZip, pArchive_name, (mz_uint16)archive_name_size, pExtra_data, extra_size, pComment, comment_size,
        uncomp_size, comp_size, uncomp_crc32, method, gen_flags, dos_time, dos_date, local_dir_header_ofs, ext_attributes,
        user_extra_data_central, user_extra_data_central_len))
        return MZ_FALSE;

    pZip->m_total_files++;
    pZip->m_archive_size = cur_archive_file_ofs;

    return MZ_TRUE;
}
