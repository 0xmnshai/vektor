/* SPDX-FileCopyrightText: 2026 Vektor Engine. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup loader
 * `.vkt` file reading entry point.
 */

#pragma once



namespace vektor::loader {

struct VktFileData;
typedef struct VktHandle VktHandle;

/**
 * Handle to a loaded Vektor file.
 * Wraps the internal FileData structure.
 */
struct VktHandle {
  void *data;
};

/**
 * Result of reading a file's full content.
 */
struct VktFileData {
  struct Main *bmain;
  struct UserDef *user;
};

/**
 * Load a VktHandle from a file path.
 * This only reads the header and DNA, not the full content.
 */
VktHandle *VLO_handle_from_file(const char *filepath);

/**
 * Read the full content of a file into a VktFileData structure.
 */
VktFileData *VLO_read_from_file(const char *filepath);

/**
 * Close and free a VktHandle.
 */
void VLO_handle_close(VktHandle *handle);

/**
 * Free VktFileData.
 */
void VLO_filedata_free(VktFileData *vfd);

}  // namespace vektor::loader
