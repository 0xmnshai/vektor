#include "VKE_blendfile.h"
#include "../../intern/clog/COG_log.hh"
#include "VKE_version.hh"

namespace vektor
{
CLG_LogRef LOG_VEKTOR = {"VEKTOR"};

void       BLO_blendfiledata_free(BlendFileData* bfd)
{
    if (bfd->main)
    {
        VKE_main_free(bfd->main);
    }

    if (bfd->user)
    {
        MEM_delete(bfd->user);
    }

    MEM_delete(bfd);
}

static void handle_subversion_warning(Main*                main,
                                      BlendFileReadReport* reports)
{
    if (main->versionfile > VEKTOR_FILE_VERSION ||
        (main->versionfile == VEKTOR_FILE_VERSION && main->subversionfile > VEKTOR_FILE_SUBVERSION))
    {
        VKE_reportf(reports->reports, RPT_WARNING, "File written by newer vektor binary (%d.%d), expect loss of data!",
                    main->versionfile, main->subversionfile);
    }
}

BlendFileData* BLO_read_from_file(const char*          filepath,
                                  eBLOReadSkip         skip_flags,
                                  BlendFileReadReport* reports);

BlendFileData* VKE_blendfile_read(const char*                filepath,
                                  const BlendFileReadParams* params,
                                  BlendFileReadReport*       reports)
{
    if (params->is_startup == false)
    {
        CLOG_INFO_NOCHECK(&LOG_VEKTOR, "Read vektor file: \"%s\"", filepath);
    }

    BlendFileData* bfd = BLO_read_from_file(filepath, eBLOReadSkip(params->skip_flags), reports);
    if (bfd && bfd->main->is_read_invalid)
    {
        BLO_blendfiledata_free(bfd);
        bfd = nullptr;
    }
    if (bfd)
    {
        handle_subversion_warning(bfd->main, reports);
    }
    else
    {
        VKE_reports_prependf(reports->reports, "Loading \"%s\" failed: ", filepath);
    }
    return bfd;
};

struct FileReader;

typedef int64_t (*FileReaderReadFn)(struct FileReader* reader,
                                    void*              buffer,
                                    size_t             size);
typedef uint64_t (*FileReaderSeekFn)(struct FileReader* reader,
                                     uint64_t           offset,
                                     int                whence);
typedef void (*FileReaderCloseFn)(struct FileReader* reader);
/** General structure for all #FileReaders, implementations add custom fields at the end. */
struct FileReader
{
    FileReaderReadFn  read;
    FileReaderSeekFn  seek;
    FileReaderCloseFn close;

    uint64_t          offset;
};

// core vektor header .h
#define MIN_SIZEOFVEKTORHEADER 12

struct MemoryReader
{
    FileReader  reader;

    const char* data;
    size_t      length;
};

static size_t memory_read_raw(FileReader* reader,
                              void*       buffer,
                              size_t      len)
{
    MemoryReader* mem = reinterpret_cast<MemoryReader*>(reader);
    size_t        rem = mem->length - mem->reader.offset;
    size_t        cpy = (len < rem) ? len : rem;
    if (cpy > 0)
    {
        memcpy(buffer, mem->data + mem->reader.offset, cpy);
        mem->reader.offset += cpy;
    }
    return cpy;
}

static bool memory_seek(FileReader* reader,
                        off_t       offset,
                        int         whence)
{
    MemoryReader* mem = reinterpret_cast<MemoryReader*>(reader);
    switch (whence)
    {
        case SEEK_SET:
            if (offset < 0 || (size_t)offset > mem->length)
            {
                return false;
            }
            mem->reader.offset = offset;
            break;
        case SEEK_CUR:
            if (offset < 0 && (size_t)-offset > mem->reader.offset)
            {
                return false;
            }
            mem->reader.offset += offset;
            break;
        case SEEK_END:
            if (offset > 0 || (size_t)-offset > mem->length)
            {
                return false;
            }
            mem->reader.offset = mem->length + offset;
            break;
    }
    return true;
}

static void memory_close_raw(FileReader* reader)
{
    MemoryReader* mem = reinterpret_cast<MemoryReader*>(reader);
    MEM_delete(mem);
}

FileReader* BLI_filereader_new_memory(const void* data,
                                      size_t      len)
{
    MemoryReader* mem  = MEM_new_zeroed<MemoryReader>(__func__);

    mem->data          = static_cast<const char*>(data);
    mem->length        = len;
    mem->reader.offset = 0;
    // mem->reader.read   = memory_read_raw;
    // mem->reader.seek   = memory_seek;
    mem->reader.close  = memory_close_raw;

    return reinterpret_cast<FileReader*>(mem);
};

bool BLI_file_magic_is_gzip(const char header[4]);
bool BLI_file_magic_is_zstd(const char header[4]);

bool BLI_file_magic_is_gzip(const char header[4])
{
    /* GZIP itself starts with the magic bytes 0x1f 0x8b.
     * The third byte indicates the compression method, which is 0x08 for DEFLATE. */
    return header[0] == 0x1f && header[1] == 0x8b && header[2] == 0x08;
}

FileReader* BLI_filereader_new_file(int filedes) ATTR_WARN_UNUSED_RESULT;
/** Create #FileReader from raw file descriptor using memory-mapped IO. */
FileReader* BLI_filereader_new_mmap(int filedes) ATTR_WARN_UNUSED_RESULT;
/** Create #FileReader from a region of memory. */
FileReader* BLI_filereader_new_memory(const void* data,
                                      size_t      len) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL();
/** Create #FileReader from applying `Zstd` decompression on an underlying file. */
FileReader* BLI_filereader_new_zstd(FileReader* base) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL();
/** Create #FileReader from applying `Gzip` decompression on an underlying file. */
FileReader* BLI_filereader_new_gzip(FileReader* base) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL();

// implement this in VKI_mmap.h / .cc
struct BLI_mmap_file
{
    /* The address to which the file was mapped. */
    char*  memory;

