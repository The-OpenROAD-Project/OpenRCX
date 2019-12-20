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
#include <stdlib.h>
#include <string.h>
#define GS_OLD
#include "gseq.h"

BEGIN_NAMESPACE_ADS


//#define DEBUG_GEN
//#define DEBUG_SEQ_MEM
//#define DEBUG_SEQ
//#define DEBUG_ROW
//#define DEBUG_COL

// ----------------------------------------------------------------------------
//
// gs - Methods
//
// ----------------------------------------------------------------------------
gs::gs()
{

    _init = INIT;

    pixint sum=PIXFILL;
	int i;
    for(i=0; i<PIXMAPGRID; i++)
    {
        start[i]=sum;
        sum = (sum>>1);
    }

    sum = PIXMAX;
    pixint s2 = sum;
    for(i=0;i<PIXMAPGRID;i++)
    {
        end[i] = sum;
        sum = (sum>>1) | PIXMAX;

        middle[i] = s2;
        s2 = (s2>>1);
    }

    nslices = -1;
    maxslice = -1;
    width = -1;
    height = -1;

    x0 = -1;
    y0 = -1;
    x1 = -1;
    y1 = -1;

    precolor[0] = 0xff0000;
	precolor[1] =  0x00ff00;
	precolor[2] =  0x20cfff;
	precolor[3] =  0xffff00;
	precolor[4] =  0x880000;
	precolor[5] =  0x007700;
	precolor[6] =  0x0000ff;
	precolor[7] =  0xff9000;
	precolor[8] =  0xff8080;
	precolor[9] =  0x607710;
	precolor[10] =  0x602090;
	precolor[11] =  0x808080;
	precolor[12] =  0xe040e0;
	precolor[13] =  0x88ff88;
	precolor[14] =  0x000080;
	precolor[15] =  0x805010;

	_seqPool= new AthPool<SEQ>(false, 1024);
}


gs::~gs()
{
    free_mem();
	delete _seqPool;
}

void gs::init_pixcol()
{

    for(int i=0;i<nslices;i++)
    {
        colorize(i,precolor[i]);
        if( i == MAXPRECOLOR)
            break;
    }
}

void gs::init_pixbuff()
{
    pixbuff[0]=0x00;
    pixbuff[1]=0x00;
    pixbuff[2]=0x00;
    pixbuff[3]='\0';

}

void gs::dump_mem(pixmap *pm, int size)
{
    int i;
    fprintf(stderr,"DM: %d\n",size);
    for(i=0;i<size;i++)
    {
        fprintf(stderr,"%x ",pm[i].lword);
        if ( (i % 1000) == 0 )
        {
            fprintf(stderr,"\n");
        }
    }
    fprintf(stderr,"End of DM: %d\n",i);
}
void gs::check_mem()
{
    if( (_init & ALLOCATED) )
    {
        int y;
        int pl;
        for(pl=0;pl<nslices;pl++)
        {
            fprintf(stderr,"Plane: %d at %x\n",pl,(int)planes[pl]);
            for(y=0;y<height;y++)
            {
                //fprintf(stderr,"Row: %d\n",y);
                dump_row(y,pl);
            }
        }
    }
}
void gs::check_mem2()
{
    if( (_init & ALLOCATED) )
    {
        int y;
        int pl;
        for(pl=0;pl<nslices;pl++)
        {
            fprintf(stderr,"\nPlane: %d at %x\n",pl,(int)planes[pl]);
            for(y=0;y<height;y++)
            {
                dump_row2(y,pl);
            }
        }
    }
}

int gs::free_mem()
{
    if( (_init & ALLOCATED) )
    {
        for(int s=0; s<nslices ; s++) 
        {
            free((pixmap**)plalloc[s]);
        }
        free((pixmap*)plalloc);
        free((pixmap*)planes);
        free((pixmap*)plptr);

        free( (struct rgb*) pixcol );

        _init = (_init & (~ALLOCATED) );
        return 0;
    }
    else
    {
        return -1;
    }
}

