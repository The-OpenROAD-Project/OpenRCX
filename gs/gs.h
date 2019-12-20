///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (c) 2019, Nefelus Inc
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef ADS_GS_H
#define ADS_GS_H

#ifndef ADS_H
#include "ads.h"
#include "array1.h"
#include "util.h"
#endif

#include <vector>


BEGIN_NAMESPACE_ADS

typedef char gsPixel;


#define PIXMAPGRID 32

#if (PIXMAPGRID == 64)
    typedef uint64 pixint;
    typedef unsigned int pixints;
    #define PIXFILL 0xffffffffffffffffLL
    #define PIXMAX 0x8000000000000000LL
    #define PIXADJUST 2
#elif (PIXMAPGRID == 32)
    typedef unsigned int pixint;
    typedef short unsigned int pixints;
    #define PIXFILL 0xffffffff
    #define PIXMAX 0x80000000
    #define PIXADJUST 4
#endif

/* Values for the member variable _init
 * INIT = created, 
 * CONFIGURED = has reasonable values for width, height, slices, etc
 * ALLOCATED = memory has been allocated
 */
#define INIT 0
#define WIDTH 1
#define SLICES 2
#define SCALING 4
#define ALLOCATED 8

#define ALLTYPE (WIDTH | SLICES | SCALING | ALLOCATED)


#define GS_FILE 1
#define GS_STRING 2

#define MAXPRECOLOR 15

#define GS_WHITE 0
#define GS_BLACK 1
#define GS_NONE 3

#define GS_ROW 1     // horizontal segments
#define GS_COLUMN 0  // vertical segments

typedef struct {
    int _ll[2];
    int _ur[2];
    int type;
} SEQ;
        

class gs
{
  public:
    gs();
    ~gs();

    int configure(int xres, int yres, int x0, int y0, int x1, int y1, int nslices);

	// render a rectangle
	int box(int x0, int y0, int x1, int y1, int slice);

	// colorize a slice
	int colorize(int slice,int rgb);

    // call to get the output
	int create_image(FILE*fp, char **out, int output, int encoding);

    // allocate (re-allocate) memory
    int alloc_mem(void);

    // set the scaling parameters
    int scaling(int _x0, int _y0, int _x1, int _y1);

    int clear_layer(int layer);

    // set the number of slices
    int setSlices(int _nslices);

    int get_max_slice();

    char *get_color_value(int slice);

    int get_bw_count(int row, int plane, int &black, int &white);
    int get_seqrow(int y, int plane, int start, int &end, int &bw);
    int get_seqcol(int x, int plane, int start, int &end, int &bw);

    uint get_seq(int *ll, int *ur, uint order, uint plane, Ath__array1D<SEQ*> *array);
    void dump_row(int row, int plane);
    void dump_row2(int row, int plane);

    void show_seq(SEQ * s);
	void release(SEQ *s);
    SEQ *salloc();

    int intersect_rows(int row1, int row2, int store);
    int union_rows(int row1, int row2, int store);
    int xor_rows(int row1, int row2, int store);


  protected:
    int    width;
    int    height;
    int    pwidth;
    int    pheight;
    int    xres;
    int    yres;
    int    nslices;              // max number of slices
    int    maxslice;            // maximum used slice
	int x0,x1,y0,y1;          // bounding box
    int     csize;              // size of the color table
    char    ppmheader[128];

    char    pixbuff[4];

    int _init;

    struct rgb
    {
        char out[5];
    };

    struct rgb * pixcol;

    typedef union 
    {
        pixint lword;
        pixints word[2];
        /*
        unsigned short int sword[4];
        unsigned char  cword[8];
        */
    } pixmap;

    pixmap **plptr;
    pixmap **planes;
    pixmap **plalloc;
    int pixwidth; // how many pixels pixmap is wide, upped to multiple of 
                   // PIXMAPGRID
    int pixwrem;  // how many pixels are used in the last block of a row
    int pixstride;    // how many memory blocks per row
    int pixfullblox;  // how many "full" blocks per row 
                      // (equal to stride, or one less if pixwrem > 0)
    int maxplane;

    pixint start[PIXMAPGRID];
    pixint middle[PIXMAPGRID];
    pixint end[PIXMAPGRID];

    int precolor[MAXPRECOLOR+1];

	AthPool<SEQ> *_seqPool;
    

  public:
    void check_mem2();
    void check_mem();
    void dump_mem(pixmap* pm, int size);
    
  protected:
	inline int R_COLOR(unsigned int p) { return 0xff & ((p & 0xff0000)>>16); }
	inline int G_COLOR(unsigned int p) { return 0xff & ((p & 0x00ff00)>>8); }
	inline int B_COLOR(unsigned int p) { return 0xff & ((p & 0x0000ff)   ); }

	// find the highest slice of each pixel

    // set the size parameters
    int setSize(int _xres, int _yres, int _x0, int _x1, int _y0, int _y1);

    void dump_bytes(char *s);

    int free_mem(void);

    // set up ppm headers
    void init_headers();

	// write all slices as a series of PBM files
	int write_string(char **s, int encoding);


	// write all slices as a series of PBM files
	int write_ppm_file(FILE*fp, int encoding);

	// write all slices as a series of PBM files
	int write_ppm_string(char **s, int encoding);

    // set up initial pixcolor stuff
    void init_pixcol();
    void init_pixbuff();

    SEQ * newseq(int x0, int x1, int y0, int y1, int type);

    int check_slice(int sl);
};

END_NAMESPACE_ADS

#endif
