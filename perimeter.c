/*
* Author: Aditya Bettadapura
*
* This program reads parenthood.ppm, a 649 x 567 PPM image.
* Using a template of letter e in parenthood_e_template.ppm,
* This program does optical character recognition to get
* positional values for the template letter.
*
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define FAIL 0
#define PASS 1

unsigned char *inputImage, *msfImage, *binaryImage, *binary;
int inputCols, inputRows, inputBytes;
int binaryCols, binaryRows, binaryBytes;	

int FindEdgeTransition(int pixel)
{
	int n = 0;
	int pixelCol, pixelRow;
	int transitionCount = 0;
	int r=0,c=0;
	int currentPixel,nextPixel;

	pixelCol = pixel % inputCols;
	pixelRow = pixel/inputCols;


	if ((pixelRow < 1) || (pixelRow >= inputRows))
	{
		/*Do nothing*/
	}
	else if ((pixelCol < 1) || (pixelCol >= inputCols))
	{
		/* Do nothing */
	}
	else
	{
		//Row above centre pixel
		for(c=pixelCol-1; c<pixelCol+1; c++)
		{

			currentPixel = binary[(pixelRow-1)*inputCols+c];
			nextPixel = binary[(pixelRow-1)*inputCols+c+1];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}
		c = 0;

		//Row below centre pixel
		for(c=pixelCol+1; c>pixelCol-1; c--)
		{
			currentPixel = binary[(pixelRow+1)*inputCols+c];
			nextPixel = binary[(pixelRow+1)*inputCols+c-1];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}

		//Column right of centre pixel
		for (r=pixelRow-1; r <pixelRow+1; r++)
		{
			currentPixel = binary[(r*inputCols)+pixelCol+1];
			nextPixel = binary[((r+1)*inputCols)+pixelCol+1];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}
		r = 0;

		//Column to the left of the centre pixel
		for (r=pixelRow+1; r>pixelRow-1; r--)
		{
			currentPixel = binary[(r*inputCols)+pixelCol-1];
			nextPixel = binary[((r-1)*inputCols)+pixelCol-1];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}
	}
//	printf("transition count = %d\n",transitionCount );
	return transitionCount;
	

}

int FindEdgeNeighbours(int pixel)
{
	int n = 0;
	int pixelCol, pixelRow;
	int edgeCount = 0;
	int r=0,c=0;
	int centrePixel, currentPixel;

	pixelCol = pixel % inputCols;
	pixelRow = pixel/inputCols;
	centrePixel = binary[pixel];


	if ((pixelRow < 1) || (pixelRow >= inputRows))
	{
		/*Do nothing*/
	}
	else if ((pixelCol < 1) || (pixelCol >= inputCols))
	{
		/* Do nothing */
	}
	else
	{
		//Row above centre pixel
		for(c=pixelCol-1; c<=pixelCol+1; c++)
		{
			currentPixel = binary[(pixelRow-1)*inputCols+c];
			if (currentPixel == centrePixel)
			{
				edgeCount++;
			}
		}
		c = 0;

		//Row below centre pixel
		for(c=pixelCol+1; c>=pixelCol-1; c--)
		{
			currentPixel = binary[(pixelRow+1)*inputCols+c];
			if (currentPixel== centrePixel)
			{
				edgeCount++;
			}
		}

		//Column right of centre pixel
			currentPixel = binary[pixel+1];
			if (currentPixel == centrePixel)
			{
				edgeCount++;
			}
	
			currentPixel = binary[pixel-1];
			if (currentPixel == centrePixel)
			{
				edgeCount++;
			}
	}
	return edgeCount;
}

int FindNonEdgeNeighbour(int pixel)
{
	int n = 0;
	int pixelCol, pixelRow;
	int northPixel, southPixel, eastPixel, westPixel;
	int r,c;
	int centrePixel;

	pixelCol = pixel % inputCols;
	pixelRow = pixel/inputCols;
	centrePixel = binary[pixel];

	northPixel = binary[(pixelRow-1)*inputCols+pixelCol];
	southPixel = binary[(pixelRow+1)*inputCols+pixelCol];
	eastPixel = binary[(pixelRow*inputCols)+pixelCol+1];
	westPixel = binary[(pixelRow*inputCols)+pixelCol-1];

	if((northPixel != centrePixel)||(eastPixel != centrePixel)||((westPixel != centrePixel)&&(southPixel != centrePixel)))
	{
		return PASS;
	}
	else
	{
		return FAIL;
	}
}