int gs::alloc_mem()
{
    if( (_init & ALLOCATED) )
    {
        free_mem();
    }

    if( (_init & WIDTH) && (_init & SLICES) )
    {
        plptr  = (pixmap**) calloc(nslices,sizeof(pixmap*));
        planes  = (pixmap**) calloc(nslices,sizeof(pixmap*));
        plalloc  = (pixmap**) calloc(nslices,sizeof(pixmap*));


        pixcol = (struct rgb*)calloc(nslices,sizeof(struct rgb));
        //fprintf(stderr,"Allocing pixcol at: %x (%d)\n",pixcol, nslices*sizeof(struct rgb));


        //fprintf(stderr,"Allocating: %d, %d*%d+%d = %d\n",sizeof(pixmap),height,pixstride,pixstride+PIXADJUST,(height*pixstride+PIXADJUST)*sizeof(pixmap));

        for(int s=0; s<nslices ; s++) 
        {
            pixmap * pm = (pixmap*)calloc(height*pixstride+PIXADJUST,sizeof(pixmap));
            if ( pm == NULL )
            {
                fprintf(stderr,"Error: not enough memory available - allocated %d planes out of %d!!\n",s,nslices);
                exit(-1);
            } 
            else 
            {
                //fprintf(stderr,"Allocated at: %x\n",pm);
            }

            plalloc[s] = pm;

            // align on a 16-byte boundary
            //pm = (pixmap*)(((char*)pm)+15);
            //pm = (pixmap*) ((long)pm >> 4);
            //pm = (pixmap*) ((long)pm << 4);
            planes[s] = pm;


        }


        _init |= ALLOCATED;

        //check_mem();

        init_pixbuff();
        init_pixcol();
        return 0;
    }
    else
    {
        return -1;
    }
}


void gs::init_headers()
{
    // Initialize headers - does NOT depend on memory allocation
    sprintf(ppmheader,"P6\n%d %d\n255\n",width,height);

    char hbuff[128];
    char hb2[4] = "";

    sprintf(hbuff,"P6");
    do 
    {
        hbuff[0]='\0';
        sprintf(hbuff,"P6%s\n%d %d\n255\n",hb2,width,height);
        strcat(hb2," ");
    } while ( (strlen(hbuff) % 3) != 0 );


}

int gs::clear_layer(int layer)
{
    if( (layer < 0 ) || (layer >= nslices) )
    {
        return -1;
    }
    memset(planes[layer],0,(height*pixstride+PIXADJUST)*sizeof(pixmap));
    return 0;
}

int gs::setSlices(int _nslices)
{
    if( nslices != _nslices )
    {
        free_mem();
        nslices = _nslices;
        maxplane = nslices-1;
        _init |= SLICES;
        alloc_mem();
    }
    return 0;
}

int gs::setSize(int _xres, int _yres, int _x0, int _y0, int _x1, int _y1)
{
    x0 = _x0;
    x1 = _x1;
    y0 = _y0;
    y1 = _y1;

    if( x1 <= x0 )
    {
        // to avoid things like divide by 0, etc
        x1 = x0 + 1;
    }

    if (y1 <= y0 )
    {
        // to avoid things like divide by 0, etc
        y1 = y0 + 1;
    }

    xres = _xres;
    yres = _yres;

    width = (x1-x0+1)/xres;
    if ( ( (x1-x0+1) % xres) != 0 )
    {
        width++;
    }
            
    height = (y1-y0+1)/yres;
    if ( ( (y1-y0+1) % yres) != 0 )
    {
        height++;
    }

#ifdef DEBUG_GEN
    fprintf(stderr,"xres/yres: %d, %d\n",xres,yres);
    fprintf(stderr,"x0,x1 y0,y1: %d,%d %d,%d\n",x0,x1,y0,y1);
    fprintf(stderr,"Height/Width: %d, %d\n",height,width);
#endif


    pixwrem = width % PIXMAPGRID;

    pixwidth = width;
    if ( pixwrem != 0 )
    {
        // round off to next multiple of PIXMAPGRID
        pixwidth +=  (PIXMAPGRID - (width % PIXMAPGRID));
    } 

    pixstride = pixwidth / PIXMAPGRID;

    pixfullblox = pixstride;
    if( pixwrem != 0 )
    {
        pixfullblox--;
    }
    pixwrem--;

    //fprintf(stderr,"pixwrem: %d, pixstride: %d\n",pixwrem,pixstride);
    //fprintf(stderr,"pixwidth: %d, pixfullblox: %d\n",pixwidth,pixfullblox);

    //fprintf(stderr,"Sizes: %d, %d, %d, %d\n",sizeof(pixint),sizeof(pixints),sizeof(unsigned short int), sizeof(unsigned char));

    _init |= WIDTH | SCALING;

    alloc_mem();
    init_headers();

    return 0;
}

int gs::configure(int _xres,int _yres, 
        int _x0, int _y0, int _x1, int _y1,int _nslices) {

	assert(_nslices>0);
    
    if( _init & ALLOCATED )
    {
        free_mem();
    }

    //fprintf(stderr,"Configure: %d slices, xres; %d, yres: %d, dimensions: %d,%d to %d,%d\n",_nslices, _xres, _yres,_x0,_y0,_x1,_y1);
    setSize(_xres, _yres, _x0, _y0, _x1, _y1);
    setSlices(_nslices);

    return 0;
}

// CLIP MACRO
#define CLIP(p,min,max) p = (p<min) ? min : (p>=max)? (max - 1) : p