    /* The length of the file (and therefore the mapped region). */
    size_t length;

    /* Platform-specific handle for the mapping. */
    void* volatile handle;

    /* Flag to indicate IO errors. Needs to be volatile since it's being set from
     * within the signal handler, which is not part of the normal execution flow. */
    volatile bool io_error;

    /* Used to break out of infinite loops when an error keeps occurring.
     * See the comments in #try_handle_error_for_address for details. */
    size_t        id;
};

FileReader* BLO_file_reader_uncompressed(FileReader* rawfile)
{
    if (!rawfile)
    {
        return nullptr;
    }
    char first_bytes[7];
    if (rawfile->read(rawfile, first_bytes, sizeof(first_bytes)) != sizeof(first_bytes))
    {
        /* The file is too small to possibly be a valid blend file. */
        rawfile->close(rawfile);
        return nullptr;
    }
    /* Rewind to the start of the file. */
    rawfile->seek(rawfile, 0, SEEK_SET);
    if (memcmp(first_bytes, "VEKTOR", sizeof(first_bytes)) == 0)
    {
        /* The file is uncompressed. */
        return rawfile;
    }
    if (BLI_file_magic_is_gzip(first_bytes))
    {
        /* The new reader takes ownership of the rawfile. */
        return BLI_filereader_new_gzip(rawfile);
    }
    if (BLI_file_magic_is_zstd(first_bytes))
    {
        /* The new reader takes ownership of the rawfile. */
        return BLI_filereader_new_zstd(rawfile);
    }
    rawfile->close(rawfile);
    return nullptr;
}

FileReader* BLO_file_reader_uncompressed_from_memory(const void* mem,
                                                     const int   memsize)
{
    return BLO_file_reader_uncompressed(BLI_filereader_new_memory(mem, memsize));
};

void blo_filedata_free(FileData* fd) {}
bool read_file_dna(FileData*    fd,
                   const char** error_message)
{
    return true;
}
bool is_minversion_older_than_vektor(FileData*   fd,
                                     ReportList* reports)
{
    return true;
}

struct MemFile
{
    //   ListBaseT<MemFileChunk> chunks;
    size_t size;
    /**
     * Some data is not serialized into a new buffer because the undo-step can take ownership of it
     * without making a copy. This is faster and requires less memory.
     */
    //   MemFileSharedStorage *shared_storage;

    /**
     * Partial storage of the WriteData's generated stable pointers data, to be re-used when writing
     * the next undo step.
     */
    //   WriteDataStableAddressIDs *stable_address_ids;
};

FileReader* BLO_memfile_new_filereader(MemFile* memfile,
                                       int      undo_direction)
{
    return nullptr;
}
BlendFileData* blo_read_file_internal(FileData*   fd,
                                      const char* filepath)
{
    return nullptr;
}
#define BLI_STATIC_ASSERT(cond, msg)

enum
{
    FD_FLAGS_FILE_OK             = 1 << 0,
    FD_FLAGS_FILE_POINTSIZE_IS_4 = 1 << 1,
    FD_FLAGS_POINTSIZE_DIFFERS   = 1 << 2,
    FD_FLAGS_SWITCH_ENDIAN       = 1 << 3,
    FD_FLAGS_FILE_FUTURE         = 1 << 4,
};

struct BlenderHeader
{
    int pointer_size;
    int endian;
    int file_version;
};

struct BlenderHeaderInvalid
{
};
struct BlenderHeaderUnknown
{
};

using BlenderHeaderVariant = std::variant<BlenderHeaderInvalid, BlenderHeaderUnknown, BlenderHeader>;

BlenderHeaderVariant BLO_readfile_vektor_header_decode(FileReader*)
{
    return BlenderHeaderUnknown{};
};

#define ENDIAN_ORDER 1
#define L_ENDIAN 1

struct FileData
{
    /** Linked list of BHeadN's. */
    bool                 is_eof               = false;

