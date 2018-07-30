/*
 * -------------------------------------------------------------
 *
 * Module: sem_post_multiple.c
 *
 * Purpose:
 *	Semaphores aren't actually part of the PThreads standard.
 *	They are defined by the POSIX Standard:
 *
 *		POSIX 1003.1b-1993	(POSIX.1b)
 *
 * -------------------------------------------------------------
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
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "pthread.h"
#include "semaphore.h"
#include "implement.h"


int
sem_post_multiple (sem_t * sem, int count)
/*
 * ------------------------------------------------------
 * DOCPUBLIC
 *      This function posts multiple wakeups to a semaphore.
 *
 * PARAMETERS
 *      sem
 *              pointer to an instance of sem_t
 *
 *      count
 *              counter, must be greater than zero.
 *
 * DESCRIPTION
 *      This function posts multiple wakeups to a semaphore. If there
 *      are waiting threads (or processes), n <= count are awakened;
 *      the semaphore value is incremented by count - n.
 *
 * RESULTS
 *              0               successfully posted semaphore,
 *              -1              failed, error in errno
 * ERRNO
 *              EINVAL          'sem' is not a valid semaphore
 *                              or count is less than or equal to zero.
 *              ERANGE          semaphore count is too big
 *
 * ------------------------------------------------------
 */
{
  ptw32_mcs_local_node_t node;
  int result = 0;
  long waiters;
  sem_t s = *sem;

  ptw32_mcs_lock_acquire(&s->lock, &node);

  if (s->value <= (SEM_VALUE_MAX - count))
    {
      waiters = -s->value;
      s->value += count;
      if (waiters > 0)
        {
#if defined(NEED_SEM)
          if (SetEvent(s->sem))
            {
              waiters--;
              s->leftToUnblock += count - 1;
              if (s->leftToUnblock > waiters)
                {
                  s->leftToUnblock = waiters;
                }
            }
#else
  if (ReleaseSemaphore (s->sem,  (waiters<=count)?waiters:count, 0))
    {
      /* No action */
    }
#endif
  else
    {
      s->value -= count;
      result = EINVAL;
    }
        }
    }
  else
    {
      result = ERANGE;
    }
  ptw32_mcs_lock_release(&node);

  if (result != 0)
    {
      PTW32_SET_ERRNO(result);
      return -1;
    }

  return 0;
}