int gs::box(int px0, int py0, int px1, int py1,int sl) {

    if( (sl < 0 ) || (sl >= nslices) )
    {
        fprintf(stderr,"Box in slice %d exceeds maximum configured slice count %d - ignored!\n",sl,nslices);
        return -1;
    }

    if( !(_init & ALLTYPE) )
    {
        return -1;
    }

    if( sl > maxslice )
    {
        maxslice = sl;
    }

    //fprintf(stderr,"Box from: %d,%d to %d,%d\n",px0,py0,px1,py1);

	// normalize bbox
	long a;
	if (px0 > px1)  {
		a = px0 ; px0 = px1 ; px1 = a ; 
	}
	if (py0 > py1) {
		a = py0 ; py0 = py1 ; py1 = a ; 
	}

    if( (px0 < x0) && (px1 < x0) )
        return -1;
    if( (px0 > x1) && (px1 > x1) )
        return -1;
    if( (py0 < y0) && (py1 < y0) )
        return -1;
    if( (py0 > y1) && (py1 > y1) )
        return -1;

	
    // convert to pixel space
	int cx0 = int((px0 - x0) / xres);
	int cx1 = int((px1 - x0) / xres);
	int cy0 = int((py0 - y0) / yres);
	int cy1 = int((py1 - y0) / yres);

	// is it a nil (1 pixel object?) skip.
	//if ((cx0==cx1)&&(cy0==cy1)) {
		//return -1;
	//}

    //fprintf(stderr,"Starting new box: (%d,%d) to (%d,%d)!\n",cx0,cy0,cx1,cy1);
	// render a rectangle on the selected slice. Paint all pixels
	CLIP(cx0,0,width);
	CLIP(cx1,0,width);
	CLIP(cy0,0,height);
	CLIP(cy1,0,height);
    //fprintf(stderr,"Clipped Box from: %d,%d to %d,%d\n",cx0,cy0,cx1,cy1);
    // now fill in planes object


    pixmap *pm;
    pixmap *pcb;
    int xbs = cx0 / PIXMAPGRID;
    int xbe = cx1 / PIXMAPGRID;

    //int xss = cx0 % PIXMAPGRID;
    int xee = cx1 % PIXMAPGRID;

    pixint smask  = start[cx0%PIXMAPGRID];
    pixint emask = end[xee];

    int mb;

    if ( xbe == xbs )
    {
        smask = smask & emask;
    }

    pm = planes[sl] + pixstride*cy0 + xbs;

    for( int yb = cy0; yb <= cy1; yb++)
    {
        // start block
        pcb = pm;
        
        // for next time through loop - allow compiler time for out-of-order
        pm += pixstride;

        // do "start" block
        pcb->lword = pcb->lword | smask;

        // do "middle" block
        for( mb=xbs + 1; mb< xbe;)
        {
            pcb++;
            // moved here to allow for out-of-order execution
            mb++;

            //if( pcb->lword != PIXFILL )
                pcb->lword = PIXFILL;

        }

        // do "end" block
        if ( xbe > xbs )
        {
            pcb++;
            pcb->lword = pcb->lword | emask;

        }

        //fprintf(stderr,"before: %x, after: %x (%d)\n",pm, pm+pixstride, pixstride);
    }

	return 0;
}

// generate an RGB image based on the colors
int gs::create_image(FILE *fp, char **src, int output, int encoding)
{


    int rc=0;
    // file-based output

    if( (output == GS_FILE) )
    {
        rc += write_ppm_file(fp,encoding);
        fflush(fp);
    }
    // string-based output
    else
    {
        rc += write_ppm_string(src,encoding);
    }

    return rc;
}

int gs::check_slice(int sl)
{
    if( (sl < 0 ) || (sl >= nslices) )
    {
        return -1;
    }
    return 0;
}


int gs::union_rows(int sl1, int sl2, int store)
{
    if ( (check_slice(sl1) != 0) || (check_slice(sl2) != 0 ) || (check_slice(store) != 0 ) )
    {
        return -1;
    }
    //fprintf(stderr,"Union rows %d and %d => %d\n",sl1,sl2,store);

            
    pixmap * sp1 = planes[sl1];
    pixmap * sp2 = planes[sl2];
    pixmap * ssp = planes[store];

    pixmap * ep = planes[sl1] + (height*pixstride);
    while( sp1 < ep)
    {
        (ssp++)->lword = (sp1++)->lword | (sp2++)->lword;
    }
    return 0;

}
int gs::intersect_rows(int sl1, int sl2, int store)
{
    if ( (check_slice(sl1) != 0) || (check_slice(sl2) != 0 ) || (check_slice(store) != 0 ) )
    {
        return -1;
    }
    //fprintf(stderr,"Intersect rows %d and %d => %d\n",sl1,sl2,store);
            
    pixmap * sp1 = planes[sl1];
    pixmap * sp2 = planes[sl2];
    pixmap * ssp = planes[store];

    pixmap * ep = planes[sl1] + (height*pixstride);
    while( sp1 < ep)
    {
        (ssp++)->lword = (sp1++)->lword & (sp2++)->lword;
    }
    //fprintf(stderr,"After Intersect rows %d and %d => %d\n",sl1,sl2,store);

    return 0;
}

