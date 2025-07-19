// SPDX-License-Identifier: BSD-2-Clause
#define _XOPEN_SOURCE 600
#ifdef __linux__
#define _GNU_SOURCE
#endif

#include <fcntl.h>
#include <stdio.h>

FILE *open_temp(void) {
#ifdef __linux__
  // explicitly use linux-specific facilities (workaround for musl)
  int fd = open("/tmp", O_RDWR | O_TMPFILE | O_EXCL, S_IRUSR | S_IWUSR);
  if (fd < 0) {
    // fallback to standard library on failure
    return tmpfile();
  }

  return fdopen(fd, "w+b");
#else
  // just forward to the standard library
  return tmpfile();
#endif
}
