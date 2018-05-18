/* Lear's GIST implementation, version 1.1, (c) INRIA 2009, Licence: PSFL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <QtCore>
#include <QtGui>
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

//load image
bool loadImage(char* sfilename, color_image_t*& im)
{
	QImage img;
	if (!img.load(QString(sfilename)))
	{
		cout << "load file error: " << sfilename << endl;
		return false;
	}

	

	//img = img.convertToFormat(QImage::Format_RGB888);

	
	int nheight = img.height();
	int nwidth = img.width();
	int nbands = 3;

	if (im != NULL)
		color_image_delete(im);
	im = color_image_new(nwidth, nheight);

	int i = 0, j = 0;
	uchar* pdata = img.bits();

	//QImage img_test(nwidth, nheight, QImage::Format_RGB888);

	QColor _color;

	for (i = 0; i < nheight; i++)
	{
		for (j = 0; j < nwidth; j++)
		{
			_color = img.pixel(j, i);
			im->c1[i*nwidth + j] = (float)_color.red();
			im->c2[i*nwidth + j] = (float)_color.green();
			im->c3[i*nwidth + j] = (float)_color.blue();

			
			//img_test.setPixel(j, i, _color.rgb());					
			
		}
	}

	//img_test.save("hello.jpg");

	return true;
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

// 	color_image_t* im1 = NULL;
// 	loadImage("./113.221916_23.109324_0.png", im1);
// 	color_image_delete(im1);
// 	im1 = NULL;
	
	//return 1;
  
 // const char *infilename="./ar.ppm";
  //int width = 500, height = 500;
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

   
  color_image_t *im = NULL;//Random_produce_ppm(width, height);

  QFileInfoList filist = QDir("H:\\WuhanStreetview\\wuhanStreetPhoto\\").entryInfoList(QStringList(QString("*.png")));
  cout << "file size = " << filist.size() << endl;

  QFile outfile("H:\\WuhanStreetview\\Wwuhan_gist_features.csv");
  outfile.open(QIODevice::WriteOnly);
  QTextStream _in(&outfile);

  QString str = "filename";
  for (int i=0; i<descsize; i++)
	  str += QString(", %1").arg(i);
  _in << str << "\r\n";
  _in.flush();

  int nCount = 0;
  foreach (QFileInfo fi, filist)
  {
	  nCount++;
	  if (!loadImage(fi.absoluteFilePath().toLocal8Bit().data(), im))
	  {
		  cout << "[error] load image fail. image name = " << fi.absoluteFilePath().toLocal8Bit().data() << endl;
		  continue;
	  }
	  cout << nCount << " / "<< filist.size() <<" - load image success. image name = " << fi.absoluteFilePath().toLocal8Bit().data() << endl;

	  //read file
	  float *desc = color_gist_scaletab(im, nblocks, n_scale, orientations_per_scale);

	  QString str = fi.completeBaseName();
	  for (int i = 0; i < descsize; i++)
		  str += QString(", %1").arg(desc[i], 0, 'f', 6);
	  _in << str << "\r\n";
	  _in.flush();
	  /* print descriptor */
// 	  for (i = 0; i < descsize; i++)
// 		  printf("%.4f ", desc[i]);
// 
// 	  printf("\n");

	  free(desc);
  }
  
  



  color_image_delete(im);

  return 0; 
}