int gs::xor_rows(int sl1, int sl2, int store)
{
    if ( (check_slice(sl1) != 0) || (check_slice(sl2) != 0 ) || (check_slice(store) != 0 ) )
    {
        return -1;
    }
    //fprintf(stderr,"Xor rows %d and %d => %d\n",sl1,sl2,store);
            
    pixmap * sp1 = planes[sl1];
    pixmap * sp2 = planes[sl2];
    pixmap * ssp = planes[store];

    pixmap * ep = planes[sl1] + (height*pixstride);
    while( sp1 < ep)
    {
        (ssp++)->lword = (sp1++)->lword ^ (sp2++)->lword;
    }
    return 0;

}


void gs::show_seq(SEQ *s)
{
    fprintf(stderr,"Sequence from %d,%d to %d,%d type %d\n",s->_ll[0],s->_ll[1],s->_ur[0],s->_ur[1],s->type);
}


void gs::release(SEQ *s)
{
#ifdef DEBUG_SEQ_MEM
    fprintf(stderr,"Releasing sequence address %x\n",s);
#endif
	_seqPool->free(s);
}

SEQ *gs::salloc()
{
    SEQ *s = _seqPool->alloc();
#ifdef DEBUG_SEQ_MEM
    fprintf(stderr,"Alloced sequence address %x\n",s);
#endif
    return s;
}

/* get_seq - returns an integer corresponding to the longest uninterrupted 
 * sequence of virtual bits found of the same type (set or unset)
 *
 * Parameters: ll - lower left array [0] = x0, [1] = y0
 *             ur - upper right array [0] = x1, [1] = y1
 *             order - search by column or by row (GS_COLUMN, GS_ROW)
 *             plane  - which plane to search
 *             array - pool of sequence pointers to get a handle from
 */
