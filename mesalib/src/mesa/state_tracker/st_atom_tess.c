/**************************************************************************
 * 
 * Copyright 2015 Advanced Micro Devices, Inc.
 * All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 **************************************************************************/

/*
 * Authors:
 *   Marek Olšák <maraeo@gmail.com>
 */


#include "main/macros.h"
#include "st_context.h"
#include "pipe/p_context.h"
#include "st_atom.h"


static void
update_tess(struct st_context *st)
{
   const struct gl_context *ctx = st->ctx;
   struct pipe_context *pipe = st->pipe;

   if (!pipe->set_tess_state)
      return;

   pipe->set_tess_state(pipe,
                        ctx->TessCtrlProgram.patch_default_outer_level,
                        ctx->TessCtrlProgram.patch_default_inner_level);
}


const struct st_tracked_state st_update_tess = {
   "update_tess",		/* name */
   {				/* dirty */
      0,			/* mesa */
      ST_NEW_TESS_STATE,	/* st */
   },
   update_tess                  /* update */
};
