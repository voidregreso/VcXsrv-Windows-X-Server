/*
 * File: valid1.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999-2017, Pthreads-win32 contributors
 *
 *      Homepage: https://sourceforge.net/projects/pthreads4w/
 *
 *      The current list of contributors is contained
 *      in the file CONTRIBUTORS included with the source
 *      code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      https://sourceforge.net/p/pthreads4w/wiki/Contributors/
 *
 * This file is part of Pthreads-win32.
 *
 *    Pthreads-win32 is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Pthreads-win32 is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Pthreads-win32.  If not, see <http://www.gnu.org/licenses/>. *
 *
 * --------------------------------------------------------------------------
 *
 * Test Synopsis:
 * - Test that thread validation works.
 *
 * Test Method (Validation or Falsification):
 * -
 *
 * Requirements Tested:
 * -
 *
 * Features Tested:
 * -
 *
 * Cases Tested:
 * -
 *
 * Description:
 * -
 *
 * Environment:
 * -
 *
 * Input:
 * - None.
 *
 * Output:
 * - File name, Line number, and failed expression on failure.
 * - No output on success.
 *
 * Assumptions:
 * -
 *
 * Pass Criteria:
 * - Process returns zero exit status.
 *
 * Fail Criteria:
 * - Process returns non-zero exit status.
 */

#include "test.h"

enum {
	NUMTHREADS = 1
};

static int washere = 0;

void * func(void * arg)
{
  washere = 1;
  return (void *) 0; 
}
 
int
main()
{
  pthread_t t;
  void * result = NULL;

  washere = 0;
  assert(pthread_create(&t, NULL, func, NULL) == 0);
  assert(pthread_join(t, &result) == 0);
  assert((int)(size_t)result == 0);
  assert(washere == 1);
  sched_yield();
  assert(pthread_kill(t, 0) == ESRCH);

  return 0;
}