uint gs::get_seq(int *ll, int *ur, uint order, uint plane, Ath__array1D<SEQ*> *array)
{
    if( check_slice(plane) != 0 )
    {
        return 0;
    }

    SEQ * s = salloc();
    SEQ * stmp; static int acnt=0;

    // clip requests that are out of bounds

#if 0
    if ( ll[0] < x0 ) { ll[0] = x0; }
    if ( ll[0] > x1 ) { ll[0] = x1; }

    if ( ll[1] < y0 ) { ll[1] = y0; }
    if ( ll[1] > y1 ) { ll[1] = y1; }

    if ( ur[0] > x1 ) { ur[0] = x1; }
    if ( ur[0] < x0 ) { ur[0] = x0; }

    if ( ur[1] > y1 ) { ur[1] = y1; }
    if ( ur[1] < y0 ) { ur[1] = y0; }
#endif

    // convert into internal coordinates
	int cx0 = int((ll[0] - x0) / xres);
	int cy0 = int((ll[1] - y0) / yres);

	int cx1 = int((ur[0] - x0) / xres);
    if ( ( (ur[0]-x0 + 1) % xres) != 0 ) 
    {
        cx1++;
        // fprintf(stderr,"incrementing (row)!\n");
    }
    else
    {
        // fprintf(stderr,"Not incrementing (row): %d, %d, %d\n",ur[0],x0,xres);
    }
	int cy1 = int((ur[1] - y0) / yres);
    if ( ( (ur[1]-y0 + 1) % yres) != 0 ) 
    {
        cy1++;
        // fprintf(stderr,"Incrementing (col)!\n");
    }
    else
    {
        ; //fprintf(stderr,"Not incrementing (col): %d, %d, %d\n",ur[1],y0,yres);
    }
#ifdef DEBUG_SEQ
     fprintf(stderr,"Virtual: %d,%d to %d,%d\n",ll[0],ll[1],ur[0],ur[1]);
#endif

#ifdef DEBUG_SEQ
    fprintf(stderr,"Converted: %d,%d to %d,%d - direction %d\n",cx0,cy0,cx1,cy1, order);
#endif

    int row,col;
    int rs,re;
    int cs,ce;
    uint blacksum = 0;
    int start, end;
        
    uint tmp;
    bool flag;


    if ( order == GS_ROW )
    {
        //rs = ((cy1+cy0)/2)*yres + y0;
        //re = rs;
        rs = ll[1];
        re = ur[1];

        //rs = cy0*yres + y0;
        //re = rs + yres - 1;

        //if ( rs < ll[1] )
            //rs = ll[1];

        //if ( re > ur[1] )
            //re = ur[1];
        //for(row=cy0; row<=cy1; row++)
        for(row=cy0; row<=cy0; row++)
        {
            //fprintf(stderr,"row: %d\n",row);

            start = cx0;
            flag = false;
            while ( get_seqrow(row,plane,start,end,s->type) == 0 )
            {
#ifdef DEBUG_SEQ
                fprintf(stderr,"Raw Sequence results: start = %d, end = %d\n",start,end);
#endif
                s->_ll[0] = (int)(start*xres + x0);
                s->_ur[0] =  (int)((end+1)*xres + x0 -1);
                if( s->_ur[0] >= ur[0] )
                {
                    flag = true;
                    s->_ur[0] = ur[0];
                }


                if( (row == cy0) && (s->_ll[0] < ll[0]) )
                    s->_ll[0] = ll[0];

                s->_ll[1] = rs;
                s->_ur[1] = re;

#ifdef DEBUG_SEQ
                show_seq(s);
#endif

                if ( s->type == GS_BLACK )
                {
                    //fprintf(stderr,"Blacksum before row: %d\n",blacksum);

                    tmp =  (unsigned)(s->_ur[0] - s->_ll[0] );
                    if ( tmp < 0 )
                    {
                        fprintf(stderr,"Negative in Row: %d\n",tmp);
                    }
                    //fprintf(stderr,"Values: %d, %d, %d, %d (%d)\n",rs,re,s->_ll[0],s->_ur[0],tmp);
                    blacksum = blacksum + tmp;
                    //fprintf(stderr,"Blacksum after row: %d\n",blacksum);
                }

                if( array != NULL )
                {
                    array->add(s);
                    stmp = salloc(); acnt++;
                    if ( s == stmp )
                        fprintf(stderr,"UH OH!! SERIOUS ERROR in alloc: %d!!\n",acnt);

                    s = stmp;
                }
                if( flag == true )
                {
                    release(s);
#ifdef DEBUG_SEQ
                    fprintf(stderr,"Blacksum return row: %d\n",blacksum);
#endif
                    return blacksum;
                }
                start = end + 1;
            }
            rs += yres;
            if ( rs > ur[1] )
                break;
            re += yres;
        }
    }
    else if ( order == GS_COLUMN)
    {
        cs = ((cx1+cx0)/2)*xres + x0;
        ce = cs;
        //cs = cx0*xres + x0;
        //ce = cs + xres -1;

        if ( cs < ll[0] )
        {
            cs = ll[0];
            ce = cs;
        }

        if ( ce > ur[0] )
            ce = ur[0];


        // check for special net
        //for(col=cx0; col<=cx1; col++)
        for(col=cx0; col<=cx0; col++)
        {
            start = cy0;
            flag = false;
            while ( get_seqcol(col,plane,start,end,s->type) == 0 )
            {
                s->_ll[1] = (int)(start*yres + y0);
                s->_ur[1] =  (int)((end+1)*yres + y0 -1);
                if( s->_ur[1] >= ur[1] )
                {
                    flag = true;
                    s->_ur[1] = ur[1];
                }

                if( (col == cx0) && (s->_ll[1] < ll[1]) )
                    s->_ll[1] = ll[1];

                s->_ll[0] = cs;
                s->_ur[0] = ce;

                if ( s->type == GS_BLACK )
                {
                    //fprintf(stderr,"Blacksum before col: %d\n",blacksum);

                    tmp = (unsigned)((s->_ur[1]) - (s->_ll[1]) );
                    if ( tmp < 0 )
                    {
                        fprintf(stderr,"Negative in Col: %d\n",tmp);
                    }

                    //fprintf(stderr,"Values: %d, %d, %d, %d, %d, %d (%d)\n",cs,ce,s->_ll[1],s->_ur[1],start,end,tmp);
                    blacksum = blacksum + tmp;
                    //fprintf(stderr,"Blacksum after col: %d\n",blacksum);
                }

                if( array != NULL )
                {
                    array->add(s);
                    stmp = salloc(); acnt++;
                    if ( s == stmp )
                        fprintf(stderr,"UH OH!! SERIOUS ERROR in alloc: %d!!\n",acnt);

                    s = stmp;
                }
                if( flag == true )
                {
                    release(s);
#ifdef DEBUG_SEQ
                    fprintf(stderr,"Blacksum return col: %d\n",blacksum);
#endif
                    return blacksum;
                }

                start = end + 1;
            }
            cs += xres;
            if ( cs > ur[0] )
            {
                //fprintf(stderr,"breaking out early: %d, %d\n",cs, ur[0]);
                break;
            }
            ce += xres;
        }
    }
    release(s);
#ifdef DEBUG_SEQ
    fprintf(stderr,"Blacksum return final: %d\n",blacksum);
#endif
    return blacksum;
}

