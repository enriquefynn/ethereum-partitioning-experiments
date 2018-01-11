/*
 * Copyright (c) 2016, University of Lugano
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the names of it
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <cstdio>
#include <cstring>

#define UNUSED __attribute__((unused))

#ifndef NO_COLOR
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"
#else
#define ANSI_COLOR_RED
#define ANSI_COLOR_GREEN
#define ANSI_COLOR_YELLOW
#define ANSI_COLOR_BLUE
#define ANSI_COLOR_MAGENTA
#define ANSI_COLOR_CYAN
#define ANSI_COLOR_RESET
#endif

#define __BASEFILE__                                                           \
  (strrchr(__FILE__, '/')                                                      \
       ? strrchr(__FILE__, '/') + 1                                            \
       : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#if defined(DEBUG) && DEBUG > 0
#define LOG_DEBUG(fmt, args...)                                                \
  fprintf(stdout, ANSI_COLOR_RESET "DEBUG: %s:%d:%s(): " fmt "\n",             \
          __BASEFILE__, __LINE__, __func__, ##args)
#else
#define LOG_DEBUG(fmt, args...) /* Don't do anything in release builds */
#endif

#define LOG_INFO(fmt, args...)                                                 \
  fprintf(stderr,                                                              \
          ANSI_COLOR_GREEN "INFO:  %s:%d:%s(): " fmt "\n" ANSI_COLOR_RESET,    \
          __BASEFILE__, __LINE__, __func__, ##args)

#define LOG_ERROR(fmt, args...)                                                \
  fprintf(stderr,                                                              \
          ANSI_COLOR_RED "ERROR: %s:%d:%s(): " fmt "\n" ANSI_COLOR_RESET,      \
          __BASEFILE__, __LINE__, __func__, ##args)