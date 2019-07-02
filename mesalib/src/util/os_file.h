/*
 * Copyright 2019 Intel Corporation
 * SPDX-License-Identifier: MIT
 *
 * File operations helpers
 */

#ifndef _OS_FILE_H_
#define _OS_FILE_H_

#include <stdio.h>

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * Create a new file and opens it for writing-only.
 * If the given filename already exists, nothing is done and NULL is returned.
 * `errno` gets set to the failure reason; if that is not EEXIST, the caller
 * might want to do something other than trying again.
 */
FILE *
os_file_create_unique(const char *filename, int filemode);

/*
 * Read a file.
 * Returns a char* that the caller must free(), or NULL and sets errno.
 */
char *
os_read_file(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* _OS_FILE_H_ */