int gs::get_seqrow(int y, int plane, int stpix, int &epix, int &seqcol)
{
    if( !(_init & ALLOCATED) )
    {
        return -1;
    }

    if ( y >= height )
        return -1;

    if ( stpix >= width )
        return -1;

#ifdef DEBUG_ROW
    fprintf(stderr,"GSR: %d/%d, %d, %d\n",y,plane,stpix,epix);
#endif
    //pixint *bmask;
    int st; int bit;

    long offset;

    int sto = stpix / PIXMAPGRID;
    int str = stpix - (sto*PIXMAPGRID);

#ifdef DEBUG_ROW
    fprintf(stderr,"GSR2: %d/%d\n",sto,str);
#endif
    offset = y * pixstride + sto;
    pixmap *pl = planes[plane]+offset;

    seqcol=GS_NONE;
    pixint bc;

    // take care of start
#ifdef DEBUG_ROW
    fprintf(stderr,"%x, %x, %x\n",pl->lword, start[str], pl->lword  & start[str]);
#endif
    if ( (pl->lword & (start[str])) == start[str] )
    {

        seqcol = GS_BLACK;
        sto++;
        pl++;
    } else if ( (pl->lword & (start[str])) == 0 )
    {
        seqcol = GS_WHITE;
        sto++;
        pl++;
    }
    else
    {
        // ends here already
        bc = (pl->lword) & middle[str];
        if ( bc != 0 )
        {
            seqcol = GS_BLACK;
        }
        else
        {
            seqcol = GS_WHITE;
        }
        epix = (sto*PIXMAPGRID)+str;
        for(bit=str+1;bit<PIXMAPGRID;bit++)
        {
#ifdef DEBUG_ROW
            fprintf(stderr,"HERE (%d/%d)!\n",bit,seqcol);
            fprintf(stderr,"%x, %x, %d\n",pl->lword, middle[bit], (pl->lword)  & (middle[bit]));
#endif
            if( ( (pl->lword) & middle[bit] ) == 0 )
            {
#ifdef DEBUG_ROW
                fprintf(stderr,"0\n");
#endif
                
                if ( seqcol == GS_BLACK )
                {
                    break;
                }
            }
            else
            {
#ifdef DEBUG_ROW
                fprintf(stderr,"1\n");
#endif
                if ( seqcol == GS_WHITE )
                {
                    break;
                }
            }

            epix++;
        }
        return 0;
    }

    if ( sto > pixfullblox )
    {
        epix = width -1;
        return 0;
    }
        
    for(st=sto; st<pixfullblox; st++)
    {
#ifdef DEBUG_ROW
        fprintf(stderr,"Continuing with st = %d, end = %d\n",sto,epix);
#endif

        if ( (pl->lword ) == PIXFILL )
        {
#ifdef DEBUG_ROW
            fprintf(stderr,"PM!\n");
#endif
            if( seqcol != GS_BLACK )
            {
                epix = st*PIXMAPGRID-1;
                return 0;
            }
        } 
        else if ( pl->lword == 0 )
        {
#ifdef DEBUG_ROW
            fprintf(stderr,"ZERO!\n");
#endif
            if ( seqcol != GS_WHITE )
            {
                epix = st*PIXMAPGRID-1;
                return 0;
            }
        } 
        else
        {
#ifdef DEBUG_ROW
            fprintf(stderr,"MIXED: %x!\n",pl->lword);
#endif
            // ends here 
            epix = st*PIXMAPGRID-1;
            for(bit=0;bit<PIXMAPGRID;bit++)
            {
                if( ( (pl->lword) & middle[bit] ) == 0 )
                {
                    if ( seqcol == GS_BLACK )
                    {
                        break;
                    }
                }
                else
                {
                    if ( seqcol == GS_WHITE )
                    {
                        break;
                    }
                }

                epix++;
            }
            return 0;
        }
        pl++;
    }
    // handle non-even case
    epix = st*PIXMAPGRID-1;
#ifdef DEBUG_ROW
    fprintf(stderr,"end case: %d\n",epix);
#endif
    for(bit=0;bit<=pixwrem;bit++)
    {
        if( ( (pl->lword) & middle[bit] ) == 0 )
        {
            if ( seqcol == GS_BLACK )
            {
                break;
            }
        }
        else
        {
            if ( seqcol == GS_WHITE )
            {
                break;
            }
        }
        epix++;
    }
    return 0;

}

