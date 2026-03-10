/* SPDX-FileCopyrightText: 2026 Vektor Engine. All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup loader
 * Implementation of the Vektor file loader.
 */

#include "../../intern/clog/CLG_log.h"
#include "../dna/DNA_genfile.h"
#include "VLO_readfile.hh"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

namespace vektor::loader {

CLG_LOGREF_DECLARE_GLOBAL(V_LOG, "loader");

#define VLO_CODE_ENDB 'EDNE'  // ENDB
#define VLO_CODE_DATA 'ATAD'  // DATA
#define VLO_CODE_GLOB 'BOLG'  // GLOB
#define VLO_CODE_DNA1 '1AND'  // DNA1

struct BHead {
  int32_t code;
  int32_t len;
  void *old;
  int32_t SDNAnr;
  int32_t nr;
};

struct FileData {
  FILE *file;
  dna::SDNA *filesdna;
  char header[12];
  bool endian_swapped;
};

static bool read_header(FileData *fd)
{
  if (fread(fd->header, 1, 12, fd->file) != 12) {
    return false;
  }

  if (strncmp(fd->header, "VEKTOR", 6) != 0) {
    CLOG_ERROR(V_LOG, "Not a Vektor file (header: %.12s)", fd->header);
    return false;
  }

  // Example: VEKTOR_v100_L (L for Little Endian)
  fd->endian_swapped = (fd->header[11] == 'B');

  return true;
}

static BHead *read_bhead(FileData *fd)
{
  BHead *bh = (BHead *)malloc(sizeof(BHead));
  if (fread(bh, sizeof(BHead), 1, fd->file) != 1) {
    free(bh);
    return nullptr;
  }
  return bh;
}

VktHandle *VLO_handle_from_file(const char *filepath)
{
  FILE *f = fopen(filepath, "rb");
  if (!f) {
    CLOG_ERROR(V_LOG, "Could not open file %s", filepath);
    return nullptr;
  }

  FileData *fd = new FileData();
  fd->file = f;
  fd->filesdna = nullptr;

  if (!read_header(fd)) {
    fclose(f);
    delete fd;
    return nullptr;
  }

  VktHandle *handle = new VktHandle();
  handle->data = fd;
  return handle;
}

void VLO_handle_close(VktHandle *handle)
{
  if (!handle)
    return;
  auto *fd = (FileData *)handle->data;
  if (fd->file) {
    fclose(fd->file);
  }
  if (fd->filesdna) {
    dna::DNA_sdna_free(fd->filesdna);
  }
  delete fd;
  delete handle;
}

VktFileData *VLO_read_from_file(const char *filepath)
{
  VktHandle *handle = VLO_handle_from_file(filepath);
  if (!handle)
    return nullptr;

  FileData *fd = (FileData *)handle->data;
  CLOG_INFO(V_LOG, "Reading Vektor file: %s", filepath);

  // Simple traversal
  BHead *bh;
  while ((bh = read_bhead(fd)) != nullptr) {
    if (bh->code == VLO_CODE_DNA1) {
      void *dna_data = malloc(bh->len);
      fread(dna_data, bh->len, 1, fd->file);
      fd->filesdna = dna::DNA_sdna_from_data(dna_data, bh->len, fd->endian_swapped);
      free(dna_data);
      CLOG_INFO(V_LOG, "Loaded DNA with %d structs", (int)fd->filesdna->structs.size());
    }
    else if (bh->code == VLO_CODE_ENDB) {
      free(bh);
      break;
    }
    else {
      // Skip data for now
      fseek(fd->file, bh->len, SEEK_CUR);
    }
    free(bh);
  }

  auto *vfd = new VktFileData();
  vfd->bmain = nullptr;  // Placeholder for Main database
  vfd->user = nullptr;

  VLO_handle_close(handle);
  return vfd;
}

void VLO_filedata_free(VktFileData *vfd)
{
  if (!vfd)
    return;
  // Free Main and other resources...
  delete vfd;
}

}  // namespace vektor::loader
