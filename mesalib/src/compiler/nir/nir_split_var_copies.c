/*
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Jason Ekstrand (jason@jlekstrand.net)
 *
 */

#include "nir.h"
#include "nir_builder.h"

/*
 * Implements "copy splitting" which is similar to structure splitting only
 * it works on copy operations rather than the datatypes themselves.  The
 * GLSL language allows you to copy one variable to another an entire
 * structure (which may contain arrays or other structures) at a time.
 * Normally, in a language such as C this would be handled by a "structure
 * splitting" pass that breaks up the structures.  Unfortunately for us,
 * structures used in inputs or outputs can't be split.  Therefore,
 * regardlesss of what we do, we have to be able to copy to/from
 * structures.
 *
 * The primary purpose of structure splitting is to allow you to better
 * optimize variable access and lower things to registers where you can.
 * The primary issue here is that, if you lower the copy to a bunch of
 * loads and stores, you loose a lot of information about the copy
 * operation that you would like to keep around.  To solve this problem, we
 * have a "copy splitting" pass that, instead of splitting the structures
 * or lowering the copy into loads and storres, splits the copy operation
 * into a bunch of copy operations one for each leaf of the structure tree.
 * If an intermediate array is encountered, it is referenced with a
 * wildcard reference to indicate that the entire array is to be copied.
 *
 * As things become direct, array copies may be able to be losslessly
 * lowered to having fewer and fewer wildcards.  However, until that
 * happens we want to keep the information about the arrays intact.
 *
 * Prior to the copy splitting pass, there are no wildcard references but
 * there may be incomplete references where the tail of the deref chain is
 * an array or a structure and not a specific element.  After the copy
 * splitting pass has completed, every variable deref will be a full-length
 * dereference pointing to a single leaf in the structure type tree with
 * possibly a few wildcard array dereferences.
 */

static void
split_deref_copy_instr(nir_builder *b,
                       nir_deref_instr *dst, nir_deref_instr *src)
{
   assert(dst->type == src->type);
   if (glsl_type_is_vector_or_scalar(src->type)) {
      nir_copy_deref(b, dst, src);
   } else if (glsl_type_is_struct(src->type)) {
      for (unsigned i = 0; i < glsl_get_length(src->type); i++) {
         split_deref_copy_instr(b, nir_build_deref_struct(b, dst, i),
                                   nir_build_deref_struct(b, src, i));
      }
   } else {
      assert(glsl_type_is_matrix(src->type) || glsl_type_is_array(src->type));
      split_deref_copy_instr(b, nir_build_deref_array_wildcard(b, dst),
                                nir_build_deref_array_wildcard(b, src));
   }
}

static bool
split_var_copies_impl(nir_function_impl *impl)
{
   bool progress = false;

   nir_builder b;
   nir_builder_init(&b, impl);

   nir_foreach_block(block, impl) {
      nir_foreach_instr_safe(instr, block) {
         if (instr->type != nir_instr_type_intrinsic)
            continue;

         nir_intrinsic_instr *copy = nir_instr_as_intrinsic(instr);
         if (copy->intrinsic != nir_intrinsic_copy_deref)
            continue;

         b.cursor = nir_instr_remove(&copy->instr);

         nir_deref_instr *dst =
            nir_instr_as_deref(copy->src[0].ssa->parent_instr);
         nir_deref_instr *src =
            nir_instr_as_deref(copy->src[1].ssa->parent_instr);
         split_deref_copy_instr(&b, dst, src);

         progress = true;
      }
   }

   if (progress) {
      nir_metadata_preserve(impl, nir_metadata_block_index |
                                  nir_metadata_dominance);
   } else {
#ifndef NDEBUG
      impl->valid_metadata &= ~nir_metadata_not_properly_reset;
#endif
   }

   return progress;
}

bool
nir_split_var_copies(nir_shader *shader)
{
   bool progress = false;

   nir_foreach_function(function, shader) {
      if (function->impl)
         progress = split_var_copies_impl(function->impl) || progress;
   }

   return progress;
}