int gs::get_seqcol(int x, int plane, int stpix, int &epix, int &seqcol)
{
    if( !(_init & ALLOCATED) )
    {
        return -1;
    }

    if ( x >= width )
        return -1;

    if ( stpix >= height )
        return -1;

#ifdef DEBUG_COL
    fprintf(stderr,"GSC: %d/%d, %d, %d\n",x,plane,stpix,epix);
#endif

    long offset;

    // get proper "word" offset
    int sto = x / PIXMAPGRID;
    int stc = x - (sto * PIXMAPGRID);

#ifdef DEBUG_COL
    fprintf(stderr,"GSC2: %d/%d\n",sto,stc);
#endif
    int row = stpix;
    offset = sto + row*pixstride;
    pixint bitmask = middle[stc];
    pixmap *pl = planes[plane]+offset;

#ifdef DEBUG_COL
    fprintf(stderr,"GSC3: %x %x %x\n",planes[plane],offset,*pl);
#endif
    seqcol=GS_NONE;


    if ( (pl->lword) & bitmask )
    {
        seqcol = GS_BLACK;
    }
    else
    {
        seqcol = GS_WHITE;
    }

    for(row = stpix+1; row < height; row++)
    {
        pl += pixstride;
        if ( (pl->lword) & bitmask )
        {
            if ( seqcol == GS_BLACK)
            {
                continue;
            }
            else
            {
                epix = row-1;
                return 0;
            }
        }
        else 
        {
            if ( seqcol == GS_BLACK)
            {
                epix = row-1;
                return 0;
            }
            else
            {
                continue;
            }
        }
    }

    epix = height;
    return 0;
}


int gs::get_bw_count(int y, int plane, int &black, int &white)
{

    if( !(_init & ALLOCATED) )
    {
        return -1;
    }

    black = 0;
    white=0;

    pixint *bmask;
    int st; int bit;

    long offset;


    offset = y * pixstride;
    pixmap * pl = planes[plane]+offset;

    for(st=0; st<pixfullblox; st++)
    {
        if ( pl->lword == PIXFILL ) 
        {
            //fprintf(stderr,"b1\n");
            black += PIXMAPGRID;
        }
        else if ( pl->lword == 0 )
        {
            //fprintf(stderr,"w1\n");
            white += PIXMAPGRID;
        }
        else
        {
            bmask = middle;
            for(bit=0;bit<PIXMAPGRID;bit++)
            {
                if( (pl->lword) & (*bmask) )
                {
                    //fprintf(stderr,"b2\n");
                    black++;
                } 
                else
                {
                    //fprintf(stderr,"w2\n");
                    white++;
                }
                bmask++;
            }
        }
        pl++;
    }
    // handle non-even case
    bmask = middle;
    for(bit=0;bit<=pixwrem;bit++)
    {
        if( (pl->lword) & (*bmask) )
        {
            black++;
        } 
        else
        {
            white++;
        }
        bmask++;
    }

    //fprintf(stderr,"Row %d, plane %d, %d white, %d black\n",y,plane,white,black);
    return 0;
}

int gs::write_ppm_file(FILE*fp, int encoding)
{

    if( !(_init & ALLOCATED) )
    {
        return -1;
    }

	//fprintf(fp,"P3\n");
    struct rgb * pixlookup;

    char * buffptr;
    int bufflen;

    if ( encoding == 255 )
    {
        pixlookup = pixcol;
        buffptr = pixbuff;
        bufflen = 3;
        //fprintf(fp,"%s",ppmheader);
    } 
    else 
    {
        fprintf(stderr,"Unsupported encoding style!\n");
        return -1;
    }

    pixint *bmask;
    int py; int st; int pln; int bit;

    char * outptr;
    long offset;

    //pixint *bend=&middle[PIXMAPGRID];

    for(py=height-1; py>=0; py--)
    {
        offset = py * pixstride;
        for(pln = maxplane; pln >= 0; pln--)
        {
            plptr[pln] = planes[pln]+offset;
        }
        for(st=0; st<pixfullblox; st++)
        {
            bmask = middle;
            for(bit=0;bit<PIXMAPGRID;bit++)
            {
                outptr = buffptr;
                for(pln = maxplane; pln >=0; pln--)
                {
                    if( (plptr[pln]->lword) & (*bmask) )
                    {
                        outptr = pixlookup[pln].out;
                        break;
                    }
                }
                fwrite(outptr,1,bufflen,fp);
                bmask++;
            }
            for(pln = maxplane; pln >=0; pln--)
            {
                plptr[pln]++;
            }
        }
        // handle non-even case
        bmask = middle;
        for(bit=0;bit<=pixwrem;bit++)
        {
            outptr = buffptr;
            for(pln = maxplane; pln >=0; pln--)
            {
                if( (plptr[pln]->lword) & (*bmask) )
                {
                    outptr = pixlookup[pln].out;
                    break;
                }
            }
            fwrite(outptr,1,bufflen,fp);
            bmask++;
        }
    }

    fflush(fp);
    return 0;

}

