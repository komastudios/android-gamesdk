/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

namespace androidgamesdk_deviceinfo {

namespace stream_util {

#if __ANDROID_API__ < 18

// getdelim adapted from NetBSD getdelim.c:

/*-
 * Copyright (c) 2011 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Christos Zoulas.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

ssize_t getdelim(char **buf, size_t *bufsiz, int delimiter, FILE *fp) {
  char *ptr, *eptr;

  if (*buf == NULL || *bufsiz == 0) {
    *bufsiz = BUFSIZ;
    if ((*buf = static_cast<char *>(malloc(*bufsiz))) == NULL) return -1;
  }

  for (ptr = *buf, eptr = *buf + *bufsiz;;) {
    int c = fgetc(fp);
    if (c == -1) {
      if (feof(fp)) {
        ssize_t diff = (ssize_t)(ptr - *buf);
        if (diff != 0) {
          *ptr = '\0';
          return diff;
        }
      }
      return -1;
    }
    *ptr++ = c;
    if (c == delimiter) {
      *ptr = '\0';
      return ptr - *buf;
    }
    if (ptr + 2 >= eptr) {
      char *nbuf;
      size_t nbufsiz = *bufsiz * 2;
      ssize_t d = ptr - *buf;
      if ((nbuf = static_cast<char *>(realloc(*buf, nbufsiz))) == NULL)
        return -1;
      *buf = nbuf;
      *bufsiz = nbufsiz;
      eptr = nbuf + nbufsiz;
      ptr = nbuf + d;
    }
  }
}

ssize_t getline(char **buf, size_t *bufsiz, FILE *fp) {
  return stream_util::getdelim(buf, bufsiz, '\n', fp);
}

#endif

}  // namespace stream_util
}  // namespace androidgamesdk_deviceinfo
