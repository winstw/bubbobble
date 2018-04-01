

/*

Voici le fichier d'origine que j'ai modifié afin de ne l'utiliser qu'avec du 8bits sans compression

 * tga.c -- tga texture loader
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut tga.c -o tga
 */

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "tga_small.h"
#define ALPHA_R 2
#define ALPHA_G 2
#define ALPHA_B 2



struct gl_texture_t
{
  GLsizei width;
  GLsizei height;

  GLenum format;
  GLint	internalFormat;
  GLuint id;

  GLubyte *texels;
};

/* static GLuint texId = 0; */



#pragma pack(push, 1)
/* TGA header */
struct tga_header_t
{
  GLubyte id_lenght;          /* size of image id */
  GLubyte colormap_type;      /* 1 is has a colormap */
  GLubyte image_type;         /* compression type */

  short	cm_first_entry;       /* colormap origin */
  short	cm_length;            /* colormap length */
  GLubyte cm_size;            /* colormap size */

  short	x_origin;             /* bottom left x coord origin */
  short	y_origin;             /* bottom left y coord origin */

  short	width;                /* picture width (in pixels) */
  short	height;               /* picture height (in pixels) */

  GLubyte pixel_depth;        /* bits per pixel: 8, 16, 24 or 32 */
  GLubyte image_descriptor;   /* 24 bits = 0x00; 32 bits = 0x80 */
};
#pragma pack(pop)

/* OpenGL texture info */

/* /\* Texture id for the demo *\/ */
/* GLuint texId = 0; */




static void ReadTGA8bits_add_alpha (FILE *fp, const GLubyte *colormap, struct gl_texture_t *texinfo);

static struct gl_texture_t *ReadTGAFile (const char *filename);


static void
GetTextureInfo (const struct tga_header_t *header,
		struct gl_texture_t *texinfo)

/* Get informations from header  of tga file */
{
  texinfo->width = header->width;
  texinfo->height = header->height;

	    texinfo->format = GL_RGBA;
	    texinfo->internalFormat = 4;

	    /* 	    texinfo->format = GL_RGB; */
	    /* texinfo->internalFormat = 3; */

}





static void ReadTGA8bits_add_alpha (FILE *fp, const GLubyte *colormap,
	      struct gl_texture_t *texinfo)
{
  int i;
  GLubyte color;
  GLubyte alpha;
  for (i = 0; i < texinfo->width * texinfo->height; ++i)
    {
      /* Read index color byte */
      color = (GLubyte)fgetc (fp);
	
	  
      /* Convert to RGB 24 bits */
      texinfo->texels[(i * 4) + 2] = colormap[(color * 3) + 0];
      texinfo->texels[(i * 4) + 1] = colormap[(color * 3) + 1];
      texinfo->texels[(i * 4) + 0] = colormap[(color * 3) + 2];
/* ----------------------------------------------       */
      /* transparency byte */
      /* je crée une texture 32 bits à partir des 8 bits d'origine 
	 le dernier octet sert à coder la transparence, je le mets à 
	 0 (= transparent ) si le pixel est noir (0, 0, 0) ou presque. */

      if (texinfo->texels[(i*4)+2] <= ALPHA_R && texinfo->texels[(i*4)+1] <= ALPHA_G
	  && texinfo->texels[(i*4)+0] <= ALPHA_B)
	alpha = 0;
      else alpha = 255;

      
      texinfo->texels[(i * 4) + 3] = (GLubyte)alpha;
    }
}


static struct gl_texture_t *ReadTGAFile (const char *filename)
{
  FILE *fp;
  struct gl_texture_t *texinfo;
  struct tga_header_t header;
  GLubyte *colormap = NULL;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return NULL;
    }

  /* Read header */
  fread (&header, sizeof (struct tga_header_t), 1, fp);

  texinfo = (struct gl_texture_t *)
    malloc (sizeof (struct gl_texture_t));
  GetTextureInfo (&header, texinfo);
  fseek (fp, header.id_lenght, SEEK_CUR);

  /* Memory allocation */
  texinfo->texels = (GLubyte *)malloc (sizeof (GLubyte) *
	texinfo->width * texinfo->height * texinfo->internalFormat);
  if (!texinfo->texels)
    {
      free (texinfo);
      return NULL;
    }

  /* Read color map */
  if (header.colormap_type)
    {
      /* NOTE: color map is stored in BGR format */
      colormap = (GLubyte *)malloc (sizeof (GLubyte)
	* header.cm_length * (header.cm_size >> 3));
      fread (colormap, sizeof (GLubyte), header.cm_length
	* (header.cm_size >> 3), fp);
    }


      ReadTGA8bits_add_alpha (fp, colormap, texinfo);

      if (colormap)
	free (colormap);

  fclose (fp);
  return texinfo;
}

GLuint
loadTGATexture (const char *filename)
{
  struct gl_texture_t *tga_tex = NULL;
  GLuint tex_id = 0;
  GLint alignment;

  tga_tex = ReadTGAFile (filename);
  if (!tga_tex)
    fprintf(stderr, "Erreur chargement fichier texture : %s\n", filename );
  if (!tga_tex->texels)
 fprintf(stderr, "Erreur chargement texels : %s\n", filename );   

  if (tga_tex && tga_tex->texels)
    {
      /* Generate texture */
      glGenTextures (1, &tga_tex->id);
      fprintf(stdout, "generation texture ok : %s", filename);
      glBindTexture (GL_TEXTURE_2D, tga_tex->id);
      fprintf(stdout, "chargement texels ok : %s", filename);



      
      /* Setup some parameters for texture filters and mipmapping */


      
      glGetIntegerv (GL_UNPACK_ALIGNMENT, &alignment);
      glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

#if 1

      /* GL_NEAREST donne un effet plus "old_school" */
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      

      
      glTexImage2D (GL_TEXTURE_2D, 0, tga_tex->internalFormat,
		    tga_tex->width, tga_tex->height, 0, tga_tex->format,
		    GL_UNSIGNED_BYTE, tga_tex->texels);
#else
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


      gluBuild2DMipmaps (GL_TEXTURE_2D, tga_tex->internalFormat,
			 tga_tex->width, tga_tex->height,
			 tga_tex->format, GL_UNSIGNED_BYTE, tga_tex->texels);
#endif

      glPixelStorei (GL_UNPACK_ALIGNMENT, alignment);

      tex_id = tga_tex->id;

      /* OpenGL has its own copy of texture data */
      free (tga_tex->texels);
      free (tga_tex);
    }
  fprintf(stdout, "%i", tex_id);
  return tex_id;

}