int gs::write_ppm_string(char **s, int encoding) 
{
    if( !(_init & ALLOCATED) )
    {
        return -1;
    }

	//fprintf(fp,"P3\n");
    struct rgb * pixlookup;

    char * buffptr;
    int bufflen;

    char *stout;

    if ( encoding == 255 )
    {
        pixlookup = pixcol;
        buffptr = pixbuff;
        bufflen = 3;
        // add a little extra for base-64 padding and header
        *s = (char*)malloc(bufflen * width * height+128);
        stout = *s;
        sprintf(stout,"%s",ppmheader);
    } 
    {
        fprintf(stderr,"Unsupported encoding style!\n");
        return -1;
    }
    //fprintf(stderr,"Starting string write!\n");
    //dump_bytes(stout);
    stout = (stout + strlen(stout));

    // just for testing

    pixint *bmask;
    int py; int st; int pln; int bit;

    char * outptr;
    long offset;

    //pixint *bend=&middle[PIXMAPGRID];
    int buffcopy;

    for(py=height-1; py>=0; py--)
    {
        offset = py * pixstride;
        for(pln = maxplane; pln >= 0; pln--)
        {
            plptr[pln] = planes[pln]+offset;
        }
        for(st=0; st<pixfullblox; st++)
        {
            bmask = middle;
            for(bit=0;bit<PIXMAPGRID;bit++)
            {
                outptr = buffptr;
                for(pln = maxplane; pln >=0; pln--)
                {
                    if( (plptr[pln]->lword) & (*bmask) )
                    {
                        outptr = pixlookup[pln].out;
                        break;
                    }
                }
                for(buffcopy=0; buffcopy<bufflen; buffcopy++)
                {
                    *stout++=*outptr++;
                }
                bmask++;
            }
            for(pln = maxplane; pln >=0; pln--)
            {
                plptr[pln]++;
            }
        }
        // handle non-even case
        bmask = middle;
        for(bit=0;bit<=pixwrem;bit++)
        {
            outptr = buffptr;
            for(pln = maxplane; pln >=0; pln--)
            {
                if( (plptr[pln]->lword) & (*bmask) )
                {
                    outptr = pixlookup[pln].out;
                    break;
                }
            }
            for(buffcopy=0; buffcopy<bufflen; buffcopy++)
            {
                *stout++=*outptr++;
            }
            bmask++;
        }
    }
    *stout='\0';
    //fprintf(stderr,"Done string write!\n");
    //dump_bytes(*s);

    return 0;
}

int gs::get_max_slice()
{
    return maxslice;
}

char *gs::get_color_value(int slice)
{
    if( (slice < 0) || (slice >= nslices) )
    {
        return NULL;
    }
    return pixcol[slice].out;
}

int gs::colorize(int slice,int rgb) {

    if( !(_init & ALLOCATED) )
    {
        return -1;
    }

    /*
    if ( slice >= nslices ) 
    {
        fprintf(stderr,"Slice: %d, nslices: %d, rgb: %d\n",slice,nslices, rgb);
    }
    */
	assert(slice < nslices);
	assert(slice >= 0);
    char buff[4];
    sprintf(buff,"%c%c%c",R_COLOR(rgb),G_COLOR(rgb),B_COLOR(rgb));
    //fprintf(stderr,"COL: %x %x %x %x\n",&buff, &pixcol[slice].out[0], &pixcol[slice].out[1], &pixcol[slice].out[2]);
    pixcol[slice].out[0]=buff[0];
    pixcol[slice].out[1]=buff[1];
    pixcol[slice].out[2]=buff[2];
    pixcol[slice].out[3]='\0';

    return 0;
}


void gs::dump_bytes(char *s)
{
    fprintf(stderr,"First values:\n");
    if( s == NULL )
    {
        fprintf(stderr,"s is null!\n");
        return;
    }
    for(int i=0; i<width; i++)
    {
        fprintf(stderr,"%d: %x %x %x ",i,*(s+i),*(s+i+1),*(s+i+2));
        if( (i % 2) == 0 )
        {
            fprintf(stderr,"\n");
        }
    }
}

void gs::dump_row(int row, int plane)
{

    pixmap * pm = planes[plane]+(row*pixstride);
    for(int i=0;i<pixstride;i++)
    {
        fprintf(stderr,"address: %x (%x, %d, %d, %d)\n",(int)&(pm->lword),(int)planes[plane],i,row,pixstride);
        fprintf(stderr,"element %d: %x\n",i,pm->lword);
        pm++;
    }
}
void gs::dump_row2(int row, int plane)
{

    pixmap * pm = planes[plane]+(row*pixstride);
    int header=0;
    for(int i=0;i<pixstride;i++)
    {
        if ( pm->lword != 0 )
        {
            if ( header == 0 )
            {
                fprintf(stderr,"\nPlane %d, row %d\n",plane, row);
                header++;
            }
            //fprintf(stderr,"address: %x (%x, %d, %d, %d)\n",(int)&(pm->lword),(int)planes[plane],i,row,pixstride);
            fprintf(stderr,"%8d: %8x  ",i,pm->lword);
        }
        pm++;
    }
}

END_NAMESPACE_ADS
