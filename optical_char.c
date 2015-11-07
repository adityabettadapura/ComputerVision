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


void main(int argc, char *argv[])
{
	FILE *fptr, *file1, *file2;
	unsigned char *inputImage, *template, *groundTruth;
	signed int *msfImage, *zeroMeanTemplate;
	unsigned char *binaryImage, *normalizedImage;
	int *flag;
	char *inputHeader, *templateHeader, *gtLetter;
	int inputCols, inputRows, inputBytes;	
	int templateCols, templateRows, templateBytes;
	int sumTemplate = 0, templateMean = 0;
	int i=0,j,k,l,sum;
	int r,c,r2,c2;
	char Header_1[320], Header_2[320];
	char outputChar[2000], character[2000];
	int gtcolumn[2000], gtrow[2000];
	int outputRow[2000], outputCol[2000];
	int TP, FP;
	int m,n,p,index=0,x,y;
	signed int max = 0,min = 0;

	printf("Initialization done!\n");
	
	if ((fptr=fopen("parenthood.ppm","r"))==NULL)
	{
		printf("Unable to open input file for reading\n");
		exit(0);
	}
	
	if ((fptr=fopen("parenthood_e_template.ppm","r"))==NULL)
	{
		printf("Unable to open template file for reading\n");
		exit(0);
	}
	
	printf("Input Check done!\n");

	if ((fptr=fopen("parenthood_gt.txt","r"))==NULL)
	{
		printf("Unable to open gt table for reading\n", argv[3]);
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
	fptr = fopen("parenthood_e_template.ppm", "r");
	fscanf(fptr,"%s %d %d %d",&templateHeader, &templateCols, &templateRows, &templateBytes);	
	Header_2[0]=fgetc(fptr);	/* read white-space character that separates header */
	template = (unsigned char*)calloc(templateCols*templateRows,sizeof(unsigned char));
	fread(template, 1, templateCols*templateRows, fptr);
	fclose(fptr);
	

	//Calculate Zero Mean template
	printf("Calculating template mean!\n");
	while(i < (templateCols*templateRows))
	{
		sumTemplate += template[i];
		i++;
	}

	i = 0;

	while(i < templateCols)
	{
		sumTemplate -= template[i];
		i++;

	}

	templateMean = sumTemplate/((templateRows-1)*templateCols);
	printf("Template mean = %d!\nCalculating Zero mean!\n",templateMean);


	zeroMeanTemplate = (signed int*)calloc(templateCols*templateRows,sizeof(signed int));
	msfImage = (signed int*)calloc(inputCols*inputRows,sizeof(signed int));
	normalizedImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	binaryImage = (unsigned char*)calloc(inputCols*inputRows,sizeof(unsigned char));
	
	for(j = 0; j<(templateRows*templateCols); j++)
	{
		zeroMeanTemplate[j] = template[j] - templateMean;
	}


	//Convolve with zero mean template
	printf("Convolving MSF Image and ZMT!\n");	
	for (r=6; r<inputRows-6; r++)
	{
		for (c=4; c<inputCols-4; c++)
		{
			sum = 0;
			for (r2=-6; r2<=6; r2++)
				for (c2=-4; c2<=4; c2++)
					sum += inputImage[(r+r2)*inputCols+(c+c2)]*zeroMeanTemplate[(r2+7)*templateCols+c2+4];
			msfImage[(r*inputCols)+c]=sum;
		}
	}

	min = msfImage[0];
	max = msfImage[0];
	for(k=1;k<(inputCols*inputRows);k++)
	{
		if (msfImage[k] > max)
		{
			max = msfImage[k];
		}
		if (msfImage[k] < min)
		{
			min = msfImage[k];
		}
	}

	m = 0;
	printf("max = %d min = %d\n",max,min);
	for(l=0;l<(inputCols*inputRows);l++)
	{
		normalizedImage[l] = ((msfImage[l] - min)*255)/(max - min);
		if(normalizedImage[l] > 240)
		{
			binaryImage[l]=255;
			outputCol[m] = l%inputCols;
			outputRow[m] = l/inputCols;
			fptr=fopen("output_gt.txt","a");
			fprintf(fptr,"e %d %d\n",outputCol[m],outputRow[m]);
			fclose(fptr);
			m++;
		}
		else
		{
			binaryImage[l]=0;
		}
	}
	printf("m = %d\n",m);

	 
	printf("Convolution done! Write out MSF Image\n");
	/* write out zero mean template to see result */
	fptr=fopen("zero_mean_template.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",templateCols,templateRows);
	fwrite(zeroMeanTemplate,templateCols*templateRows,1,fptr);
	fclose(fptr);

	//Write out MSF Image file
	fptr=fopen("msf_image.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(msfImage,inputCols*inputRows,1,fptr);
	fclose(fptr);
		
	//Write out MSF Image file
	fptr=fopen("normalized_image.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(normalizedImage,inputCols*inputRows,1,fptr);
	fclose(fptr);
	printf("write out binary image!\n");

	fptr=fopen("binary.ppm","w");
	fprintf(fptr,"P5 %d %d 255\n",inputCols,inputRows);
	fwrite(binaryImage,inputCols*inputRows,1,fptr);
	fclose(fptr);
	
	fptr = fopen("parenthood_gt.txt","r");

	while(fscanf(fptr,"%c %d %d\n",&character[index], &gtcolumn[index], &gtrow[index]) != EOF)
	{
		index++;
	}


	TP = 0;
	FP = 0;
	int index2 = 0;
	n = 0;
	
	while(n < 1262)
	{
		if(character[n] == 'e')
		{
			index2+=1;
			x = gtcolumn[n];
			y = gtrow[n];

			for(p=0;p<m;p++)
			{
			
				if(((outputCol[p] - x >= -2) && (outputCol[p] - x <= 2)) && ((outputRow[p] - y >= -2) && (outputRow[p] - y <= 2)))
				{

					TP += 1;
					break;
				}
									
				
			}
	  	}

	  	n++;
	  }

	  FP = m - TP;

	printf("%d\n",index2);
	printf("TP = %d, FP = %d\n",TP,FP);

}