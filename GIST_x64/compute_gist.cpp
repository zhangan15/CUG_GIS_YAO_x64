/* Lear's GIST implementation, version 1.1, (c) INRIA 2009, Licence: PSFL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;


#ifndef USE_GIST
#define USE_GIST
#endif

#ifndef STANDALONE_GIST
#define STANDALONE_GIST
#endif

#include "gist.h"


static color_image_t *Random_produce_ppm(int width, int height) {
  color_image_t *im=color_image_new(width,height);

  srand(time_t(NULL));
  int i;
  for(i=0;i<width*height;i++) {
//    im->c1[i]=fgetc(f);
//     if(px==6) {
//       im->c2[i]=fgetc(f);
//       im->c3[i]=fgetc(f);    
//     } else {
//       im->c2[i]=im->c1[i];
//       im->c3[i]=im->c1[i];   
//     }
	  im->c1[i] = rand()%1024;
	  im->c2[i] = rand()%1024;
	  im->c3[i] = rand()%1024;
  }
  
  return im;
}

//im must malloc memeory
template<class TT> bool getGistImg(TT* pdata, int nwidth, int nheight, int nbands, color_image_t*& im, bool bIsBSQ = true)
{
	if (im->width != nwidth || im->height != nheight)
	{
		cout<<"error image"<<endl;
		return false;
	}

	int i, j;
	if (nbands == 1)
	{
		for (i=0; i<nwidth*nheight; i++)
		{
			im->c1[i] = float(pdata[i]);
			im->c2[i] = float(pdata[i]);
			im->c3[i] = float(pdata[i]);
		}		
	}
	else if (nbands == 3)
	{
		if (bIsBSQ)
		{
			for (i=0; i<nheight; i++)
			{
				for (j=0; j<nwidth; j++)
				{
					im->c1[i*nheight+j] = (float)pdata[0*nwidth*nheight+i*nheight+j];
					im->c2[i*nheight+j] = (float)pdata[1*nwidth*nheight+i*nheight+j];
					im->c3[i*nheight+j] = (float)pdata[2*nwidth*nheight+i*nheight+j];
				}
			}				
		}
		else
		{
			for (i=0; i<nheight; i++)
			{
				for (j=0; j<nwidth; j++)
				{
					im->c1[i*nwidth+j] = (float)pdata[i*nwidth*nbands+j*nbands+0];
					im->c2[i*nwidth+j] = (float)pdata[i*nwidth*nbands+j*nbands+1];
					im->c3[i*nwidth+j] = (float)pdata[i*nwidth*nbands+j*nbands+2];
				}
			}
		}		
	}
	else
	{
		cout<<"unknown data"<<endl;
		return false;
	}

	return true;
}


static void usage(void) {
  fprintf(stderr,"compute_gist options... [infilename]\n"
          "infile is a PPM raw file\n"
          "options:\n"
          "[-nblocks nb] use a grid of nb*nb cells (default 4)\n"
          "[-orientationsPerScale o_1,..,o_n] use n scales and compute o_i orientations for scale i\n"
          );
  
  exit(1);
}



int main(int argc,char **args) {
  
 // const char *infilename="./ar.ppm";
  int width = 500, height = 500;
  int nblocks=4;
  int n_scale=4;
  int orientations_per_scale[50]={8,8,8,8};

  int descsize=0;
  /* compute descriptor size */
  int i;
  for(i=0;i<n_scale;i++) 
	  descsize+=nblocks*nblocks*orientations_per_scale[i];

  descsize*=3; /* color */

  cout<<"desc size = "<<descsize<<endl;
  
  /*
  while(*++args) {
    const char *a=*args;
    
    if(!strcmp(a,"-h")) usage();
    else if(!strcmp(a,"-nblocks")) {
      if(!sscanf(*++args,"%d",&nblocks)) {
        fprintf(stderr,"could not parse %s argument",a); 
        usage();
      }
    } else if(!strcmp(a,"-orientationsPerScale")) {
      char *c;
      n_scale=0;
      for(c=strtok(*++args,",");c;c=strtok(NULL,",")) {
        if(!sscanf(c,"%d",&orientations_per_scale[n_scale++])) {
          fprintf(stderr,"could not parse %s argument",a); 
          usage();         
        }
      }
    } else {
      infilename=a;
    }
  }
  */

  
  color_image_t *im=Random_produce_ppm(width, height);

  
  float *desc=color_gist_scaletab(im,nblocks,n_scale,orientations_per_scale);   
  
  /* print descriptor */
  for(i=0;i<descsize;i++) 
    printf("%.4f ",desc[i]);

  printf("\n");
  
  free(desc);



  color_image_delete(im);

  return 0; 
}
