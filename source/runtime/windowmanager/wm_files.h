#pragma once

#include <cstdint>
#include "../kernel/intern/VK_report.hh"
#include "../vklib/VKE_context.h"
#include "../vklib/VKE_listlist.h"

namespace vektor
{

#define FILE_MAX 1024

struct wmHomeFileRead_Params
{
    /** Load data, disable when only loading user preferences. */
    unsigned int use_data : 1;
    /** Load factory settings as well as startup file (disabled for "File New"). */
    unsigned int use_userdef : 1;

    /**
     * Ignore on-disk startup file, use bundled `datatoc_startup_blend` instead.
     * Used for "Restore Factory Settings".
     */
    unsigned int use_factory_settings : 1;
    /** Read factory settings from the app-templates only (keep other defaults). */
    unsigned int use_factory_settings_app_template_only : 1;
    /**
     * Load the startup file without any data-blocks.
     * Useful for automated content generation, so the file starts without data.
     */
    unsigned int use_empty_data : 1;
    /**
     * When true, this is the first time the home file is read.
     * In this case resetting the previous state can be skipped.
     */
    unsigned int is_first_time : 1;
    /**
     * Optional path pointing to an alternative blend file (may be NULL).
     */
    const char*  filepath_startup_override;
    /**
     * Template to use instead of the template defined in user-preferences.
     * When not-null, this is written into the user preferences.
     */
    const char*  app_template_override;
};

struct wmFileReadPost_Params
{
    uint32_t use_data : 1;
    uint32_t use_userdef : 1;

    uint32_t is_startup_file : 1;
    uint32_t is_factory_startup : 1;
    uint32_t reset_app_template : 1;

    /* Used by #wm_homefile_read_post. */
    uint32_t success : 1;
    uint32_t is_alloc : 1;
    uint32_t is_first_time : 1;
};

struct BlendFileReadReport
{
    /** General reports handling. */
    ReportList* reports;

    /** Timing information. */
    struct
    {
        double whole;
        double libraries;
        double lib_overrides;
        double lib_overrides_resync;
        double lib_overrides_recursive_resync;
    } duration;

    /** Count information. */
    struct
    {
        /**
         * Some numbers of IDs that ended up in a specific state, or required some specific process
         * during this file read.
         */
        int missing_libraries;
        int missing_linked_id;
        /** Some sub-categories of the above `missing_linked_id` counter. */
        int missing_obdata;
        int missing_obproxies;

        /** Number of root override IDs that were resynced. */
        int resynced_lib_overrides;

        /** Number of proxies converted to library overrides. */
        int proxies_to_lib_overrides_success;
        /** Number of proxies that failed to convert to library overrides. */
        int proxies_to_lib_overrides_failures;
        /** Number of sequencer strips that were not read because were in non-supported channels. */
        int sequence_strips_skipped;
    } count;

    /**
     * Number of libraries which had overrides that needed to be resynced,
     * and a single linked list of those.
     */
    int       resynced_lib_overrides_libraries_count;

    bool      do_resynced_lib_overrides_libraries_list;

    LinkNode* resynced_lib_overrides_libraries;
};

void wm_homefile_read_ex(vkContext*                   C,
                         const wmHomeFileRead_Params* params_homefile,
                         ReportList*                  reports,
                         wmFileReadPost_Params**      r_params_file_read_post);

void wm_homefile_read_post(vkContext*             C,
                           wmFileReadPost_Params* params);

void wm_init_userdef(Main* bmain);

struct BlendFileReadWMSetupData;
void                      wm_file_read_setup_wm_finalize(vkContext*                C,
                                                         Main*                     bmain,
                                                         BlendFileReadWMSetupData* wm_setup_data);

BlendFileReadWMSetupData* wm_file_read_setup_wm_init(vkContext* C,
                                                     Main*      bmain,
                                                     bool       use_data);

struct BlendFileReadParams;
struct BlendFileData;
void VKE_blendfile_read_setup_readfile(vkContext*                 C,
                                       BlendFileData*             bfd,
                                       const BlendFileReadParams* params,
                                       BlendFileReadWMSetupData*  wm_setup_data,
                                       BlendFileReadReport*       read_report,
                                       bool                       is_startup,
                                       const char*                filepath);

} // namespace vektor