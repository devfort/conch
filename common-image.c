/*
 *  Imaging tools for cacaview and img2irc
 *  Copyright (c) 2003-2010 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  This program is free software. It comes without any warranty, to
 *  the extent permitted by applicable law. You can redistribute it
 *  and/or modify it under the terms of the Do What The Fuck You Want
 *  To Public License, Version 2, as published by Sam Hocevar. See
 *  http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include <Imlib2.h>
#include <caca.h>
#include <stdlib.h>
#include <string.h>

#include "common-image.h"

struct image *load_image(char const *name) {
  struct image *im = malloc(sizeof(struct image));
  unsigned int depth, bpp, rmask, gmask, bmask, amask;

  Imlib_Image image;

  /* Load the new image */
  image = imlib_load_image(name);

  if (!image) {
    free(im);
    return NULL;
  }

  imlib_context_set_image(image);
  im->pixels = (char *)imlib_image_get_data_for_reading_only();
  im->w = imlib_image_get_width();
  im->h = imlib_image_get_height();
  rmask = 0x00ff0000;
  gmask = 0x0000ff00;
  bmask = 0x000000ff;
  amask = 0xff000000;
  bpp = 32;
  depth = 4;

  /* Create the libcaca dither */
  im->dither = caca_create_dither(bpp, im->w, im->h, depth * im->w, rmask,
                                  gmask, bmask, amask);
  if (!im->dither) {
    imlib_free_image();
    free(im);
    return NULL;
  }

  im->priv = (void *)image;
  return im;
}

void unload_image(struct image *im) {
  /* Imlib_Image image = (Imlib_Image)im->priv; */
  imlib_free_image();
  caca_free_dither(im->dither);
}