    FileReader*          file                 = nullptr;
    int                  flags                = 0;
    BlenderHeader        vektor_header        = {};

    /**
     * Whether we are undoing (< 0) or redoing (> 0), used to choose which 'unchanged' flag to use
     * to detect unchanged data from memfile.
     * #eUndoStepDir.
     */
    int                  undo_direction       = 0;

    /** Used for relative paths handling.
     *
     * Typically the actual filepath of the read blend-file, except when recovering
     * save-on-exit/autosave files. In the latter case, it will be the path of the file that
     * generated the auto-saved one being recovered.
     *
     * NOTE: Currently expected to be the same path as #BlendFileData.filepath. */
    char                 relabase[FILE_MAX]   = {};

    const char*          compflags            = nullptr;

    int                  fileversion          = 0;
    /**
     * Unlike the `fileversion` which is read from the header,
     * this is initialized from #read_file_dna.
     */
    int                  filesubversion       = 0;

    /** Used to retrieve ID names from (bhead+1). */
    int                  id_name_offset       = 0;
    /** Used to retrieve asset data from (bhead+1). NOTE: This may not be available in old files,
     * will be -1 then! */
    int                  id_asset_data_offset = 0;
    int                  id_flag_offset       = 0;
    int                  id_deep_hash_offset  = 0;
    /** For do_versions patching. */
    int                  globalf              = 0;
    int                  fileflags            = 0;

    /** Optionally skip some data-blocks when they're not needed. */
    eBLOReadSkip         skip_flags           = BLO_READ_SKIP_NONE;

    /**
     * Tag to apply to all loaded ID data-blocks.
     *
     * \note This is initialized from #LibraryLink_Params.id_tag_extra since passing it as an
     * argument would need an additional argument to be passed around when expanding library data.
     */
    int                  id_tag_extra         = 0;

    /** Used to keep track of already loaded packed IDs to avoid loading them multiple times. */

    /**
     * Store mapping from old ID pointers (the values they have in the .blend file) to new ones,
     * typically from value in `bhead->old` to address in memory where the ID was read.
     * Used during library-linking process (see #lib_link_all).
     */
    int                  tot_bheadmap         = 0;

    /**
     * The root (main, local) Main.
     * The Main that will own Library IDs.
     *
     * When reading libraries, this is typically _not_ the same Main as the one being populated from
     * the content of this filedata, see #fd_bmain.
     */
    Main*                bmain                = nullptr;
    /** The existing root (main, local) Main, used for undo. */
    Main*                old_bmain            = nullptr;
    /**
     * The main for the (local) data loaded from this filedata.
     *
     * This is the same as #bmain when opening a blend-file, but not when reading/loading from
     * libraries blend-files.
     */
    Main*                fd_bmain             = nullptr;

    BlendFileReadReport* reports              = nullptr;

