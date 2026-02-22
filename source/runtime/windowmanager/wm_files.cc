#include <sys/fcntl.h>
#include <unistd.h>
#include <cstddef>
#include <cstdint>

#include "COG_log.hh"
#include "GLFW_IWindow.hh"
#include "MEM_gaurdalloc.hh"
#include "wm.hh"
#include "wm_api.h"

#include "../kernel/VK_global.hh"
#include "../loader/VLO_readfile.h"
#include "../vklib/VKE_Time.h"
#include "../vklib/VKE_blendfile.h"
#include "../vklib/VKE_callbacks.hh"
#include "../vklib/VKE_context.h"
#include "../vklib/VKE_timer.h"
#include "wm_cursors.hh"
#include "wm_event.h"
#include "wm_system.h"

#include "wm_api.h"
#include "wm_files.h"

#include "../../editor/include/ED_screen.hh"
#include "../dna/DNA_workspace_types.h"
#include "../vklib/VKE_workspace.hh"

#include "../vklib/VKE_context.h"
#include "../vklib/VKE_lib_id.hh"

#include "../../editor/include/ED_datafiles.h"
#include "../../intern/gaurdalloc/MEM_gaurdalloc.hh"
#include "../dna/DNA_id.h"

namespace vektor
{

#define VKE_READ_EXOTIC_FAIL_PATH -3   /* File format is not supported. */
#define VKE_READ_EXOTIC_FAIL_FORMAT -2 /* File format is not supported. */
#define VKE_READ_EXOTIC_FAIL_OPEN -1   /* Can't open the file. */
#define VKE_READ_EXOTIC_OK_VKEND 0     /* `.blend` file. */

#define O_BINARY 0

static struct
{
    std::string app_template;
    bool        override;
} wm_init_state_app_template = {{0}};

static void wm_read_callback_pre_wrapper(vkContext*  C,
                                         const char* filepath)
{
    VKE_callback_exec_string(CTX_data_main(C), filepath, VKE_CB_EVT_LOAD_PRE);
}

static void wm_read_callback_post_wrapper(vkContext*  C,
                                          const char* filepath,
                                          const bool  success)
{
    VKE_callback_exec_string(CTX_data_main(C), filepath, success ? VKE_CB_EVT_LOAD_POST : VKE_CB_EVT_LOAD_POST_FAIL);
}

int VKE_open(const char* filepath, // implement this in fileops. h / . cc
             int         oflag,
             int         pmode)
{
    // CLOG_ASSERT(CLG_LogRef, !BLI_path_is_rel(filepath));

    return open(filepath, oflag, pmode);
}

// implement this in filereader .h / .cc
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

FileReader* BLO_file_reader_uncompressed_from_descriptor(int filedes); // implement this in VLO_core_file_reader. h

// implement this in filereader
/** Create #FileReader from raw file descriptor. */
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

struct MemoryReader
{
    FileReader     reader;

    const char*    data;
    BLI_mmap_file* mmap;
    size_t         length;
};

// fileops.h
bool BLI_mmap_read(BLI_mmap_file* file,
                   void*          dest,
                   size_t         offset,
                   size_t         length) ATTR_WARN_UNUSED_RESULT ATTR_NONNULL(1);

bool BLI_mmap_read(BLI_mmap_file* file,
                   void*          dest,
                   size_t         offset,
                   size_t         length)
{
    /* If a previous read has already failed or we try to read past the end,
     * don't even attempt to read any further. */
    if (file->io_error || (offset + length > file->length))
    {
        return false;
    }

    memcpy(dest, file->memory + offset, length);

    return !file->io_error;
}

static int64_t memory_read_mmap(FileReader* reader,
                                void*       buffer,
                                size_t      size)
{
    MemoryReader* mem      = reinterpret_cast<MemoryReader*>(reader);

    size_t        readsize = std::min(size, size_t(mem->length - mem->reader.offset));

    if (!BLI_mmap_read(mem->mmap, buffer, mem->reader.offset, readsize))
    {
        return 0;
    }

    mem->reader.offset += readsize;

    return readsize;
}

static uint64_t memory_seek(FileReader* reader,
                            uint64_t    offset,
                            int         whence)
{
    MemoryReader* mem = reinterpret_cast<MemoryReader*>(reader);

    switch (whence)
    {
        case SEEK_SET:
            mem->reader.offset = offset;
            break;
        case SEEK_CUR:
            mem->reader.offset += offset;
            break;
        case SEEK_END:
            mem->reader.offset = mem->length - offset;
            break;
        default:
            break;
    }

    return mem->reader.offset;
}

static void BLI_mmap_close(BLI_mmap_file* file)
{
    if (file)
    {
        BLI_mmap_close(file);
    }
}

static void memory_close_mmap(FileReader* reader)
{
    MemoryReader* mem = reinterpret_cast<MemoryReader*>(reader);

    BLI_mmap_close(mem->mmap);
    MEM_delete(mem);
}

// implement this in VKI_mmap.h / .cc
BLI_mmap_file* BLI_mmap_open(int fd) ATTR_MALLOC ATTR_WARN_UNUSED_RESULT;

size_t         BLI_mmap_get_length(const BLI_mmap_file* file) ATTR_WARN_UNUSED_RESULT;

size_t         BLI_mmap_get_length(const BLI_mmap_file* file)
{
    return file->length;
}

// implement this in file_reader_memory . h / .cc
FileReader* BLI_filereader_new_mmap(int filedes)
{
    BLI_mmap_file* mmap = BLI_mmap_open(filedes);
    if (mmap == nullptr)
    {
        return nullptr;
    }

    MemoryReader* mem = MEM_new_zeroed<MemoryReader>(__func__);

    mem->mmap         = mmap;
    mem->length       = BLI_mmap_get_length(mmap);

    mem->reader.read  = memory_read_mmap;
    mem->reader.seek  = memory_seek;
    mem->reader.close = memory_close_mmap;

    return reinterpret_cast<FileReader*>(mem);
}

// implement in core_file_reader .h / .cc
FileReader* BLO_file_reader_uncompressed(FileReader* rawfile);

// fileops. h
bool        BLI_file_magic_is_gzip(const char header[4]);
bool        BLI_file_magic_is_zstd(const char header[4]);

bool        BLI_file_magic_is_gzip(const char header[4])
{
    /* GZIP itself starts with the magic bytes 0x1f 0x8b.
     * The third byte indicates the compression method, which is 0x08 for DEFLATE. */
    return header[0] == 0x1f && header[1] == 0x8b && header[2] == 0x08;
}

bool BLI_file_magic_is_zstd(const char header[4])
{
    /* Zstandard files start with the magic bytes 0x28 0xb5 0x2f 0xfd. */
    return header[0] == 0x28 && header[1] == 0xb5 && header[2] == 0x2f && header[3] == 0xfd;
}

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

FileReader* BLO_file_reader_uncompressed_from_descriptor(int filedes)
{
    if (FileReader* mmap_reader = BLI_filereader_new_mmap(filedes))
    {
        /* The mapped memory is still valid even when the file is closed. */
        close(filedes);
        return BLO_file_reader_uncompressed(mmap_reader);
    }
    return BLO_file_reader_uncompressed(BLI_filereader_new_file(filedes));
}

// memory_utils. h
#define BLI_SCOPED_DEFER_NAME1(a, b) a##b
#define BLI_SCOPED_DEFER_NAME2(a, b) BLI_SCOPED_DEFER_NAME1(a, b)
#define BLI_SCOPED_DEFER_NAME(a) BLI_SCOPED_DEFER_NAME2(_scoped_defer_##a##_, __LINE__)

namespace blenlib_detail
{

template <typename Func>
struct ScopedDeferHelper
{
    Func func;

    ~ScopedDeferHelper() { func(); }
};

} // namespace blenlib_detail

#define BLI_SCOPED_DEFER(function_to_defer)                                                                            \
    auto BLI_SCOPED_DEFER_NAME(func) = (function_to_defer);                                                            \
    blenlib_detail::ScopedDeferHelper<decltype(BLI_SCOPED_DEFER_NAME(func))> BLI_SCOPED_DEFER_NAME(helper){            \
        std::move(BLI_SCOPED_DEFER_NAME(func))};

template <typename T>
inline constexpr bool is_trivial_extended_v = std::is_trivial_v<T>;
template <typename T>
inline constexpr bool is_trivially_destructible_extended_v =
    is_trivial_extended_v<T> || std::is_trivially_destructible_v<T>;
template <typename T>
inline constexpr bool is_trivially_copy_constructible_extended_v =
    is_trivial_extended_v<T> || std::is_trivially_copy_constructible_v<T>;
template <typename T>
inline constexpr bool is_trivially_move_constructible_extended_v =
    is_trivial_extended_v<T> || std::is_trivially_move_constructible_v<T>;

template <typename T>
inline bool assign_if_different(T& old_value,
                                T  new_value)
{
    if (old_value != new_value)
    {
        old_value = std::move(new_value);
        return true;
    }
    return false;
}

// core vektor header

// blend core bheade .h
enum class BHeadType
{
    BHead4,
    SmallBHead8,
    LargeBHead8,
};

#define VKEND_FILE_FORMAT_VERSION_0 0

#define VKEND_FILE_FORMAT_VERSION_1 1

/**
 * Only "modern" systems support writing files with #LargeBHead8 headers. Other systems are
 * deprecated. This reduces the amount of variation we have to deal with when reading .blend files.
 */
#define SYSTEM_SUPPORTS_WRITING_FILE_VERSION_1 (ENDIAN_ORDER == L_ENDIAN && sizeof(void*) == 8)

#define MIN_SIZEOFVEKTORHEADER 12
#define MAX_SIZEOFVEKTORHEADER 17

/** See #VKEND_FILE_FORMAT_VERSION_0 for the structure. */
#define SIZEOFVEKTORHEADER_VERSION_0 12
/** See #VKEND_FILE_FORMAT_VERSION_1 for the structure. */
#define SIZEOFVEKTORHEADER_VERSION_1 17

struct BlenderHeader
{
    /** 4 or 8. */
    int       pointer_size;
    /** L_ENDIAN or B_ENDIAN. */
    int       endian;
    /** #VEKTOR_FILE_VERSION. */
    int       file_version;
    /** #VKEND_FILE_FORMAT_VERSION. */
    int       file_format_version;

