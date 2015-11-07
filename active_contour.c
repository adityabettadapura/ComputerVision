/*
* Author: Aditya Bettadapura
*
* This program reads an image and takes contour points from a text file.
* Using an iterative process, it draws the controur around the desired segment.
*
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define SQUARE(x) ((x)*(x))


void DrawPlus(unsigned char initialContour[], int colnum, int rownum, int totalCols, int colour)
{
	for (int y = rownum-3; y<=rownum+3; y++)
	{
		initialContour[(y*totalCols)+colnum] = colour;

	}

	for (int x = colnum-3; x<=colnum+3; x++)
	{
		initialContour[(rownum*totalCols)+x] = colour;

	}
}


double CalculateCellDistance(double distanceImage[], int curCol, int curRow, int nextCol, int nextRow, int inputCols)
{
	int mean = 0;

	for (int y = -3; y <=3; y++)
	{
		for (int x = -3; x <=3; x++)
		{
			distanceImage[(curRow+y)*inputCols+curCol+x] = SQUARE(nextCol - curCol-x) + SQUARE(nextRow - curRow-y);
			mean+= sqrt(distanceImage[(curRow+y)*inputCols+curCol+x]);
		}
	}
	return (mean/49);

}


void CalculateCellVariance(double varianceImage[], double distanceImage[], double variance, int curCol, int curRow, int nextCol, int nextRow, int inputCols)
{
	int mean = 0;

	for (int y = -3; y <=3; y++)
	{
		for (int x = -3; x <=3; x++)
		{
			varianceImage[(curRow+y)*inputCols+curCol+x] = SQUARE(sqrt(distanceImage[(curRow+y)*inputCols+curCol+x]) - variance);
		}
	}
}


void Normalize(double input[], int inputCols, int inputRows, int curCol, int curRow, double output[], int level)
{
	double minGrad = input[(curRow*inputCols)+curCol];
	double maxGrad = input[(curRow*inputCols)+curCol];

	for (int y = -3; y <=3; y++)
	{
		for (int x = -3; x <=3; x++)
		{
			if (input[(curRow+y)*inputCols+curCol+x] > maxGrad)
			{
				maxGrad = input[(curRow+y)*inputCols+curCol+x];
			}
			else if (input[(curRow+y)*inputCols+curCol+x] < minGrad)
			{
				minGrad = input[(curRow+y)*inputCols+curCol+x];
			}

		}
	}


	for (int y = -3; y <=3; y++)
	{
		for (int x = -3; x <=3; x++)
		{
			output[(curRow+y)*inputCols+curCol+x] = (input[(curRow+y)*inputCols+curCol+x] - minGrad)*level/(maxGrad-minGrad);

		}
	}

}


void NormalizeGray(double input[], int inputCols, int inputRows, unsigned char output[], int level)
{
	double min = 0;
	double max = 0;

	for (int i = 0; i < inputCols*inputRows; i++)
	{
		if (input[i] > max)
		{
			max = input[i];
		}
		else if (input[i] < min)
		{
			min = input[i];
		}
	}


	for (int i = 0; i < inputCols*inputRows; i++)
	{
		output[i] = (input[i] - min)*level/(max-min);

	}

}


void main(int argc, char *argv[])
{

	FILE *fptr;
	char *inputHeader;
	int inputCols, inputRows, inputBytes;
	char Header_1[320];
	unsigned char *inputImage, *initialContour;
	int initCol[42], initRow[42];
	int index = 0;
	int contourSqrDist[42];
	double mean = 0;
	double variance[42];
	unsigned char *normalGrad, *normalGrad_x, *normalGrad_y;
	double *gradXImage, *gradYImage, *grad;
	int sumX, sumY;
	int r,c,r2,c2;
	double *energyImage;
	unsigned char *finalContour;
	int newPosition;
	double cellAverage[42];
	double *varianceImage, *distanceImage;
	double *normalVariance, *normalDistance, *normalSobel;

	printf("Initialization done!\n");
	
	if ((fptr=fopen("hawk.ppm","r"))==NULL)
	{
		printf("Unable to open input file for reading\n");
		exit(0);
	}
	

	if ((fptr=fopen("hawk_init.txt","r"))==NULL)
	{
		printf("Unable to open contour table for reading\n");
		exit(0);
	}

	printf("Input Check done!\n");


	//Open and load input image
	fptr = fopen("hawk.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&inputHeader, &inputCols, &inputRows, &inputBytes);	
	Header_1[0]=fgetc(fptr);	/* read white-space character that separates header */
	inputImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	fread(inputImage, 1, inputCols*inputRows, fptr);
	fclose(fptr);
	printf("Input file opened!\n");


	//Open and load initial contour points
	fptr = fopen("hawk_init.txt","r");
	while(fscanf(fptr,"%d %d\n", &initCol[index], &initRow[index]) != EOF)
	{
		index++;
	}
	fclose(fptr);
	for (int i = 0; i < 42; i++)
	{
		printf("%d. %d %d \n",i+1, initCol[i], initRow[i]);
	}
	printf("Initial contour points loaded!\n");


	//Copy input image into initialcontour
	initialContour = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));
	for (int i = 0; i < inputCols*inputRows; i++)
	{
		initialContour[i] = inputImage[i];
	}


	//Draw plus at contour points
	for (int i = 0; i < 42; i++)
	{
		DrawPlus(initialContour, initCol[i], initRow[i], inputCols, 0);
	}
	//Write out initial contour image
	fptr=fopen("initial_contour.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(initialContour,inputCols*inputRows,1,fptr);
	fclose(fptr);

	//Allocate memory to all images
	distanceImage = (double*)calloc(inputCols*inputRows, sizeof(double));
	gradXImage = (double*)calloc(inputCols*inputRows, sizeof(double));
	gradYImage = (double*)calloc(inputCols*inputRows, sizeof(double));
	grad = (double*)calloc(inputCols*inputRows, sizeof(double));
	energyImage = (double*)calloc(inputCols*inputRows, sizeof(double));
	finalContour = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));
	normalGrad = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));
	normalGrad_x = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));
	normalGrad_y = (unsigned char*)calloc(inputCols*inputRows, sizeof(unsigned char));
	varianceImage = (double*)calloc(inputCols*inputRows, sizeof(double));
	normalVariance = (double*)calloc(inputCols*inputRows, sizeof(double));
	normalDistance = (double*)calloc(inputCols*inputRows, sizeof(double));
	normalSobel = (double*)calloc(inputCols*inputRows, sizeof(double));

	//Sobel filter kernels of order 3x3
	int sobelX[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
	int sobelY[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};


	//Compute external energy term
	for (r = 0; r < inputRows; r++)
	{
		for (c = 0; c < inputCols; c++)
		{
			sumX = 0;
			sumY = 0;
			for (r2=-1; r2<=1; r2++)
				for (c2=-1; c2<=1; c2++)
				{	
					sumX += inputImage[(r+r2)*inputCols+(c+c2)]*sobelX[(r2+1)*3+c2+1];
					sumY += inputImage[(r+r2)*inputCols+(c+c2)]*sobelY[(r2+1)*3+c2+1];
				}
			gradXImage[(r*inputCols)+c]=sumX;
			gradYImage[(r*inputCols)+c]=sumY;
			grad[(r*inputCols)+c] = (SQUARE(sumX) + SQUARE(sumY));
		}
	}

	NormalizeGray(gradXImage, inputCols, inputRows, normalGrad_x, 255);
	NormalizeGray(gradYImage, inputCols, inputRows, normalGrad_y, 255);
	NormalizeGray(grad, inputCols, inputRows, normalGrad, 255);


	//Calculate Internal energies for 'n' iterations
	int iteration = 10;
	while(iteration>0)
	{

		for (int i = 0; i < 42; i++)
		{
			int next_col = (i+1)%42;
			int next_row = (i+1)%42;
			
			//Interal energy 1
			cellAverage[i] = CalculateCellDistance(distanceImage, initCol[i], initRow[i], initCol[next_col], initRow[next_row], inputCols);

			//Interal energy 2
			CalculateCellVariance(varianceImage, distanceImage, cellAverage[i], initCol[i], initRow[i], initCol[next_col], initRow[next_row], inputCols);

		}

		//Normalize the energies to 0-1 level
		for (int i = 0; i < 42; i++)
		{
			Normalize(varianceImage, inputCols, inputRows, initCol[i], initRow[i], normalVariance,1);
			Normalize(distanceImage, inputCols, inputRows, initCol[i], initRow[i], normalDistance,1);
			Normalize(grad, inputCols, inputRows, initCol[i], initRow[i], normalSobel,1);
		}

		for (int i = 0; i < inputCols*inputRows; i++)
		{
			energyImage[i] = (normalDistance[i] + normalVariance[i] - normalSobel[i])/3;
			finalContour[i] = inputImage[i];
		}

		
		//Move contour point to position of minimum energy 
		for (int i = 0; i < 42; i++)
		{

			int curRow = initRow[i];
			int curCol = initCol[i];

			int minEnergy = energyImage[curRow*inputCols+curCol];
			int newCol, newRow;

			for (int y = -3; y <=3; y++)
			{
				for (int x = -3; x <=3; x++)
				{
					if (energyImage[(curRow+y)*inputCols+curCol+x] < minEnergy)
					{
						newCol = curCol+x;
						newRow = curRow+y;
					}

				}
			}

			DrawPlus(finalContour, newCol, newRow, inputCols, 0);
			initRow[i] = newRow;
			initCol[i] = newCol; 

		}

		iteration--;
		printf("iterations = %d\n",iteration);
	}

	//Print final positions
	fptr=fopen("output.txt","w");
	for (int i = 0; i < 42; i++)
	{
		fprintf(fptr, "%d   %d\n", initCol[i], initRow[i]);
		printf("%d %d\n", initCol[i], initRow[i]);
	}
	fclose(fptr);

	//Write out the output images
	fptr=fopen("normalized_gradient_x.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(normalGrad_x,inputCols*inputRows,1,fptr);
	fclose(fptr);

	fptr=fopen("normalized_gradient_y.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(normalGrad_y,inputCols*inputRows,1,fptr);
	fclose(fptr);

	fptr=fopen("normalized_gradient.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(normalGrad,inputCols*inputRows,1,fptr);
	fclose(fptr);

	fptr=fopen("final_contour.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(finalContour,inputCols*inputRows,1,fptr);
	fclose(fptr);

	//Free allocated memory
	free(initialContour);
	free(distanceImage);
	free(gradXImage);
	free(gradYImage);
	free(grad);
	free(energyImage);
	free(finalContour); 
	free(normalGrad);
	free(normalGrad_x);
	free(normalGrad_y);
	free(varianceImage);
	free(normalVariance);
	free(normalDistance);
	free(normalSobel);
}