void main(int argc, char *argv[])
{
	FILE *fptr;
	unsigned char *binaryImage, *thinImage;
	int msfCols, msfRows, msfBytes;
	char *inputHeader, *msfHeader, *binaryHeader;
	char Header_1[320], Header_2[320], Header_3[320];
	int i = 0, j = 0, k = 0;
	int edgeTransCount = 0, edgeNeighbours = 0, nonEdgeNeighbour = 0;
	int eraseCount = 0;

	printf("Initialization done!\n");
	
	if ((fptr=fopen("banana.pgm","r"))==NULL)
	{
		printf("Unable to open input file for reading\n");
		exit(0);
	}
	
	if ((fptr=fopen("banana.pgm","r"))==NULL)
	{
		printf("Unable to open msf image file for reading\n");
		exit(0);
	}
	
	printf("Input Check done!\n");

	if ((fptr=fopen("parenthood_gt.txt","r"))==NULL)
	{
		printf("Unable to open gt table for reading\n", argv[3]);
		exit(0);
	}
	
	//Open and load input image
	fptr = fopen("banana.pgm", "r");
	fscanf(fptr,"%s %d %d %d",&inputHeader, &inputCols, &inputRows, &inputBytes);	
	Header_1[0]=fgetc(fptr);	/* read white-space character that separates header */
	inputImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	fread(inputImage, 1, inputCols*inputRows, fptr);
	fclose(fptr);
	printf("Input file opened!\n");

	//Open and load template
	fptr = fopen("banana.pgm", "r");
	fscanf(fptr,"%s %d %d %d",&msfHeader, &msfCols, &msfRows, &msfBytes);	
	Header_2[0]=fgetc(fptr);	/* read white-space character that separates header */
	msfImage = (unsigned char*)calloc(msfCols*msfRows,sizeof(unsigned char));
	fread(msfImage, 1, msfCols*msfRows, fptr);
	fclose(fptr);
	printf("msf image opened\n");

	binaryImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	thinImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	

	while (i < (inputCols*inputRows))
	{
		if (inputImage[i] > 128)
		{
			binaryImage[i] = 255;
			thinImage[i] = 0;
		}
		else
		{
			binaryImage[i] = 0;
			thinImage[i] = 0;
		}

		i++;
	}

	//Write out MSF Image file
	fptr=fopen("new_binary.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(binaryImage,inputCols*inputRows,1,fptr);
	fclose(fptr);
	printf("Binary image created\n");

	//Open and load binary
	fptr = fopen("new_binary.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&binaryHeader, &binaryCols, &binaryRows, &binaryBytes);	
	Header_3[0]=fgetc(fptr);	/* read white-space character that separates header */
	binary = (unsigned char*)calloc(msfCols*msfRows,sizeof(unsigned char));
	fread(binary, 1, binaryCols*binaryRows, fptr);
	fclose(fptr);
	printf("msf image opened\n");

	j = inputCols+1;
	while(j <(inputCols*inputRows-inputCols-1))
	{
		
		edgeTransCount = FindEdgeTransition(j);
		if (edgeTransCount == 1)
		{
			edgeNeighbours = FindEdgeNeighbours(j);
			if ((edgeNeighbours>=3) && (edgeNeighbours<=7))
			{
				nonEdgeNeighbour = FindNonEdgeNeighbour(j);
				if (nonEdgeNeighbour==PASS)
				{
					thinImage[j] = 255;
					eraseCount++;
				}
			}
		}
		j++;
	}
	
	//Write out MSF Image file
	fptr=fopen("thin_image.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(thinImage,inputCols*inputRows,1,fptr);
	fclose(fptr);
	printf("Thin image created!\n");
	
}