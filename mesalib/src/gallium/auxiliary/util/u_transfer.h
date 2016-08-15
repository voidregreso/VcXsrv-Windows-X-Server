
#ifndef U_TRANSFER_H
#define U_TRANSFER_H

/* Fallback implementations for inline read/writes which just go back
 * to the regular transfer behaviour.
 */
#include "pipe/p_state.h"

struct pipe_context;
struct winsys_handle;

boolean u_default_resource_get_handle(struct pipe_screen *screen,
                                      struct pipe_resource *resource,
                                      struct winsys_handle *handle);

void u_default_buffer_subdata(struct pipe_context *pipe,
                              struct pipe_resource *resource,
                              unsigned usage, unsigned offset,
                              unsigned size, const void *data);

void u_default_texture_subdata(struct pipe_context *pipe,
                               struct pipe_resource *resource,
                               unsigned level,
                               unsigned usage,
                               const struct pipe_box *box,
                               const void *data,
                               unsigned stride,
                               unsigned layer_stride);

void u_default_transfer_flush_region( struct pipe_context *pipe,
                                      struct pipe_transfer *transfer,
                                      const struct pipe_box *box);

void u_default_transfer_unmap( struct pipe_context *pipe,
                               struct pipe_transfer *transfer );



/* Useful helper to allow >1 implementation of resource functionality
 * to exist in a single driver.  This is intended to be transitionary!
 */
struct u_resource_vtbl {

   boolean (*resource_get_handle)(struct pipe_screen *,
                                  struct pipe_resource *tex,
                                  struct winsys_handle *handle);

   void (*resource_destroy)(struct pipe_screen *,
                            struct pipe_resource *pt);

   void *(*transfer_map)(struct pipe_context *,
                         struct pipe_resource *resource,
                         unsigned level,
                         unsigned usage,
                         const struct pipe_box *,
                         struct pipe_transfer **);


   void (*transfer_flush_region)( struct pipe_context *,
                                  struct pipe_transfer *transfer,
                                  const struct pipe_box *);

   void (*transfer_unmap)( struct pipe_context *,
                           struct pipe_transfer *transfer );
};


struct u_resource {
   struct pipe_resource b;
   const struct u_resource_vtbl *vtbl;
};


boolean u_resource_get_handle_vtbl(struct pipe_screen *screen,
                                   struct pipe_resource *resource,
                                   struct winsys_handle *handle,
                                   unsigned usage);

void u_resource_destroy_vtbl(struct pipe_screen *screen,
                             struct pipe_resource *resource);

void *u_transfer_map_vtbl(struct pipe_context *context,
                          struct pipe_resource *resource,
                          unsigned level,
                          unsigned usage,
                          const struct pipe_box *box,
                          struct pipe_transfer **transfer);

void u_transfer_flush_region_vtbl( struct pipe_context *pipe,
                                   struct pipe_transfer *transfer,
                                   const struct pipe_box *box);

void u_transfer_unmap_vtbl( struct pipe_context *rm_ctx,
                            struct pipe_transfer *transfer );

#endif