    BHeadType bhead_type() const;
};

// means the file opened is not a vektor file
struct BlenderHeaderInvalid
{
};

/** The file is detected to be a Blender file, but it could not be decoded successfully. */
struct BlenderHeaderUnknown
{
};

// endine defines hh
#define L_ENDIAN 1
#define B_ENDIAN 0

using BlenderHeaderVariant = std::variant<BlenderHeaderInvalid, BlenderHeaderUnknown, BlenderHeader>;

BlenderHeaderVariant BLO_readfile_vektor_header_decode(FileReader* file)
{
    char    header_bytes[MAX_SIZEOFVEKTORHEADER];
    /* We read the minimal number of header bytes first. If necessary, the remaining bytes are read
     * below. */
    int64_t readsize = file->read(file, header_bytes, MIN_SIZEOFVEKTORHEADER);
    if (readsize != MIN_SIZEOFVEKTORHEADER)
    {
        return BlenderHeaderInvalid{};
    }
    if (!STREQLEN(header_bytes, "VEKTOR", 6))
    {
        return BlenderHeaderInvalid{};
    }
    /* If the first 6 bytes are Vektor, it is very likely that this is a newer version of the
     * blend-file format. If the rest of the decode fails, we can still report that this was a
     * Blender file of a potentially future version. */

    BlenderHeader header;
    /* In the old header format, the next bytes indicate the pointer size. In the new format a
     * version number comes next. */
    const bool    is_legacy_header = ELEM(header_bytes[7], '_', '-');

    if (is_legacy_header)
    {
        header.file_format_version = 0;
        switch (header_bytes[7])
        {
            case '_':
                header.pointer_size = 4;
                break;
            case '-':
                header.pointer_size = 8;
                break;
            default:
                return BlenderHeaderUnknown{};
        }
        switch (header_bytes[8])
        {
            case 'v':
                header.endian = L_ENDIAN;
                break;
            case 'V':
                header.endian = B_ENDIAN;
                break;
            default:
                return BlenderHeaderUnknown{};
        }
        if (!isdigit(header_bytes[9]) || !isdigit(header_bytes[10]) || !isdigit(header_bytes[11]))
        {
            return BlenderHeaderUnknown{};
        }
        char version_str[4];
        memcpy(version_str, header_bytes + 9, 3);
        version_str[3]      = '\0';
        header.file_version = atoi(version_str);
        return header;
    }

    if (!isdigit(header_bytes[7]) || !isdigit(header_bytes[8]))
    {
        return BlenderHeaderUnknown{};
    }
    char header_size_str[3];
    memcpy(header_size_str, header_bytes + 7, 2);
    header_size_str[2]    = '\0';
    const int header_size = atoi(header_size_str);
    if (header_size != MAX_SIZEOFVEKTORHEADER)
    {
        return BlenderHeaderUnknown{};
    }

    /* Read remaining header bytes. */
    const int64_t remaining_bytes_to_read = header_size - MIN_SIZEOFVEKTORHEADER;
    readsize = file->read(file, header_bytes + MIN_SIZEOFVEKTORHEADER, remaining_bytes_to_read);
    if (readsize != remaining_bytes_to_read)
    {
        return BlenderHeaderUnknown{};
    }
    if (header_bytes[9] != '-')
    {
        return BlenderHeaderUnknown{};
    }
    header.pointer_size = 8;
    if (!isdigit(header_bytes[10]) || !isdigit(header_bytes[11]))
    {
        return BlenderHeaderUnknown{};
    }
    char blend_file_version_format_str[3];
    memcpy(blend_file_version_format_str, header_bytes + 10, 2);
    blend_file_version_format_str[2] = '\0';
    header.file_format_version       = atoi(blend_file_version_format_str);
    if (header.file_format_version != 1)
    {
        return BlenderHeaderUnknown{};
    }
    if (header_bytes[12] != 'v')
    {
        return BlenderHeaderUnknown{};
    }
    header.endian = L_ENDIAN;
    if (!isdigit(header_bytes[13]) || !isdigit(header_bytes[14]) || !isdigit(header_bytes[15]) ||
        !isdigit(header_bytes[16]))
    {
        return BlenderHeaderUnknown{};
    }
    char version_str[5];
    memcpy(version_str, header_bytes + 13, 4);
    version_str[4]      = '\0';
    header.file_version = std::atoi(version_str);
    return header;
}

// READING .VEKTOR FILES
static int wm_read_exotic(const char* filepath)
{
    int filepath_len = strlen(filepath);
    if (filepath_len > 0 && ELEM(filepath[filepath_len - 1], '/', '\\'))
    {
        return VKE_READ_EXOTIC_FAIL_PATH;
    }

    /* Open the file. */
    const int filedes = VKE_open(filepath, O_BINARY | O_RDONLY, 0);
    if (filedes == -1)
    {
        return VKE_READ_EXOTIC_FAIL_OPEN;
    }
    FileReader* file = BLO_file_reader_uncompressed_from_descriptor(filedes);
    if (!file)
    {
        return VKE_READ_EXOTIC_FAIL_FORMAT;
    }
    BLI_SCOPED_DEFER([&]() { file->close(file); });
    const BlenderHeaderVariant header_variant = BLO_readfile_vektor_header_decode(file);
    if (std::holds_alternative<BlenderHeader>(header_variant))
    {
        return VKE_READ_EXOTIC_OK_VKEND;
    }
    return VKE_READ_EXOTIC_FAIL_FORMAT;
};

// blo read file .hh

// BLI linked list lock free .hh
struct LockfreeLinkNode
{
    struct LockfreeLinkNode* next;
    /* NOTE: "Subclass" this structure to add custom-defined data. */
};

// bli linkedlist .hh

static void wm_file_read_pre(bool use_data,
                             bool /*use_userdef*/)
{
    if (use_data)
    {
        VKE_timer_on_file_load();
    }

    /* Always do this as both startup and preferences may have loaded in many font's
     * at a different zoom level to the file being loaded. */
    // ui::view2d_zoom_cache_reset();

    // ED_preview_restart_queue_free();
}

static void wm_file_read_setup_wm_substitute_old_window(wmWindowManager* oldwm,
                                                        wmWindowManager* wm,
                                                        wmWindow*        oldwin,
                                                        wmWindow*        win)
{
    win->runtime->glfw_win = oldwin->runtime->glfw_win;
    win->runtime->gpuctx   = oldwin->runtime->gpuctx;
    win->active            = oldwin->active;
    if (win->active)
    {
        wm->runtime->winactive = win;
    }
    if (oldwm->runtime->windrawable == oldwin)
    {
        oldwm->runtime->windrawable = nullptr;
        wm->runtime->windrawable    = win;
    }

    /* File loading in background mode still calls this. */
    if (!G.background)
    {
        /* Pointer back. */
        GLFW_IWindow* ghost_window = static_cast<GLFW_IWindow*>(win->runtime->glfw_win);
        ghost_window->set_user_data(win);
    }

    oldwin->runtime->glfw_win           = nullptr;
    oldwin->runtime->gpuctx             = nullptr;

    win->runtime->eventstate            = oldwin->runtime->eventstate;
    win->runtime->event_last_handled    = oldwin->runtime->event_last_handled;
    oldwin->runtime->eventstate         = nullptr;
    oldwin->runtime->event_last_handled = nullptr;

    /* Ensure proper screen re-scaling. */
    win->sizex                          = oldwin->sizex;
    win->sizey                          = oldwin->sizey;
    win->posx                           = oldwin->posx;
    win->posy                           = oldwin->posy;
}

void WM_event_timer_remove(wmWindowManager* wm,
                           wmWindow* /*win*/,
                           wmTimer* timer)
{
    /* Extra security check. */
    if (BLI_findindex(&wm->runtime->timers, timer) == -1)
    {
        return;
    }

    timer->flags = WM_TIMER_TAGGED_FOR_REMOVAL;

    /* Clear existing references to the timer. */
    if (wm->runtime->reports.reporttimer == timer)
    {
        wm->runtime->reports.reporttimer = nullptr;
    }
    /* There might be events in queue with this timer as customdata. */
    for (wmWindow& win : wm->windows)
    {
        for (wmEvent& event : win.runtime->event_queue)
        {
            if (event.customdata == timer)
            {
                event.customdata = nullptr;
                event.type       = EVENT_NONE; /* Timer users customdata, don't want `nullptr == nullptr`. */
            }
        }
    }

    /* Immediately free `customdata` if requested, so that invalid usages of that data after
     * calling `WM_event_timer_remove` can be easily spotted (through ASAN errors e.g.). */
    WM_event_timer_free_data(timer);
}

void wm_autosave_timer_end(wmWindowManager* wm)
{
    if (wm->autosavetimer)
    {
        WM_event_timer_remove(wm, nullptr, wm->autosavetimer);
        wm->autosavetimer = nullptr;
    }
}

// list base .cc

void BLI_remlink(ListBase* listbase,
                 void*     vlink)
{
    Link* link = static_cast<Link*>(vlink);

    if (link == nullptr)
    {
        return;
    }

    if (link->next)
    {
        link->next->prev = link->prev;
    }
    if (link->prev)
    {
        link->prev->next = link->next;
    }

    if (listbase->last == link)
    {
        listbase->last = link->prev;
    }
    if (listbase->first == link)
    {
        listbase->first = link->next;
    }
}

void* BLI_pophead(ListBase* listbase)
{
    Link* link = static_cast<Link*>(listbase->first);
    if (link)
    {
        BLI_remlink(listbase, link);
    }
    return link;
}

// list base .cc
#define LISTBASE_FOREACH(type, var, list)                                                                              \
    for (type var = (type)((list)->first); var != nullptr; var = (type)(((Link*)(var))->next))

void* BLI_findptr(const ListBase* listbase,
                  const void*     ptr,
                  const int       offset)
{
    LISTBASE_FOREACH(Link*, link, listbase)
    {
        /* Exact copy of #BLI_findstring(), except for this line. */
        const void* ptr_iter =
            *(reinterpret_cast<const void**>(const_cast<char*>(((reinterpret_cast<const char*>(link)) + offset))));
        if (ptr == ptr_iter)
        {
            return link;
        }
    }
    return nullptr;
}

// workspace .cc
static void* workspace_relation_get_data_matching_parent(const ListBaseT<WorkSpaceDataRelation>* relation_list,
                                                         const void*                             parent)
{
    WorkSpaceDataRelation* relation = static_cast<WorkSpaceDataRelation*>(
        BLI_findptr(relation_list, parent, offsetof(WorkSpaceDataRelation, parent)));
    if (relation != nullptr)
    {
        return relation->value;
    }

    return nullptr;
}

// workspace .h
void VKE_workspace_active_set(WorkSpaceInstanceHook* hook,
                              WorkSpace*             workspace)
{
    /* DO NOT check for `hook->active == workspace` here. Caller code is supposed to do it if
     * that optimization is possible and needed.
     * This code can be called from places where we might have this equality, but still want to
     * ensure/update the active layout below.
     * Known case where this is buggy and will crash later due to nullptr active layout: reading
     * a blend file, when the new read workspace ID happens to have the exact same memory address
     * as when it was saved in the blend file (extremely unlikely, but possible). */

    hook->active = workspace;
    if (workspace)
    {
        WorkSpaceLayout* layout = static_cast<WorkSpaceLayout*>(
            workspace_relation_get_data_matching_parent(&workspace->hook_layout_relations, hook));
        if (layout)
        {
            hook->act_layout = layout;
        }
    }
}

// workspace .h
static WorkSpaceLayout* workspace_layout_find_exec(const WorkSpace* workspace,
                                                   const bScreen*   screen)
{
    return static_cast<WorkSpaceLayout*>(BLI_findptr(&workspace->layouts, screen, offsetof(WorkSpaceLayout, screen)));
}

// workspace .cc
WorkSpaceLayout* VKE_workspace_layout_find_global(const Main*    bmain,
                                                  const bScreen* screen,
                                                  WorkSpace**    r_workspace)
{
    if (r_workspace)
    {
        *r_workspace = nullptr;
    }

    for (WorkSpace* workspace = static_cast<WorkSpace*>(bmain->workspaces.first); workspace;
         workspace            = static_cast<WorkSpace*>(workspace->id.next))
    {
        WorkSpaceLayout* layout = workspace_layout_find_exec(workspace, screen);
        if (layout)
        {
            if (r_workspace)
            {
                *r_workspace = workspace;
            }

            return layout;
        }
    }

    return nullptr;
}

WorkSpaceLayout* VKE_workspace_layout_find(const WorkSpace* workspace,
                                           const bScreen*   screen)
{
    WorkSpaceLayout* layout = workspace_layout_find_exec(workspace, screen);
    if (layout)
    {
        return layout;
    }

    printf("%s: Couldn't find layout in this workspace: '%s' screen: '%s'. "
           "This should not happen!\n",
           __func__, workspace->id.name + 2, screen->id.name + 2);

    return nullptr;
}

// move to listbase cc
void* BLI_listbase_bytes_find(const ListBase* listbase,
                              const void*     bytes,
                              const size_t    bytes_size,
                              const int       offset)
{
    LISTBASE_FOREACH(Link*, link, listbase)
    {
        const void* ptr_iter = static_cast<const void*>((reinterpret_cast<const char*>(link)) + offset);
        if (memcmp(bytes, ptr_iter, bytes_size) == 0)
        {
            return link;
        }
    }

    return nullptr;
}

static void workspace_relation_add(ListBaseT<WorkSpaceDataRelation>* relation_list,
                                   void*                             parent,
                                   const int                         parentid,
                                   void*                             data)
{
    WorkSpaceDataRelation* relation = MEM_new<WorkSpaceDataRelation>(__func__);
    relation->parent                = parent;
    relation->parentid              = parentid;
    relation->value                 = data;

    BLI_addhead(relation_list, relation);
}

static void workspace_relation_ensure_updated(ListBaseT<WorkSpaceDataRelation>* relation_list,
                                              void*                             parent,
                                              const int                         parentid,
                                              void*                             data)
{
    WorkSpaceDataRelation* relation = static_cast<WorkSpaceDataRelation*>(
        BLI_listbase_bytes_find(relation_list, &parentid, sizeof(parentid), offsetof(WorkSpaceDataRelation, parentid)));
    if (relation != nullptr)
    {
        relation->parent = parent;
        relation->value  = data;
        /* reinsert at the head of the list, so that more commonly used relations are found faster. */
        BLI_remlink(relation_list, relation);
        BLI_addhead(relation_list, relation);
    }
    else
    {
        /* no matching relation found, add new one */
        workspace_relation_add(relation_list, parent, parentid, data);
    }
}

void VKE_workspace_active_layout_set(WorkSpaceInstanceHook* hook,
                                     const int              winid,
                                     WorkSpace*             workspace,
                                     WorkSpaceLayout*       layout)
{
    hook->act_layout = layout;
    workspace_relation_ensure_updated(&workspace->hook_layout_relations, hook, winid, layout);
}

void VKE_workspace_active_screen_set(WorkSpaceInstanceHook* hook,
                                     const int              winid,
                                     WorkSpace*             workspace,
                                     bScreen*               screen)
{
    /* we need to find the WorkspaceLayout that wraps this screen */
    WorkSpaceLayout* layout = VKE_workspace_layout_find(hook->active, screen);
    VKE_workspace_active_layout_set(hook, winid, workspace, layout);
}

// wmapi.cc
static void wm_reports_free(wmWindowManager* wm)
{
    WM_event_timer_remove(wm, nullptr, wm->runtime->reports.reporttimer);
}

void CTX_wm_manager_set(vkContext*       C,
                        wmWindowManager* wm)
{
    C->wm.manager = wm;
    C->wm.window  = nullptr;
    C->wm.screen  = nullptr;
    C->wm.area    = nullptr;
    C->wm.region  = nullptr;
}

void wm_close_and_free(vkContext*       C,
                       wmWindowManager* wm)
{
    if (wm->autosavetimer)
    {
        wm_autosave_timer_end(wm);
    }

    while (wmWindow* win = static_cast<wmWindow*>(BLI_pophead(&wm->windows)))
    {
        VKE_workspace_active_set(win->workspace_hook, nullptr);
        wm_window_free(C, wm, win);
    }

    wm_reports_free(wm);

    if (C && CTX_wm_manager(C) == wm)
    {
        CTX_wm_manager_set(C, nullptr);
    }

    MEM_delete(wm->runtime);
}

void VKE_libblock_free_data(ID*        id,
                            const bool do_id_user)
{
    // if (id->properties)
    // {
    //     IDP_FreePropertyContent_ex(id->properties, do_id_user);
    //     MEM_delete(id->properties);
    //     id->properties = nullptr;
    // }
    // if (id->system_properties)
    // {
    //     IDP_FreePropertyContent_ex(id->system_properties, do_id_user);
    //     MEM_delete(id->system_properties);
    //     id->system_properties = nullptr;
    // }

    // if (id->override_library)
    // {
    //     VKE_lib_override_library_free(&id->override_library, do_id_user);
    //     id->override_library = nullptr;
    // }

    // if (id->asset_data)
    // {
    //     VKE_asset_metadata_free(&id->asset_data);
    // }

    // if (id->library_weak_reference != nullptr)
    // {
    //     MEM_delete(id->library_weak_reference);
    // }

    // VKE_animdata_free(id, do_id_user);

    VKE_libblock_free_runtime_data(id);
}

void wm_file_read_setup_wm_use_new(vkContext* C,
                                   Main* /*bmain*/,
                                   BlendFileReadWMSetupData* wm_setup_data,
                                   wmWindowManager*          wm)
{
    wmWindowManager* old_wm  = wm_setup_data->old_wm;

    wm->op_undo_depth        = old_wm->op_undo_depth;

    /* Move existing key configurations into the new WM. */
    wm->runtime->keyconfigs  = old_wm->runtime->keyconfigs;
    wm->runtime->addonconf   = old_wm->runtime->addonconf;
    wm->runtime->defaultconf = old_wm->runtime->defaultconf;
    wm->runtime->userconf    = old_wm->runtime->userconf;

    BLI_listbase_clear(&old_wm->runtime->keyconfigs);
    old_wm->runtime->addonconf   = nullptr;
    old_wm->runtime->defaultconf = nullptr;
    old_wm->runtime->userconf    = nullptr;

    /* Ensure new keymaps are made, and space types are set. */
    wm->init_flag                = 0;
    wm->runtime->winactive       = nullptr;

    /* Clearing drawable of old WM before deleting any context to avoid clearing the wrong wm. */
    wm_window_clear_drawable(old_wm);

    bool has_match = false;
    for (wmWindow& win : wm->windows)
    {
        for (wmWindow& old_win : old_wm->windows)
        {
            if (old_win.winid == win.winid)
            {
                has_match = true;

                wm_file_read_setup_wm_substitute_old_window(old_wm, wm, &old_win, &win);
            }
        }
    }
    /* Ensure that at least one window is kept open so we don't lose the context, see #42303. */
    if (!has_match)
    {
        wm_file_read_setup_wm_substitute_old_window(old_wm, wm, static_cast<wmWindow*>(old_wm->windows.first),
                                                    static_cast<wmWindow*>(wm->windows.first));
    }

    wm_setup_data->old_wm = nullptr;
    wm_close_and_free(C, old_wm);
    /* Don't handle user counts as this is only ever called once #G_MAIN has already been freed via
     * #VKE_main_free so any access to ID's referenced by the window-manager (from ID properties)
     * will crash. See: #100703. */
    VKE_libblock_free_data(&old_wm->id, false);
    VKE_libblock_free_data_py(&old_wm->id);
    MEM_delete(old_wm);
}

void wm_file_reads_setup_wm_use_new(vkContext* C,
                                    Main* /*bmain*/,
                                    BlendFileReadWMSetupData* wm_setup_data,
                                    wmWindowManager*          wm)
{
    wmWindowManager* old_wm  = wm_setup_data->old_wm;

    wm->op_undo_depth        = old_wm->op_undo_depth;

    /* Move existing key configurations into the new WM. */
    wm->runtime->keyconfigs  = old_wm->runtime->keyconfigs;
    wm->runtime->addonconf   = old_wm->runtime->addonconf;
    wm->runtime->defaultconf = old_wm->runtime->defaultconf;
    wm->runtime->userconf    = old_wm->runtime->userconf;

    BLI_listbase_clear(&old_wm->runtime->keyconfigs);
    old_wm->runtime->addonconf   = nullptr;
    old_wm->runtime->defaultconf = nullptr;
    old_wm->runtime->userconf    = nullptr;

    /* Ensure new keymaps are made, and space types are set. */
    wm->init_flag                = 0;
    wm->runtime->winactive       = nullptr;

    /* Clearing drawable of old WM before deleting any context to avoid clearing the wrong wm. */
    wm_window_clear_drawable(old_wm);

    bool has_match = false;
    for (wmWindow& win : wm->windows)
    {
        for (wmWindow& old_win : old_wm->windows)
        {
            if (old_win.winid == win.winid)
            {
                has_match = true;

                wm_file_read_setup_wm_substitute_old_window(old_wm, wm, &old_win, &win);
            }
        }
    }
    /* Ensure that at least one window is kept open so we don't lose the context, see #42303. */
    if (!has_match)
    {
        wm_file_read_setup_wm_substitute_old_window(old_wm, wm, static_cast<wmWindow*>(old_wm->windows.first),
                                                    static_cast<wmWindow*>(wm->windows.first));
    }

    wm_setup_data->old_wm = nullptr;
    wm_close_and_free(C, old_wm);
    /* Don't handle user counts as this is only ever called once #G_MAIN has already been freed via
     * #VKE_main_free so any access to ID's referenced by the window-manager (from ID properties)
     * will crash. See: #100703. */
    VKE_libblock_free_data(&old_wm->id, false);
    VKE_libblock_free_data_py(&old_wm->id);
    MEM_delete(old_wm);
}

bScreen* VKE_workspace_layout_screen_get(const WorkSpaceLayout* layout)
{
    return layout->screen;
}

const char* VKE_workspace_layout_name_get(const WorkSpaceLayout* layout)
{
    return layout->name;
}

// implment in screen.cc
bool VKE_screen_is_fullscreen_area(const bScreen* screen)
{
    return ELEM(screen->state, SCREENMAXIMIZED, SCREENFULL);
}

// idtype.hh
using IDTypeCopyDataFunction = void (*)(Main*                   bmain,
                                        std::optional<Library*> owner_library,
                                        ID*                     id_dst,
                                        const ID*               id_src,
                                        int                     flag);

// lib_type. hh

// id type. h
template <class T>
static constexpr T InvalidPointer()
{
    return reinterpret_cast<T>(UINTPTR_MAX);
}

struct IDTypeInfo
{
    /* ********** General IDType data. ********** */