    /** Opaque handle to the storage system used for non-static allocation strings. */
    void*                storage_handle       = nullptr;
};

static FileData* filedata_new(BlendFileReadReport* reports)
{
    FileData* fd = MEM_new<FileData>(__func__);
    return fd;
};

static void read_vektor_header(FileData* fd)
{
    const BlenderHeaderVariant header_variant = BLO_readfile_vektor_header_decode(fd->file);
    if (std::holds_alternative<BlenderHeaderInvalid>(header_variant))
    {
        return;
    }
    if (std::holds_alternative<BlenderHeaderUnknown>(header_variant))
    {
        fd->flags |= FD_FLAGS_FILE_FUTURE;
        return;
    }
    const BlenderHeader& header = std::get<BlenderHeader>(header_variant);
    fd->flags |= FD_FLAGS_FILE_OK;
    if (header.pointer_size == 4)
    {
        fd->flags |= FD_FLAGS_FILE_POINTSIZE_IS_4;
    }
    if (header.pointer_size != sizeof(void*))
    {
        fd->flags |= FD_FLAGS_POINTSIZE_DIFFERS;
    }
    if (header.endian != ENDIAN_ORDER)
    {
        fd->flags |= FD_FLAGS_SWITCH_ENDIAN;
    }
    fd->fileversion   = header.file_version;
    fd->vektor_header = header;
};

static FileData* blo_decode_and_check(FileData*   fd,
                                      ReportList* reports)
{
    read_vektor_header(fd);

    if (fd->flags & FD_FLAGS_SWITCH_ENDIAN)
    {
        BLI_STATIC_ASSERT(ENDIAN_ORDER == L_ENDIAN, "Blender only builds on little endian systems")
        VKE_reportf(reports, RPT_ERROR,
                    "Blend file '%s' created by a Big Endian version of Blender, support for "
                    "these files has been removed in Blender 5.0, use an older version of Blender "
                    "to open and convert it.",
                    fd->relabase);
    }
    else if (fd->flags & FD_FLAGS_FILE_OK)
    {
        const char* error_message = nullptr;
        if (!read_file_dna(fd, &error_message))
        {
            VKE_reportf(reports, RPT_ERROR, "Failed to read blend file '%s': %s", fd->relabase, error_message);
        }
        else if (!is_minversion_older_than_vektor(fd, reports))
        {
            return fd;
        }
    }
    else if (fd->flags & FD_FLAGS_FILE_FUTURE)
    {
        VKE_reportf(reports, RPT_ERROR,
                    "Cannot read blend file '%s', incomplete header, may be from a newer version of Blender",
                    fd->relabase);
    }
    else
    {
        VKE_reportf(reports, RPT_ERROR, "Failed to read file '%s', not a blend file", fd->relabase);
    }

    blo_filedata_free(fd);
    return nullptr;
}

FileData* blo_filedata_from_memory(const void*          mem,
                                   const int            memsize,
                                   BlendFileReadReport* reports)
{
    if (!mem || memsize < MIN_SIZEOFVEKTORHEADER)
    {
        // VKE_report(reports->reports, RPT_WARNING, (mem) ? RPT_("Unable to read") : RPT_("Unable to open"));
        return nullptr;
    }

    FileReader* file = BLO_file_reader_uncompressed_from_memory(mem, memsize);
    if (!file)
    {
        return nullptr;
    }

    FileData* fd = filedata_new(reports);
    fd->file     = file;

    return blo_decode_and_check(fd, reports->reports);
}

FileData* blo_filedata_from_memfile(MemFile*                   memfile,
                                    const BlendFileReadParams* params,
                                    BlendFileReadReport*       reports)
{
    if (!memfile)
    {
        // VKE_report(reports->reports, RPT_WARNING, "Unable to open blend <memory>");
        return nullptr;
    }

    FileData* fd       = filedata_new(reports);
    fd->file           = BLO_memfile_new_filereader(memfile, params->undo_direction);
    fd->undo_direction = params->undo_direction;
    // fd->flags |= FD_FLAGS_IS_MEMFILE;

    return blo_decode_and_check(fd, reports->reports);
};

// read vekor entry . h
BlendFileData* BLO_read_from_memory(const void*  mem,
                                    int          memsize,
                                    eBLOReadSkip skip_flags,
                                    ReportList*  reports)
{
    BlendFileData*      bfd = nullptr;
    FileData*           fd;
    BlendFileReadReport bf_reports{};
    bf_reports.reports = reports;

    fd                 = blo_filedata_from_memory(mem, memsize, &bf_reports);
    if (fd)
    {
        fd->skip_flags = skip_flags;
        bfd            = blo_read_file_internal(fd, "");
        blo_filedata_free(fd);
    }

    return bfd;
}

BlendFileData* VKE_blendfile_read_from_memory(const void*                file_buf,
                                              int                        file_buf_size,
                                              const BlendFileReadParams* params,
                                              ReportList*                reports)
{
    BlendFileData* bfd = BLO_read_from_memory(file_buf, file_buf_size, eBLOReadSkip(params->skip_flags), reports);
    if (bfd && bfd->main->is_read_invalid)
    {
        BLO_blendfiledata_free(bfd);
        bfd = nullptr;
    }
    if (bfd)
    {
        /* Pass. */
    }
    else
    {
        // VKE_reports_prepend(reports, "Loading failed: ");
    }
    return bfd;
}

extern const char datatoc_startup_blend[]    = {0};
extern const int  datatoc_startup_blend_size = 0;

void              VKE_vektor_userdef_data_set_and_free(UserDef* userdef)
{
    if (userdef)
    {
        // Here we would replace global userdef, but for now just free
        MEM_delete(userdef);
    }
}

} // namespace vektor