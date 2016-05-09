/*
 * Copyright (c) 2016, Linaro Limited
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License Version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#ifndef _LIBSMAF_H_
#define _LIBSMAF_H_

/* For O_CLOEXEC and O_RDWR flags definition */
#include <fcntl.h>

/* For close() */
#include <unistd.h>

#if defined(__cplusplus)
extern "C" {
#endif

int smaf_open();
void smaf_close();

int smaf_create_buffer(unsigned int length, unsigned int flags, char *name, int *fd);
int smaf_set_secure(int fd, int secure);
int smaf_get_secure(int fd);

#if defined(__cplusplus)
}
#endif

#endif