    /**
     * Unique identifier of this type, either as a short or an array of two chars, see
     * DNA_ID_enums.h's ID_XX enums.
     */
    short                  id_code               = ID_LINK_PLACEHOLDER;
    /**
     * Bit-flag matching id_code, used for filtering (e.g. in file browser), see DNA_ID.h's
     * FILTER_ID_XX enums.
     */
    uint64_t               id_filter             = 0;

    const char*            name                  = nullptr;

    /**
     * Known types of ID dependencies.
     *
     * Used by #VKE_library_id_can_use_filter_id, together with additional runtime heuristics, to
     * generate a filter value containing only ID types that given ID could be using.
     */
    uint64_t               dependencies_id_types = 0;

    /**
     * Define the position of this data-block type in the virtual list of all data in a Main that is
     * returned by `VKE_main_lists_get()`.
     * Very important, this has to be unique and below INDEX_ID_MAX, see DNA_ID.h.
     */
    int                    main_listbase_index   = INDEX_ID_MAX;

    /** Memory size of a data-block of that type. */
    size_t                 struct_size           = 0;

    uint32_t               flags                 = 0;

    IDTypeCopyDataFunction copy_data             = InvalidPointer<IDTypeCopyDataFunction>();
};

// implement in id_type.hh
const IDTypeInfo*                            VKE_idtype_get_info_from_id(const ID* id);
const IDTypeInfo*                            VKE_idtype_get_info_from_idcode(short id_code);
const IDTypeInfo*                            VKE_idtype_get_info_from_idtype_index(const int idtype_index);

static std::array<IDTypeInfo*, INDEX_ID_MAX> id_types;

int                                          VKE_idtype_idcode_to_index(short idcode);

int                                          VKE_idtype_idcode_to_index(const short idcode)
{
#define CASE_IDINDEX(_id)                                                                                              \
    case ID_##_id:                                                                                                     \
        return INDEX_ID_##_id

    switch (ID_Type(idcode))
    {
        CASE_IDINDEX(AC);
        CASE_IDINDEX(AR);
        CASE_IDINDEX(BR);
        CASE_IDINDEX(CA);
        CASE_IDINDEX(CF);
        CASE_IDINDEX(CU_LEGACY);
        CASE_IDINDEX(GD_LEGACY);
        CASE_IDINDEX(GP);
        CASE_IDINDEX(GR);
        CASE_IDINDEX(CV);
        CASE_IDINDEX(IM);
        CASE_IDINDEX(KE);
        CASE_IDINDEX(LA);
        CASE_IDINDEX(LI);
        CASE_IDINDEX(LS);
        CASE_IDINDEX(LT);
        CASE_IDINDEX(MA);
        CASE_IDINDEX(MB);
        CASE_IDINDEX(MC);
        CASE_IDINDEX(ME);
        CASE_IDINDEX(MSK);
        CASE_IDINDEX(NT);
        CASE_IDINDEX(OB);
        CASE_IDINDEX(PA);
        CASE_IDINDEX(PAL);
        CASE_IDINDEX(PC);
        CASE_IDINDEX(PT);
        CASE_IDINDEX(LP);
        CASE_IDINDEX(SCE);
        CASE_IDINDEX(SCR);
        CASE_IDINDEX(SPK);
        CASE_IDINDEX(SO);
        CASE_IDINDEX(TE);
        CASE_IDINDEX(TXT);
        CASE_IDINDEX(VF);
        CASE_IDINDEX(VO);
        CASE_IDINDEX(WM);
        CASE_IDINDEX(WO);
        CASE_IDINDEX(WS);
    }

    /* Special naughty boy... */
    if (idcode == ID_LINK_PLACEHOLDER)
    {
        return INDEX_ID_NULL;
    }

    return -1;

#undef CASE_IDINDEX
}

const IDTypeInfo* VKE_idtype_get_info_from_idtype_index(const int idtype_index)
{
    if (idtype_index >= 0 && idtype_index < int(id_types.size()))
    {
        const IDTypeInfo* id_type = id_types[size_t(idtype_index)];
        if (id_type && id_type->name[0] != '\0')
        {
            // BLI_assert_msg(VKE_idtype_idcode_to_index(id_type->id_code) == idtype_index,
            //                "Critical inconsistency in ID type information");
            return id_type;
        }
    }

    return nullptr;
}

const IDTypeInfo* VKE_idtype_get_info_from_idcode(const short id_code)
{
    return VKE_idtype_get_info_from_idtype_index(VKE_idtype_idcode_to_index(id_code));
}

const IDTypeInfo* VKE_idtype_get_info_from_id(const ID* id)
{
    return VKE_idtype_get_info_from_idcode((ID_Type)(*((const short*)(id->name))));
}

// id_type.h

enum
{
    IDTYPE_FLAGS_NO_COPY  = 1 << 0,
    LIB_ID_CREATE_NO_MAIN = 1 << 0,
};

void VKE_libblock_copy_in_lib(Main*                    bmain,
                              std::optional<Library*>  owner_library,
                              const ID*                id,
                              std::optional<const ID*> new_owner_id,
                              ID**                     new_id_p,
                              const int                orig_flag)
{
    ID*        new_id         = *new_id_p;
    int        flag           = orig_flag;

    const bool is_embedded_id = (id->flag & ID_FLAG_EMBEDDED_DATA) != 0;

    // BLI_assert((flag & LIB_ID_CREATE_NO_MAIN) != 0 || bmain != nullptr);
    // BLI_assert((flag & LIB_ID_CREATE_NO_MAIN) != 0 || (flag & LIB_ID_CREATE_NO_ALLOCATE) == 0);
    // BLI_assert((flag & LIB_ID_CREATE_NO_MAIN) != 0 || (flag & LIB_ID_CREATE_LOCAL) == 0);

    if (bmain != nullptr && is_embedded_id)
    {
        flag |= LIB_ID_CREATE_NO_MAIN;
    }

    // /* 1. Allocation & Initialization */
    // if (flag & LIB_ID_CREATE_NO_ALLOCATE)
    // {
    //     const size_t size = VKE_libblock_get_alloc_info(GS(id->name), nullptr);
    //     memset(new_id, 0, size);
    //     VKE_libblock_runtime_ensure(*new_id);
    //     STRNCPY(new_id->name, id->name);
    //     new_id->us = 0;
    //     new_id->tag |= ID_TAG_NOT_ALLOCATED | ID_TAG_NO_MAIN | ID_TAG_NO_USER_REFCOUNT;
    //     new_id->lib = owner_library ? *owner_library : id->lib;
    // }
    // else
    // {
    //     new_id =
    //         static_cast<ID*>(VKE_libblock_alloc_in_lib(bmain, owner_library, GS(id->name), VKE_id_name(*id), flag));
    // }
    // BLI_assert(new_id != nullptr);

    // /* 2. Copy struct data */
    // const size_t id_len = VKE_libblock_get_alloc_info(GS(new_id->name), nullptr);
    // if (id_len > sizeof(ID))
    // {
    //     memcpy(reinterpret_cast<char*>(new_id) + sizeof(ID), reinterpret_cast<const char*>(id) + sizeof(ID),
    //            id_len - sizeof(ID));
    // }

    // /* 3. Flags and Tags Mapping */
    // if (flag & LIB_ID_COPY_SET_COPIED_ON_WRITE)
    // {
    //     new_id->tag |= ID_TAG_COPIED_ON_EVAL;
    // }
    // else
    // {
    //     new_id->tag &= ~ID_TAG_COPIED_ON_EVAL;
    // }

    // new_id->flag       = (new_id->flag & ~ID_FLAG_EMBEDDED_DATA) | (id->flag & ID_FLAG_EMBEDDED_DATA);
    // const int tag_mask = (owner_library && *owner_library) ? (ID_TAG_EXTERN | ID_TAG_INDIRECT) : 0;
    // new_id->tag        = (new_id->tag & ~tag_mask) | (id->tag & tag_mask);

    // if (is_embedded_id && (orig_flag & LIB_ID_CREATE_NO_MAIN) == 0)
    // {
    //     new_id->tag &= ~ID_TAG_NO_MAIN;
    // }

    // /* 4. Re-parent Embedded IDs */
    // if (new_owner_id.has_value())
    // {
    //     if (ID** owner_p = VKE_idtype_get_info_from_id(new_id)->owner_pointer_get(new_id, false))
    //     {
    //         *owner_p = const_cast<ID*>(*new_owner_id);
    //         if (*new_owner_id == nullptr)
    //         {
    //             new_id->flag &= ~ID_FLAG_EMBEDDED_DATA;
    //         }
    //     }
    // }

    // /* 5. Sub-data & Property Duplication */
    // const int copy_data_flag = orig_flag | LIB_ID_CREATE_NO_USER_REFCOUNT;

    // if (id->properties)
    // {
    //     new_id->properties = IDP_CopyProperty_ex(id->properties, copy_data_flag);
    // }
    // if (id->system_properties)
    // {
    //     new_id->system_properties = IDP_CopyProperty_ex(id->system_properties, copy_data_flag);
    // }

    // new_id->library_weak_reference = nullptr;

    // if ((orig_flag & LIB_ID_COPY_NO_LIB_OVERRIDE) == 0)
    // {
    //     if (ID_IS_OVERRIDE_LIBRARY_REAL(id))
    //     {
    //         VKE_lib_override_library_copy(new_id, id, false);
    //     }
    //     else if (ID_IS_OVERRIDE_LIBRARY_VIRTUAL(id))
    //     {
    //         new_id->flag |= ID_FLAG_EMBEDDED_DATA_LIB_OVERRIDE;
    //     }
    // }

    // if (id_can_have_animdata(new_id))
    // {
    //     IdAdtTemplate* iat = reinterpret_cast<IdAdtTemplate*>(new_id);
    //     iat->adt           = (flag & LIB_ID_COPY_NO_ANIMDATA) == 0
    //                              ? VKE_animdata_copy_in_lib(bmain, owner_library, iat->adt, copy_data_flag)
    //                              : nullptr;
    // }

    // if ((flag & LIB_ID_COPY_ASSET_METADATA) && id->asset_data)
    // {
    //     new_id->asset_data = VKE_asset_metadata_copy(id->asset_data);
    // }

    // /* 6. Depsgraph & Internal State Flags */
    // if ((flag & LIB_ID_CREATE_NO_DEG_TAG) == 0 && (flag & LIB_ID_CREATE_NO_MAIN) == 0)
    // {
    //     DEG_id_type_tag(bmain, GS(new_id->name));
    // }

    // if (owner_library && *owner_library && ((*owner_library)->flag & LIBRARY_FLAG_IS_ARCHIVE))
    // {
    //     new_id->flag |= ID_FLAG_LINKED_AND_PACKED;
    // }

    // if (flag & LIB_ID_COPY_ID_NEW_SET)
    // {
    //     ID_NEW_SET(const_cast<ID*>(id), new_id);
    // }

    *new_id_p = new_id;
}

struct IDCopyLibManagementData
{
    const ID* id_src;
    ID*       id_dst;
    int       flag;
};

enum LibraryForeachIDCallbackFlag
{
    IDWALK_CB_NOP                  = 0,
    IDWALK_CB_NEVER_NULL           = (1 << 0),
    IDWALK_CB_NEVER_SELF           = (1 << 1),
    IDWALK_IGNORE_MISSING_OWNER_ID = (1 << 6),

};

struct LibraryIDLinkCallbackData
{
    void*                        user_data;
    /** Main database used to call `VKE_library_foreach_ID_link()`. */
    Main*                        bmain;
    /**
     * 'Real' ID, the one that might be in bmain, only differs from self_id when the later is an
     * embedded one.
     */
    ID*                          owner_id;
    /**
     * ID from which the current ID pointer is being processed. It may be an embedded ID like master
     * collection or root node tree.
     */
    ID*                          self_id;
    ID**                         id_pointer;
    LibraryForeachIDCallbackFlag cb_flag;
};

void id_us_plus_no_lib(ID* id)
{
    if (id)
    {
        if ((id->tag & ID_TAG_EXTRAUSER) && (id->tag & ID_TAG_EXTRAUSER_SET))
        {
            // BLI_assert(id->us >= 1);
            /* No need to increase count, just tag extra user as no more set.
             * Avoids annoying & inconsistent +1 in user count. */
            id->tag &= ~ID_TAG_EXTRAUSER_SET;
        }
        else
        {
            // BLI_assert(id->us >= 0);
            id->us++;
        }
    }
}

void id_lib_extern(ID* id)
{
    if (id && ID_IS_LINKED(id))
    {
        // BLI_assert(VKE_idtype_idcode_is_linkable(GS(id->name)));
        if (id->tag & ID_TAG_INDIRECT)
        {
            id->tag &= ~ID_TAG_INDIRECT;
            id->flag &= ~ID_FLAG_INDIRECT_WEAK_LINK;
            id->tag |= ID_TAG_EXTERN;
            id->lib->runtime->parent = nullptr;
        }
    }
}

void id_us_plus(ID* id)
{
    if (id)
    {
        id_us_plus_no_lib(id);
        id_lib_extern(id);
    }
}

// lib query bke. hh

enum
{
    IDWALK_RET_NOP            = 0,
    /**
     * Completely stop iteration.
     *
     * \note Should never be returned by a callback in case #IDWALK_READONLY is not set.
     */
    IDWALK_RET_STOP_ITER      = 1 << 0,
    /**
     * Stop recursion, that is, do not loop over ID used by current one.
     *
     * \note Should never be returned by a callback in case #IDWALK_READONLY is not set.
     */
    IDWALK_RET_STOP_RECURSION = 1 << 1,
};

static int id_copy_libmanagement_cb(LibraryIDLinkCallbackData* cb_data)
{
    ID**                               id_pointer = cb_data->id_pointer;
    ID*                                id         = *id_pointer;
    const LibraryForeachIDCallbackFlag cb_flag    = cb_data->cb_flag;
    IDCopyLibManagementData*           data       = static_cast<IDCopyLibManagementData*>(cb_data->user_data);

    /* Remap self-references to new copied ID. */
    if (id == data->id_src)
    {
        /* We cannot use self_id here, it is not *always* id_dst (thanks to confounded node-trees!). */
        id = *id_pointer = data->id_dst;
    }

    /* Increase used IDs refcount if needed and required. */
    if (data->flag) // & LIB_ID_CREATE_NO_USER_REFCOUNT) == 0 && (cb_flag & IDWALK_CB_USER))
    {
        if ((data->flag & LIB_ID_CREATE_NO_MAIN) != 0)
        {
            // BLI_assert(cb_data->self_id->tag & ID_TAG_NO_MAIN);
            id_us_plus_no_lib(id);
        }
        else if (ID_IS_LINKED(cb_data->owner_id))
        {
            /* Do not mark copied ID as directly linked, if its current user is also linked data (which
             * is now fairly common when using 'copy_in_lib' feature). */
            id_us_plus_no_lib(id);
        }
        else
        {
            id_us_plus(id);
        }
    }

    return IDWALK_RET_NOP;
}

// lib. qyery h
// void VKE_library_foreach_ID_link(Main*                              bmain,
//                                  ID*                                id,
//                                  FunctionRef<LibraryIDLinkCallback> callback,
//                                  void*                              user_data,
//                                  LibraryForeachIDFlag               flag);

ID* VKE_id_copy_in_lib(Main*                    bmain,
                       std::optional<Library*>  owner_library,
                       const ID*                id,
                       std::optional<const ID*> new_owner_id,
                       ID**                     new_id_p,
                       const int                flag)
{
    ID* newid = (new_id_p != nullptr) ? *new_id_p : nullptr;
    // BLI_assert_msg(newid || (flag & LIB_ID_CREATE_NO_ALLOCATE) == 0,
    //                "Copying with 'no allocate' behavior should always get a non-null new ID buffer");

    /* Make sure destination pointer is all good. */
    if ((flag & LIB_ID_CREATE_NO_ALLOCATE) == 0)
    {
        newid = nullptr;
    }
    else
    {
        if (!newid)
        {
            /* Invalid case, already caught by the assert above. */
            return nullptr;
        }
    }

    /* Early output if source is nullptr. */
    if (id == nullptr)
    {
        return nullptr;
    }

    const IDTypeInfo* idtype_info = VKE_idtype_get_info_from_id(id);

    if (idtype_info != nullptr)
    {
        if ((idtype_info->flags & IDTYPE_FLAGS_NO_COPY) != 0)
        {
            return nullptr;
        }

        VKE_libblock_copy_in_lib(bmain, owner_library, id, new_owner_id, &newid, flag);

        if (idtype_info->copy_data != nullptr)
        {
            idtype_info->copy_data(bmain, owner_library, newid, id, flag);
        }
    }
    else
    {
        // BLI_assert_msg(0, "IDType Missing IDTypeInfo");
    }

    // BLI_assert_msg(newid, "Could not get an allocated new ID to copy into");
    if (!newid)
    {
        return nullptr;
    }

    /* Update ID refcount, remap pointers to self in new ID. */
    IDCopyLibManagementData data{};
    data.id_src = id;
    data.id_dst = newid;
    data.flag   = flag;
    /* When copying an embedded ID, typically at this point its owner ID pointer will still point to
     * the owner of the source, this code has no access to its valid (i.e. destination) owner. This
     * can be added at some point if needed, but currently the #id_copy_libmanagement_cb callback
     * does need this information. */
    // VKE_library_foreach_ID_link(bmain, newid, id_copy_libmanagement_cb, &data, IDWALK_IGNORE_MISSING_OWNER_ID);

    /* FIXME: Check if this code can be moved in #VKE_libblock_copy_in_lib ? Would feel more fitted
     * there, having library handling split between both functions does not look good. */
    /* Do not make new copy local in case we are copying outside of main...
     * XXX TODO: is this behavior OK, or should we need a separate flag to control that? */
    if ((flag & LIB_ID_CREATE_NO_MAIN) == 0)
    {
        // BLI_assert(!owner_library || newid->lib == *owner_library);
        /* If the ID was copied into a library, ensure paths are properly remapped, and that it has a
         * 'linked' tag set. */
        if (ID_IS_LINKED(newid))
        {
            if (newid->lib != id->lib)
            {
                // lib_id_library_local_paths(bmain, newid->lib, id->lib, newid);
            }
            if ((newid->tag & (ID_TAG_EXTERN | ID_TAG_INDIRECT)) == 0)
            {
                newid->tag |= ID_TAG_EXTERN;
            }
        }
        /* Expanding local linked ID usages should never be needed with embedded IDs - this will be
         * handled together with their owner ID copying code. */
        else if ((newid->flag & ID_FLAG_EMBEDDED_DATA) == 0)
        {
            // lib_id_copy_ensure_local(bmain, id, newid, 0);
        }
    }

    else
    {
        /* NOTE: Do not call `ensure_local` for IDs copied outside of Main, even if they do become
         * local.
         *
         * Most of the time, this would not be the desired behavior currently.
         *
         * In the few cases where this is actually needed (e.g. from liboverride resync code, see
         * #lib_override_library_create_from), calling code is responsible for this. */
        newid->lib = owner_library ? *owner_library : id->lib;
    }

    if (new_id_p != nullptr)
    {
        *new_id_p = newid;
    }

    return newid;
}

static void workspace_layout_name_set(WorkSpace*       workspace,
                                      WorkSpaceLayout* layout,
                                      const char*      new_name)
{
    // STRNCPY(layout->name, new_name);
    // BLI_uniquename(&workspace->layouts, layout, "Layout", '.', offsetof(WorkSpaceLayout, name),
    // sizeof(layout->name));
}

WorkSpaceLayout* VKE_workspace_layout_add(Main*       bmain,
                                          WorkSpace&  workspace,
                                          bScreen&    screen,
                                          const char* name);

WorkSpaceLayout* VKE_workspace_layout_add(Main*       bmain,
                                          WorkSpace&  workspace,
                                          bScreen&    screen,
                                          const char* name)
{
    WorkSpaceLayout* layout = MEM_new<WorkSpaceLayout>(__func__);

    //     BLI_assert(!bmain || !workspaces_is_screen_used(bmain, &screen));
    // #ifdef NDEBUG
    //     UNUSED_VARS(bmain);
    // #endif
    layout->screen          = &screen;
    id_us_plus(&layout->screen->id);
    workspace_layout_name_set(&workspace, layout, name);
    // BLI_addtail(&workspace.layouts, layout);

    return layout;
}

// implement in lib .cc
ID* VKE_id_copy_ex(Main*     bmain,
                   const ID* id,
                   ID**      new_id_p,
                   const int flag)
{
    return VKE_id_copy_in_lib(bmain, std::nullopt, id, std::nullopt, new_id_p, flag);
}

WorkSpaceLayout* VKE_workspace_layout_add_from_layout(Main*                  bmain,
                                                      WorkSpace&             workspace_dst,
                                                      const WorkSpaceLayout& layout_src,
                                                      const int              id_copy_flags)
{
    bScreen*    screen_src = VKE_workspace_layout_screen_get(&layout_src);
    const char* name       = VKE_workspace_layout_name_get(&layout_src);

    /* In case the current layout's screen is a 'full screen' one, find the 'full' area, and its
     * 'restore screen' as source, instead of the temporary full-screen one. */
    if (VKE_screen_is_fullscreen_area(screen_src))
    {
        for (ScrArea& area_old : screen_src->areabase)
        {
            /* The original layout/screen will also have one area->full set, but it will point to the
             * same source screen. This can be ignored. */
            if (area_old.full && area_old.full != screen_src)
            {
                screen_src = area_old.full;
                break;
            }
        }
    }

    bScreen* screen_dst = id_cast<bScreen*>(VKE_id_copy_ex(bmain, &screen_src->id, nullptr, id_copy_flags));

    return VKE_workspace_layout_add(bmain, workspace_dst, *screen_dst, name);
}

WorkSpaceLayout* ED_workspace_layout_duplicate(Main*                  bmain,
                                               WorkSpace*             workspace,
                                               const WorkSpaceLayout* layout_old,
                                               wmWindow* /*win*/)
{
    return VKE_workspace_layout_add_from_layout(bmain, *workspace, *layout_old, LIB_ID_COPY_DEFAULT);
}

void wm_file_read_setup_wm_keep_old(vkContext*                C,
                                    Main*                     bmain,
                                    BlendFileReadWMSetupData* wm_setup_data,
                                    wmWindowManager*          wm,
                                    const bool                load_ui)
{
    /* This data is not needed here, besides detecting that old WM has been kept (in caller code).
     * Since `old_wm` is kept, do not free it, just clear the pointer as clean-up. */
    wm_setup_data->old_wm = nullptr;

    if (!load_ui)
    {
        /* When loading without UI (i.e. keeping existing UI), no matching needed.
         *
         * The other UI data (workspaces, layouts, screens) has also been re-used from old Main, and
         * newly read one from file has already been discarded in #setup_app_data. */
        return;
    }

    /* Old WM is being reused, but other UI data (workspaces, layouts, screens) comes from the new
     * file, so the WM needs to be updated to use these. */
    bScreen* screen = CTX_wm_screen(C);

    if (screen != nullptr)
    {
        for (wmWindow& win : wm->windows)
        {
            WorkSpace*       workspace;

            WorkSpaceLayout* layout_ref = VKE_workspace_layout_find_global(bmain, screen, &workspace);
            VKE_workspace_active_set(win.workspace_hook, workspace);
            win.scene = CTX_data_scene(C);

            /* All windows get active screen from file. */
            if (screen->winid == 0)
            {
                WM_window_set_active_screen(&win, workspace, screen);
            }
            else
            {
                WorkSpaceLayout* layout_new = ED_workspace_layout_duplicate(bmain, workspace, layout_ref, &win);

                // WM_window_set_active_layout(&win, workspace, layout_new);
            }

            bScreen* win_screen = WM_window_get_active_screen(&win);
            win_screen->winid   = win.winid;
        }
    }
};

void wm_file_read_setup_wm_finalize(vkContext*                C,
                                    Main*                     bmain,
                                    BlendFileReadWMSetupData* wm_setup_data)
{
    // BLI_assert(BLI_listbase_count_at_most(&bmain->wm, 2) <= 1);
    // BLI_assert(wm_setup_data != nullptr);
    wmWindowManager* wm = static_cast<wmWindowManager*>(bmain->wm.first);

    /* If reading factory startup file, and there was no previous WM, clear the size of the windows
     * in newly read WM so that they get resized to occupy the whole available space on current
     * monitor.
     */
    if (wm_setup_data->is_read_homefile && wm_setup_data->is_factory_startup && wm_setup_data->old_wm == nullptr)
    {
        wm_clear_default_size(C);
    }

    if (wm == nullptr)
    {
        /* Add a default WM in case none exists in newly read main (should only happen when opening
         * an old pre-2.5 .blend file at startup). */
        wm_add_default(bmain, C);
    }
    else if (wm_setup_data->old_wm != nullptr)
    {
        if (wm_setup_data->old_wm == wm)
        {
            /* Old WM was kept, update it with new workspaces/layouts/screens read from file.
             *
             * Happens when not loading UI, or when the newly read file has no WM (pre-2.5 files). */
            // wm_file_read_setup_wm_keep_old(C, bmain, wm_setup_data, wm, (G.fileflags & G_FILE_NO_UI) == 0);

            wm_file_read_setup_wm_keep_old(C, bmain, wm_setup_data, wm, (G.fileflags & G_FILE_NO_UI) == 0);
        }
        else
        {
            /* Using new WM from read file, try to keep current GHOST windows, transfer keymaps, etc.,
             * from old WM.
             *
             * Also takes care of clearing old WM data (temporarily stored in `wm_setup_data->old_wm`).
             */
            wm_file_read_setup_wm_use_new(C, bmain, wm_setup_data, wm);
        }
    }
    /* Else just using the new WM read from file, nothing to do. */
    // BLI_assert(wm_setup_data->old_wm == nullptr);
    MEM_delete(wm_setup_data);

    /* UI Updates. */
    /* Flag local View3D's to check and exit if they are empty. */
    // for (bScreen& screen : bmain->screens)
    // {
    //     for (ScrArea& area : screen.areabase)
    //     {
    //         for (SpaceLink& sl : area.spacedata)
    //         {
    //             if (sl.spacetype == SPACE_VIEW3D)
    //             {
    //                 View3D* v3d = reinterpret_cast<View3D*>(&sl);
    //                 if (v3d->localvd)
    //                 {
    //                     v3d->localvd->runtime.flag |= V3D_RUNTIME_LOCAL_MAYBE_EMPTY;
    //                 }
    //             }
    //         }
    //     }
    // }
};

void setup_app_userdef(BlendFileData* bfd)
{
    if (bfd->user)
    {
        /* only here free userdef themes... */
        // VKE_vektor_userdef_data_set_and_free(bfd->user);
        bfd->user = nullptr;

        /* Security issue: any blend file could include a #BLO_CODE_USER block.
         *
         * Preferences are loaded from #VEKTOR_STARTUP_FILE and later on load #VEKTOR_USERPREF_FILE,
         * to load the preferences defined in the users home directory.
         *
         * This means we will never accidentally (or maliciously)
         * enable scripts auto-execution by loading a `.blend` file. */
        // U.flag |= USER_SCRIPT_AUTOEXEC_DISAVKE;
    }
};

static void setup_app_data(vkContext*     C,
                           BlendFileData* bfd,
                           const BlendFileReadParams* /*params*/,
                           BlendFileReadWMSetupData* wm_setup_data,
                           BlendFileReadReport* /*reports*/)
{
    Main*      bmain          = G_MAIN;

    // ReuseOldBMainData reuse_data = {nullptr};
    // reuse_data.new_bmain         = bfd->main;
    // reuse_data.old_bmain         = bmain;
    // reuse_data.wm_setup_data     = wm_setup_data;

    // swap_wm_data_for_blendfile(&reuse_data, true);

    // reuse_data.id_map = VKE_main_idmap_create(reuse_data.new_bmain, true, reuse_data.old_bmain,
    // MAIN_IDMAP_TYPE_NAME); swap_old_bmain_data_dependencies_process(&reuse_data, ID_WM);
    // VKE_main_idmap_destroy(reuse_data.id_map);

    ViewLayer* cur_view_layer = bfd->cur_view_layer;
    // Scene*     curscene       = bfd->curscene;
    // if (curscene == nullptr)
    // {
    //     curscene = static_cast<Scene*>(bfd->main->scenes.first);
    // }
    // if (curscene == nullptr)
    // {
    //     // curscene = VKE_scene_add(bfd->main, "Empty");
    // }
    // if (cur_view_layer == nullptr)
    // {
    //     // cur_view_layer = VKE_view_layer_default_view(curscene);
    // }

    // CTX_data_scene_set(C, curscene);

    // // VKE_vektor_globals_main_replace(bfd->main);
    // bmain     = G_MAIN;
    // bfd->main = nullptr;
    // CTX_data_main_set(C, bmain);

    // CTX_wm_manager_set(C, static_cast<wmWindowManager*>(bmain->wm.first));
    // CTX_wm_screen_set(C, bfd->curscreen);
    // CTX_wm_area_set(C, nullptr);
    // CTX_wm_region_set(C, nullptr);
    // CTX_wm_region_popup_set(C, nullptr);

    // VKE_scene_set_background(bmain, curscene);
};

void setup_app_blend_file_data(vkContext*                 C,
                               BlendFileData*             bfd,
                               const BlendFileReadParams* params,
                               BlendFileReadWMSetupData*  wm_setup_data,
                               BlendFileReadReport*       reports)
{
    if ((params->skip_flags & BLO_READ_SKIP_USERDEF) == 0)
    {
        setup_app_userdef(bfd);
    }
    if ((params->skip_flags & BLO_READ_SKIP_DATA) == 0)
    {
        setup_app_data(C, bfd, params, wm_setup_data, reports);
    }
};

void VKE_blendfile_read_setup_readfile(vkContext*                 C,
                                       BlendFileData*             bfd,
                                       const BlendFileReadParams* params,
                                       BlendFileReadWMSetupData*  wm_setup_data,
                                       BlendFileReadReport*       reports,
                                       /* Extra args. */
                                       const bool                 startup_update_defaults,
                                       const char*                startup_app_template)
{
    if (bfd->main->is_read_invalid)
    {
        // VKE_reports_prepend(reports->reports, "File could not be read, critical data corruption detected");
        BLO_blendfiledata_free(bfd);
        return;
    }

    if (startup_update_defaults)
    {
        if ((params->skip_flags & BLO_READ_SKIP_DATA) == 0)
        {
            // BLO_update_defaults_startup_blend(bfd->main, startup_app_template);
        }
    }
    setup_app_blend_file_data(C, bfd, params, wm_setup_data, reports);
    BLO_blendfiledata_free(bfd);
}

BlendFileReadWMSetupData* wm_file_read_setup_wm_init(vkContext* C,
                                                     Main*      bmain,
                                                     const bool is_read_homefile)
{
    // BLI_assert(BLI_listbase_count_at_most(&bmain->wm, 2) <= 1);
    wmWindowManager*          wm            = static_cast<wmWindowManager*>(bmain->wm.first);
    BlendFileReadWMSetupData* wm_setup_data = MEM_new_zeroed<BlendFileReadWMSetupData>(__func__);
    wm_setup_data->is_read_homefile         = is_read_homefile;
    /* This info is not always known yet when this function is called. */
    wm_setup_data->is_factory_startup       = false;

    if (wm == nullptr)
    {
        return wm_setup_data;
    }

    /* First wrap up running stuff.
     *
     * Code copied from `wm_init_exit.cc`. */
    // WM_jobs_kill_all(wm);

    wmWindow* active_win = CTX_wm_window(C);
    for (wmWindow& win : wm->windows)
    {
        CTX_wm_window_set(C, &win); /* Needed by operator close callbacks. */
        WM_event_remove_handlers(C, &win.runtime->handlers);
        // WM_event_remove_handlers(C, &win.runtime->modalhandlers);
        ED_screen_exit(C, &win, WM_window_get_active_screen(&win));
    }
    /* Reset active window. */
    CTX_wm_window_set(C, active_win);

    /* NOTE(@ideasman42): Clear the message bus so it's always cleared on file load.
     * Otherwise it's cleared when "Load UI" is set (see #USER_FILENOUI and #wm_close_and_free).
     * However it's _not_ cleared when the UI is kept. This complicates use from add-ons
     * which can re-register subscribers on file-load. To support this use case,
     * it's best to have predictable behavior - always clear. */
    // if (wm->runtime->message_bus != nullptr)
    // {
    //     WM_msgbus_destroy(wm->runtime->message_bus);
    //     wm->runtime->message_bus = nullptr;
    // }

    /* XXX Hack! We have to clear context popup-region here, because removing all
     * #wmWindow::modalhandlers above frees the active menu (at least, in the 'startup splash' case),
     * causing use-after-free error in later handling of the button callbacks in UI code
     * (see #ui_apply_but_funcs_after()).
     * Tried solving this by always nullptr-ing context's menu when setting wm/win/etc.,
     * but it broke popups refreshing (see #47632),
     * so for now just handling this specific case here. */
    CTX_wm_region_popup_set(C, nullptr);

    // ED_editors_exit(bmain, true);

    /* Asset loading is done by the UI/editors and they keep pointers into it. So make sure to clear
     * it after UI/editors. */
    // ed::asset::list::storage_exit();
    // AS_asset_libraries_exit();

    /* NOTE: `wm_setup_data->old_wm` cannot be set here, as this pointer may be swapped with the
     * newly read one in `setup_app_data` process (See #swap_wm_data_for_blendfile). */

    return wm_setup_data;
}

bool WM_file_read(vkContext*  C,
                  const char* filepath,
                  const bool  use_scripts_autoexec_check,
                  ReportList* reports)
{
    const bool do_history_file_update = (G.background == false) && (CTX_wm_manager(C)->op_undo_depth == 0);

    bool       success                = false;

    const bool use_data               = true;
    const bool use_userdef            = false;

    wm_read_callback_pre_wrapper(C, filepath);

    Main* vkmain = CTX_data_main(C);

    errno        = 0;

    WM_cursor_wait(true);

    const int retval = wm_read_exotic(filepath);

    if (retval == VKE_READ_EXOTIC_OK_VKEND)
    {
        BlendFileReadParams params{};
        params.is_startup = false;
        /* Loading preferences when the user intended to load a regular file is a security
         * risk, because the excluded path list is also loaded. Further it's just confusing
         * if a user loads a file and various preferences change. */
        params.skip_flags = BLO_READ_SKIP_USERDEF;

        BlendFileReadReport bf_reports{};
        bf_reports.reports        = reports;
        bf_reports.duration.whole = VKE_time_now_seconds();
        BlendFileData* bfd        = VKE_blendfile_read(filepath, &params, &bf_reports);
        if (bfd != nullptr)
        {
            wm_file_read_pre(use_data, use_userdef);

            /* Close any user-loaded fonts. */
            // BLF_reset_fonts();

            /* Put WM into a stable state for post-readfile processes (kill jobs, removes event handlers,
             * message bus, and so on). */
            BlendFileReadWMSetupData* wm_setup_data = wm_file_read_setup_wm_init(C, vkmain, false);

            /* This flag is initialized by the operator but overwritten on read.
             * need to re-enable it here else drivers and registered scripts won't work. */
            const int                 G_f_orig      = G.f;

            /* Frees the current main and replaces it with the new one read from file. */
            VKE_blendfile_read_setup_readfile(C, bfd, &params, wm_setup_data, &bf_reports, false, nullptr);
            vkmain = CTX_data_main(C);

            /* Finalize handling of WM, using the read WM and/or the current WM depending on things like
             * whether the UI is loaded from the .blend file or not, etc. */
            wm_file_read_setup_wm_finalize(C, vkmain, wm_setup_data);

            if (G.f != G_f_orig)
            {
                const int flags_keep = G_FLAG_ALL_RUNTIME;
                G.f &= G_FLAG_ALL_READFILE;
                G.f = (G.f & ~flags_keep) | (G_f_orig & flags_keep);
            }

            /* Set by the `use_scripts` property on file load.
             * If this was not set, then it should be calculated based on the file-path.
             * Note that this uses `vkmain->filepath` and not `filepath`, necessary when
             * recovering the last session, where the file-path can be #VEKTOR_QUIT_FILE. */
            if (use_scripts_autoexec_check)
            {
                // WM_file_autoexec_init(vkmain->filepath);
            }

            // WM_check(C); /* Opens window(s), checks keymaps. */

            if (do_history_file_update)
            {
                // wm_history_file_update();
            }

            // wmFileReadPost_Params read_file_post_params{};
            // read_file_post_params.use_data           = use_data;
            // read_file_post_params.use_userdef        = use_userdef;
            // read_file_post_params.is_startup_file    = false;
            // read_file_post_params.is_factory_startup = false;
            // read_file_post_params.reset_app_template = false;
            // read_file_post_params.success            = true;
            // read_file_post_params.is_alloc           = false;
            // wm_file_read_post(C, filepath, &read_file_post_params);

            // bf_reports.duration.whole = VKE_time_now_seconds() - bf_reports.duration.whole;
            // file_read_reports_finalize(&bf_reports);

            success = true;
        }
    }

    else if (retval == VKE_READ_EXOTIC_FAIL_OPEN)
    {
        // VKE_reportf(reports, RPT_ERROR, "Cannot read file \"%s\": %s", filepath,
        //             errno ? strerror(errno) : RPT_("unable to open the file"));
    }
    else if (retval == VKE_READ_EXOTIC_FAIL_FORMAT)
    {
        VKE_reportf(reports, RPT_ERROR, "File format is not supported in file \"%s\"", filepath);
    }
    else if (retval == VKE_READ_EXOTIC_FAIL_PATH)
    {
        VKE_reportf(reports, RPT_ERROR, "File path \"%s\" invalid", filepath);
    }
    else
    {
        VKE_reportf(reports, RPT_ERROR, "Unknown error loading \"%s\"", filepath);
        // BLI_assert_msg(0, "invalid 'retval'");
    }

    WM_cursor_wait(false);

    wm_read_callback_post_wrapper(C, filepath, success);

    // BLI_assert(VKE_main_namemap_validate(*CTX_data_main(C)));

    return success;
};

void wm_homefile_read_ex(vkContext*                   C,
                         const wmHomeFileRead_Params* params_homefile,
                         ReportList*                  reports,
                         wmFileReadPost_Params**      r_params_file_read_post)
{
    Main*        bmain       = G_MAIN;
    bool         success     = false;

    const bool   use_data    = params_homefile->use_data;
    const bool   use_userdef = params_homefile->use_userdef;

    eBLOReadSkip skip_flags  = eBLOReadSkip(0);

    if (use_data == false)
    {
        skip_flags |= BLO_READ_SKIP_DATA;
    }
    if (use_userdef == false)
    {
        skip_flags |= BLO_READ_SKIP_USERDEF;
    }

    if (use_data)
    {
        wm_read_callback_pre_wrapper(C, "");
    }

    wm_file_read_pre(use_data, use_userdef);

    BlendFileReadWMSetupData* wm_setup_data = nullptr;
    if (use_data)
    {
        wm_setup_data = wm_file_read_setup_wm_init(C, bmain, true);
    }

    /* VEKTOR: Always load factory settings for the game engine. */
    BlendFileReadParams read_file_params{};
    read_file_params.is_startup          = true;
    read_file_params.is_factory_settings = true;
    read_file_params.skip_flags          = skip_flags;
    BlendFileData* bfd =
        VKE_blendfile_read_from_memory(datatoc_startup_blend, datatoc_startup_blend_size, &read_file_params, nullptr);

    if (bfd != nullptr)
    {
        BlendFileReadReport read_report{};
        VKE_blendfile_read_setup_readfile(C, bfd, &read_file_params, wm_setup_data, &read_report, true, nullptr);
        success = true;
        bmain   = CTX_data_main(C);
    }

    if (params_homefile->use_empty_data)
    {
        VKE_blendfile_read_make_empty(C);
    }

    if (use_userdef)
    {
        UserDef* userdef_default = VKE_blendfile_userdef_from_defaults();
        VKE_vektor_userdef_data_set_and_free(userdef_default);
    }

    if (use_userdef)
    {
        wm_init_userdef(bmain);
    }

    if (use_data)
    {
        wm_setup_data->is_factory_startup = true;
        wm_file_read_setup_wm_finalize(C, bmain, wm_setup_data);
    }

    if (use_userdef)
    {
        for (wmWindowManager& wm : bmain->wm)
        {
            if (wm.runtime->defaultconf)
            {
                wm.runtime->defaultconf->flag &= ~KEYCONF_INIT_DEFAULT;
            }
        }
    }

    if (use_data)
    {
        WM_check(C);
        bmain->filepath[0] = '\0';
    }

    {
        wmFileReadPost_Params params_file_read_post{};
        params_file_read_post.use_data           = use_data;
        params_file_read_post.use_userdef        = use_userdef;
        params_file_read_post.is_startup_file    = true;
        params_file_read_post.is_factory_startup = true;
        params_file_read_post.is_first_time      = params_homefile->is_first_time;
        params_file_read_post.reset_app_template = false;
        params_file_read_post.success            = success;
        params_file_read_post.is_alloc           = false;

        if (r_params_file_read_post == nullptr)
        {
            wm_homefile_read_post(C, &params_file_read_post);
        }
        else
        {
            params_file_read_post.is_alloc = true;
            *r_params_file_read_post       = MEM_new_uninitialized<wmFileReadPost_Params>(__func__);
            **r_params_file_read_post      = params_file_read_post;
            CTX_wm_window_set(C, nullptr);
        }
    }
}

void wm_homefile_read(vkContext*                   C,
                      const wmHomeFileRead_Params* params_homefile,
                      ReportList*                  reports)
{
    wm_homefile_read_ex(C, params_homefile, reports, nullptr);
}

void wm_homefile_read_post(vkContext*             C,
                           wmFileReadPost_Params* params)
{
    // Stub
}

void wm_init_userdef(Main* bmain)
{
    // Stub
}

void WM_check(vkContext* C) {}

struct UserDef;
struct wmOperator;
struct ARegion;
struct StructRNA;

#include "../rna/RNA_types.h"

bool screen_temp_region_exists(const ARegion* region)
{
    return false;
}
PointerRNA RNA_pointer_create_discrete(ID*        id,
                                       StructRNA* type,
                                       void*      data)
{
    return PointerRNA();
}
void     VKE_blendfile_read_make_empty(vkContext* C) {}
ScrArea* ED_fileselect_handler_area_find(wmWindow*   win,
                                         wmOperator* op)
{
    return nullptr;
}
void     wm_operator_free_for_fileselect(wmOperator* op) {}
UserDef* VKE_blendfile_userdef_from_defaults()
{
    return nullptr;
}

#include "../translation/VKT_translation.hh"

float WM_window_dpi_get_scale(const wmWindow* win)
{
    return 1.0f;
}
void VKE_report(ReportList* reports,
                eReportType type,
                const char* format,
                ...)
{
}
void             VKE_reports_clear(ReportList* reports) {}
vklib::StringRef VKT_translate_do_iface(vklib::StringRef msgctx,
                                        vklib::StringRef msgid)
{
    return msgid;
}
void WM_cursor_grab_disable(wmWindow* win,
                            void*     customdata)
{
}
FileReader* BLI_filereader_new_file(int filedes)
{
    return nullptr;
}
FileReader* BLI_filereader_new_gzip(FileReader* reader)
{
    return nullptr;
}
FileReader* BLI_filereader_new_zstd(FileReader* reader)
{
    return nullptr;
}
void VKE_libblock_free_data_py(ID* id) {}

} // namespace vektor