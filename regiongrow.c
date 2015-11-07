	/* Author: Dr. Adam Hoover
  **
  ** This code is needed by range.c. The original author is Dr. Adam Hoover
  ** of the ECE department, Clemson University.
  **
	** Reads a greyscale image from the comand line, and creates
	** a segmentation of regions based upon similar greyscale areas.
	** This version demonstrates how to use a paint-fill technique
	** along with region criteria to do segmentation.
	**
	** Good demonstration on targets.ppm.
	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SQR(x) ((x)*(x))

void main(int argc, char *argv[])
{
unsigned char	*image,*labels;
FILE		*fpt;
char		header[80];
int		ROWS2,COLS2,BYTES,r,c,r2,c2;
int		*indices,i;
int		RegionSize,*RegionPixels,TotalRegions;
double		avg,var;
void		RegionGrow();

fopen("threshold_binary.ppm", "rb");
fscanf(fpt,"%s %d %d %d",header,&COLS2,&ROWS2,&BYTES);

printf("error 1\n");
image=(unsigned char *)calloc(ROWS2*COLS2,sizeof(unsigned char));
	/* segmentation image = labels; calloc initializes all labels to 0 */
labels=(unsigned char *)calloc(ROWS2*COLS2,sizeof(unsigned char));
	/* used to quickly erase small grown regions */
indices=(int *)calloc(ROWS2*COLS2,sizeof(int));
header[0]=fgetc(fpt);	/* whitespace character after header */
fread(image,1,ROWS2*COLS2,fpt);
fclose(fpt);
printf("error 2\n");
TotalRegions=0;
for (r=3; r<ROWS2-3; r++)
  {
  for (c=3; c<COLS2-3; c++)
    {
    if (labels[r*COLS2+c] != 0)
      continue;
    avg=var=0.0;	/* compute average and variance in 7x7 window */
    for (r2=-3; r2<=3; r2++)
      for (c2=-3; c2<=3; c2++)
         avg+=(double)(image[(r+r2)*COLS2+(c+c2)]);
    avg/=49.0;
    for (r2=-3; r2<=3; r2++)
      for (c2=-3; c2<=3; c2++)
         var+=SQR(avg-(double)image[(r+r2)*COLS2+(c+c2)]);
    var=sqrt(var)/49.0;
    if (var < 1.0)	/* condition for seeding a new region is low var */
      {
      // printf("%d,%d avg=%lf var=%lf\n",r,c,avg,var);
      TotalRegions++;
      if (TotalRegions == 255)
        {
        printf("Segmentation incomplete.  Ran out of labels.\n");
        break;
        }
      RegionGrow(image,labels,ROWS2,COLS2,r,c,0,TotalRegions,
		indices,&RegionSize);
      if (RegionSize < 100)
        {	/* erase region (relabel pixels back to 0) */
        for (i=0; i<RegionSize; i++)
          labels[indices[i]]=0;
        TotalRegions--;
        }
      else
        printf("Region labeled %d is %d in size\n",TotalRegions,RegionSize);
      }
    }
  if (c < COLS2-3)
    break;	/* ran out of labels -- break both loops */
  }
printf("%d total regions were found\n",TotalRegions);
printf("error 3\n");
if ((fpt=fopen("seg.ppm","wb")) == NULL)
  {
  printf("Unable to open file for writing\n");
  exit(0);
  }
fprintf(fpt,"P5 %d %d 255\n",COLS2,ROWS2);
fwrite(labels,1,ROWS2*COLS2,fpt);
fclose(fpt);
}


	/*
	** Given an image, a starting point, and a label, this routine
	** paint-fills (8-connected) the area with the given new label
	** according to the given criteria (pixels close to the average
	** intensity of the growing region are allowed to join).
	*/

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow(unsigned char *image,	/* image data */
		unsigned char *labels,	/* segmentation labels */
		int ROWS2,int COLS2,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count)		/* output:  count of pixels painted */
{
int	r2,c2;
int	queue[MAX_QUEUE],qh,qt;
int	average,total;	/* average and total intensity in growing region */

*count=0;
if (labels[r*COLS2+c] != paint_over_label)
  return;
labels[r*COLS2+c]=new_label;
average=total=(int)image[r*COLS2+c];
if (indices != NULL)
  indices[0]=r*COLS2+c;
queue[0]=r*COLS2+c;
qh=1;	/* queue head */
qt=0;	/* queue tail */
(*count)=1;
while (qt != qh)
  {
  if ((*count)%50 == 0)	/* recalculate average after each 50 pixels join */
    {
    average=total/(*count);
    // printf("new avg=%d\n",average);
    }
  for (r2=-1; r2<=1; r2++)
    for (c2=-1; c2<=1; c2++)
      {
      if (r2 == 0  &&  c2 == 0)
        continue;
      if ((queue[qt]/COLS2+r2) < 0  ||  (queue[qt]/COLS2+r2) >= ROWS2  ||
	  (queue[qt]%COLS2+c2) < 0  ||  (queue[qt]%COLS2+c2) >= COLS2)
        continue;
      if (labels[(queue[qt]/COLS2+r2)*COLS2+queue[qt]%COLS2+c2]!=paint_over_label)
        continue;
		/* test criteria to join region */
      if (abs((int)(image[(queue[qt]/COLS2+r2)*COLS2+queue[qt]%COLS2+c2])
		-average) > 10)
        continue;
      labels[(queue[qt]/COLS2+r2)*COLS2+queue[qt]%COLS2+c2]=new_label;
      if (indices != NULL)
        indices[*count]=(queue[qt]/COLS2+r2)*COLS2+queue[qt]%COLS2+c2;
      total+=image[(queue[qt]/COLS2+r2)*COLS2+queue[qt]%COLS2+c2];
      (*count)++;
      queue[qh]=(queue[qt]/COLS2+r2)*COLS2+queue[qt]%COLS2+c2;
      qh=(qh+1)%MAX_QUEUE;
      if (qh == qt)
        {
        printf("Max queue size exceeded\n");
        exit(0);
        }
      }
  qt=(qt+1)%MAX_QUEUE;
  }
}
