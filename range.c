/*
* Author: Aditya Bettadapura
*
* This program reads an image and takes contour points from a text file.
* Using an iterative process, it draws the controur around the desired segment.
* 
* Code for Odetics to Coordinates given by Dr. Adam Hoover
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define SQUARE(x) ((x)*(x))

#define ROWS	128
#define COLS	128

#define MAX_QUEUE 10000
#define PI 3.14
/*
**	This routine converts the data in an Odetics range image into 3D
**	cartesian coordinate data.  The range image is 8-bit, and comes
**	already separated from the intensity image.
*/


void OdeticsCoordinates(void)
{
int	r,c;
double	cp[7];
double	xangle,yangle,dist;
double	ScanDirectionFlag,SlantCorrection;
unsigned char	RangeImage[128*128];
double		P[3][128*128];
int             ImageTypeFlag;
char	Filename[160],Outfile[160];
FILE	*fpt;

printf("Enter range image file name:");
scanf("%s",Filename);
if ((fpt=fopen(Filename,"r")) == NULL)
  {
  printf("Couldn't open %s\n",Filename);
  exit(0);
  }
fread(RangeImage,1,128*128,fpt);
fclose(fpt);

printf("Up(-1), Down(1) or Neither(0)? ");
scanf("%d",&ImageTypeFlag);


cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
cp[1]=32.0;		/* scan time per single pixel in microseconds */
cp[2]=(COLS/2)-0.5;		/* middle value of columns */
cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

switch(ImageTypeFlag)
  {
  case 1:		/* Odetics image -- scan direction upward */
    ScanDirectionFlag=-1;
    break;
  case 0:		/* Odetics image -- scan direction downward */
    ScanDirectionFlag=1;
    break;
  default:		/* in case we want to do this on synthetic model */
    ScanDirectionFlag=0;
    break;
  }

	/* start with semi-spherical coordinates from laser-range-finder: */
	/*			(r,c,RangeImage[r*COLS+c])		  */
	/* convert those to axis-independant spherical coordinates:	  */
	/*			(xangle,yangle,dist)			  */
	/* then convert the spherical coordinates to cartesian:           */
	/*			(P => X[] Y[] Z[])			  */

if (ImageTypeFlag != 3)
  {
  for (r=0; r<ROWS; r++)
    {
    for (c=0; c<COLS; c++)
      {
      SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
      xangle=cp[0]*cp[1]*((double)c-cp[2]);
      yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+	/* Standard Transform Part */
	SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
      dist=(double)RangeImage[r*COLS+c]+cp[6];
      P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
	+(tan(yangle)*tan(yangle))));
      P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
      P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
      }
    }
  }

sprintf(Outfile,"%s.coords",Filename);
fpt=fopen(Outfile,"w");
// fwrite(P[0],8,128*128,fpt);
// fwrite(P[1],8,128*128,fpt);
// fwrite(P[2],8,128*128,fpt);
// for (int i = 0; i < 128*128; i++)
// {
//   fprintf(fpt, "%lf %lf %lf\n",P[0][i], P[1][i], P[2][i]);
// }

fwrite(P[0],128*128,8,fpt);
fwrite(P[1],128*128,8,fpt);
fwrite(P[2],128*128,8,fpt);
fclose(fpt);
}

void CrossProduct(double a[], double b[], double norm_x[], double norm_y[], double norm_z[], int r, int inputCols, int c)
{
	norm_x[r*inputCols+c] = a[1]*b[2]-a[2]*b[1];
	norm_y[r*inputCols+c] = a[2]*b[0]-a[0]*b[2];
	norm_z[r*inputCols+c] = a[0]*b[1]-a[1]*b[0];
}

double DotProduct(double a[3], double b[3])
{
	double mod_a = sqrt((pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2)));
	double mod_b = sqrt((pow(b[0], 2) + pow(b[1], 2) + pow(b[2], 2)));
	double adotb = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	double theta = acos(adotb / (mod_a*mod_b)) * 180 / PI;
	return theta;
}

void main(int argc, char *argv[])
{

	FILE *fptr;
	char *inputHeader;
	int inputCols, inputRows, inputBytes;
	char Header_1[320];
	unsigned char *inputImage;
	int index = 0;
	//int r,c,r2,c2;
	double p[3][128*128];
	double norm_x[128*128], norm_y[128*128], norm_z[128*128];
	unsigned char *thresholdImg, *threshholdBinary;
	double a[3], b[3];

	printf("Initialization done!\n");
	
	if ((fptr=fopen("chair-range.ppm","r"))==NULL)
	{
		printf("Unable to open input file for reading\n");
		exit(0);
	}
	

	printf("Input Check done!\n");


	//Open and load input image
	fptr = fopen("chair-range.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&inputHeader, &inputCols, &inputRows, &inputBytes);	
	Header_1[0]=fgetc(fptr);	/* read white-space character that separates header */
	inputImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	fread(inputImage, 1, inputCols*inputRows, fptr);
	fclose(fptr);
	printf("Input file opened!\n");

	fptr=fopen("input_image.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(inputImage,inputCols*inputRows,1,fptr);
	fclose(fptr);

	//allocate memory
	thresholdImg = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));
	threshholdBinary = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));

	//threshold the image
	for (int r = 0; r < inputRows; r++)
	{
		for (int c = 0; c < inputCols; c++)
		{
			if (inputImage[r*inputCols+c] < 120)
			{
				thresholdImg[r*inputCols+c] = inputImage[r*inputCols+c];
				threshholdBinary[r*inputCols+c] = 255;
			}
			else
			{
				threshholdBinary[r*inputCols+c] = 0;
			}
		}
	}

	fptr=fopen("threshold_image.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(thresholdImg,inputCols*inputRows,1,fptr);
	fclose(fptr);

	fptr=fopen("threshold_binary.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(threshholdBinary,inputCols*inputRows,1,fptr);
	fclose(fptr);

	OdeticsCoordinates();
	fptr=fopen("chair-range.ppm.coords","r");
	fread(p[0], 8, 128*128, fptr);
	fread(p[1], 8, 128*128, fptr);
	fread(p[2], 8, 128*128, fptr);
	fclose(fptr);


	for (int r = 0; r < inputRows-3; r++)
	{
		for (int c = 0; c < inputCols-3; c++)
		{
			a[0] = p[0][r*inputCols+c+3] - p[0][r*inputCols+c];
			a[1] = p[1][r*inputCols+c+3] - p[1][r*inputCols+c];
			a[2] = p[2][r*inputCols+c+3] - p[2][r*inputCols+c];

			b[0] = p[0][(r+3)*inputCols+c] - p[0][r*inputCols+c];
			b[1] = p[1][(r+3)*inputCols+c] - p[1][r*inputCols+c];
			b[2] = p[2][(r+3)*inputCols+c] - p[2][r*inputCols+c];

			//Cross product
			CrossProduct(a,b,norm_x,norm_y,norm_z,r,inputCols,c);
			printf("norm_x[%d] = %lf\n",(r*inputCols+c), norm_x[r*inputCols+c]);
		}
	}

}