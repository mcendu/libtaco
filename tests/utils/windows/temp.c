// SPDX-License-Identifier: BSD-2-Clause
//
// The C standard tmpfile is broken on Windows NT (along with
// the "recommended" counterpart, tmpfile_s), so alternatives
// like this implementation of "open a temporary file" have to
// be used instead.
#include <Windows.h>

#include <fcntl.h>
#include <fileapi.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

FILE *open_temp(void) {
  // get path for temp files
  size_t tmpdir_len = GetTempPathA(0, NULL);
  char *tmpdir = malloc(tmpdir_len + 1);
  tmpdir[tmpdir_len] = 0;
  GetTempPath2A(tmpdir_len, tmpdir);

  // generate a name for the temporary file
  char *tmpname = malloc(MAX_PATH + 1);
  tmpname[MAX_PATH] = 0;
  GetTempFileNameA(tmpdir, "tmp", 0, tmpname);
  free(tmpdir);

  // create and open the temporary file
  intptr_t handle = (intptr_t)CreateFileA(
      tmpname, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
      FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
  free(tmpname);

  // convert to FILE *
  return _fdopen(_open_osfhandle(handle, _O_RDWR | _O_BINARY), "w+b");
}
