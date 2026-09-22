/**
 * @file ciot_storage_mock.h
 * @brief In-memory fake of ciot_storage_t for host unit tests.
 *
 * struct ciot_storage is a plain, publicly-visible vtable (no opaque
 * allocation hidden in a platform backend), so this fake just fills its 3
 * function pointers with an in-memory {path, bytes} store instead of
 * touching a real filesystem/NVS/etc. Intended to be reused by any project
 * (ciot_c's own tests included) that needs a deterministic, disk-free
 * ciot_storage_t in a unit test.
 */

#ifndef __CIOT_STORAGE_MOCK__H__
#define __CIOT_STORAGE_MOCK__H__

#include "ciot_storage.h"

#ifndef CIOT_STORAGE_MOCK_MAX_FILES
#define CIOT_STORAGE_MOCK_MAX_FILES 16
#endif

#ifndef CIOT_STORAGE_MOCK_MAX_FILE_SIZE
#define CIOT_STORAGE_MOCK_MAX_FILE_SIZE 16384
#endif

#ifndef CIOT_STORAGE_MOCK_MAX_PATH
#define CIOT_STORAGE_MOCK_MAX_PATH 64
#endif

/** Allocates a fresh, empty in-memory storage fake. */
ciot_storage_t ciot_storage_mock_new(void);

/** Clears every stored file, without freeing the fake itself - call from a test's setUp/tearDown. */
void ciot_storage_mock_reset(ciot_storage_t self);

/** Frees a fake created with ciot_storage_mock_new(). */
void ciot_storage_mock_free(ciot_storage_t self);

/** Test-only helper: true if `path` currently has stored bytes. */
bool ciot_storage_mock_has_file(ciot_storage_t self, const char *path);

#endif  //!__CIOT_STORAGE_MOCK__H__
