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
	return transitionCount;
	

}

int FindEdgeTransitionTemplate(int pixel)
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
		for(c=pixelCol-4; c<pixelCol+4; c++)
		{

			currentPixel = binary[(pixelRow-4)*inputCols+c];
			nextPixel = binary[(pixelRow-4)*inputCols+c+4];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}
		c = 0;

		//Row below centre pixel
		for(c=pixelCol+4; c>pixelCol-4; c--)
		{
			currentPixel = binary[(pixelRow+4)*inputCols+c];
			nextPixel = binary[(pixelRow+4)*inputCols+c-4];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}

		//Column right of centre pixel
		for (r=pixelRow-6; r <pixelRow+7; r++)
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
		for (r=pixelRow+7; r>pixelRow-6; r--)
		{
			currentPixel = binary[(r*inputCols)+pixelCol-1];
			nextPixel = binary[((r-1)*inputCols)+pixelCol-1];
			if (currentPixel > nextPixel)
			{
				transitionCount++;
			}
		}
	}
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

		//Column left of centre pixel
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
	unsigned char *thinImage, *duplicateImage;
	int msfCols, msfRows, msfBytes;
	char *inputHeader, *msfHeader, *binaryHeader;
	char Header_1[320], Header_2[320], Header_3[320];
	int i = 0, j = 0, k = 0, l = 0, m = 0, n = 0;
	int edgeTransCount = 0, edgeNeighbours = 0, nonEdgeNeighbour = 0;
	int threshold;
	int r1, r2, c1, c2;
	int *endPoint, *branchPoint, *numBranchPoints, *numEndPoints;
	int z=0;
	int index = 0;
	char character[2000];
	int gtCol[2000], gtRow[2000];
	int TP, FP, FN, d;
	int a,b,x,y;
	int *endPointCol, *endPointRow, *branchPointCol, *branchPointRow;
	int branchcol, branchrow, endcol, endrow;
	int outputRow[2000], outputCol[2000];
	int count, centrePixel;


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
		printf("Unable to open gt table for reading\n");
		exit(0);
	}
	
	//Open and load input image
	fptr = fopen("parenthood.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&inputHeader, &inputCols, &inputRows, &inputBytes);	
	Header_1[0]=fgetc(fptr);	/* read white-space character that separates header */
	inputImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	fread(inputImage, 1, inputCols*inputRows, fptr);
	fclose(fptr);
	printf("Input file opened!\n");

	//Open and load template
	fptr = fopen("msf_e.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&msfHeader, &msfCols, &msfRows, &msfBytes);	
	Header_2[0]=fgetc(fptr);	/* read white-space character that separates header */
	msfImage = (unsigned char*)calloc(msfCols*msfRows,sizeof(unsigned char));
	fread(msfImage, 1, msfCols*msfRows, fptr);
	fclose(fptr);
	printf("msf image opened\n");

	binaryImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	thinImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	duplicateImage = (unsigned char*)calloc(msfCols*msfRows,sizeof(unsigned char));
	for(int o =0;o<(inputCols*inputRows);o++)
	{
		duplicateImage[o] = 255;
	}
	

	for (int l = 0; l < (msfCols*msfRows); l++)
	{
		if (msfImage[l] >= 220)
		{
			if(l>=58)
			{
				r1 = (l/msfCols)-6;
				c1 = (l%msfCols)-4;
				r2 = (l/msfCols)+7;
				c2 = (l%msfCols)+4;

				for(m = r1; m<=r2; m++)
				{
					for (n = c1; n <= c2; n++)
					{
						duplicateImage[(m*inputCols)+n] = inputImage[(m*inputCols)+n];
					}
				}

			}
		}
	}

	fptr=fopen("duplicateImage.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(duplicateImage,inputCols*inputRows,1,fptr);
	fclose(fptr);
	printf("Duplicate image created\n");

	while (i < (inputCols*inputRows))
	{
		if (duplicateImage[i] < 128)
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

	//Write out binary Image file
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
	printf("binary image opened\n");


	//apply thinning process on binary image
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
				}
			}
		}
		j++;
	}

	//Write out thin Image file
	for (int k = 0; k < inputCols*inputRows; k++)
	{
		if (thinImage[k]==255)
		{
			binary[k] = 0;
			thinImage[k]=0;
		}
	}

	fptr=fopen("thinned.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(binary,inputCols*inputRows,1,fptr);
	fclose(fptr);
	printf("Binary image created\n");

	//Open and load binary
	fptr = fopen("thinned.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&binaryHeader, &binaryCols, &binaryRows, &binaryBytes);	
	Header_3[0]=fgetc(fptr);	/* read white-space character that separates header */
	binary = (unsigned char*)calloc(msfCols*msfRows,sizeof(unsigned char));
	fread(binary, 1, binaryCols*binaryRows, fptr);
	fclose(fptr);
	printf("msf image opened\n");

	//Second iteration
	endPoint = (int*)calloc(inputCols*inputRows,sizeof(int));
	branchPoint = (int*)calloc(inputCols*inputRows,sizeof(int));
	endPointCol = (int*)calloc(inputCols*inputRows,sizeof(int));
	endPointRow = (int*)calloc(inputCols*inputRows,sizeof(int));
	branchPointCol = (int*)calloc(inputCols*inputRows,sizeof(int));
	branchPointRow = (int*)calloc(inputCols*inputRows,sizeof(int));

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
				}
			}
			endPoint[j] = 0;
			endPointCol[j] = j%inputCols;
			endPointRow[j] = j/inputCols;
			branchPoint[j] = 255;
			x++;
		}
		else
		{
			endPoint[j] = 255;
			branchPoint[j] = 0;
			branchPointCol[j] = j%inputCols;
			branchPointRow[j] = j/inputCols;
			y++;
		}
		j++;
	}

	for (int k = 0; k < inputCols*inputRows; k++)
	{
		if (thinImage[k]==255)
		{
			binary[k] = 0;
			thinImage[k]=0;
		}
	}

	fptr=fopen("thinned.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(binary,inputCols*inputRows,1,fptr);
	fclose(fptr);
	printf("Binary image created\n");


	fptr = fopen("parenthood_gt.txt","r");
	while(fscanf(fptr,"%c %d %d\n",&character[index], &gtCol[index], &gtRow[index]) != EOF)
	{
		index++;
	}
	printf("index = %d\n",index);

	int branchpoint, endpoint;

	for (int i = 0; i < index; i++)
	{
		centrePixel = (gtRow[i]*inputCols)+gtCol[i];
		count = FindEdgeTransitionTemplate(centrePixel);
		printf("%c edge transition = %d\n",character[i],count);
		if (character[i] == 'e')
		{
			if(count >= 6)
				TP++;
			if(count == 0)
				FN++;

		}
		else
		{
			if(count>=5)
				FP++;
		}
	}


	printf("TP = %d FP = %d FN = %d\n",TP,FP,FN);



}