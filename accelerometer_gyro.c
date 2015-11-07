/*
* Author: Aditya Bettadapura
*
* This program reads an iPhone accelerometer and gyroscope data from a text file.
* Using the data, it calculates the distance moved along x,y,z axes and rotation along those axes.
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>

#define SQUARE(x) ((x)*(x))


double ComputeMean(int end, double array[], int size)
{
	double mean = 0;

	for (int i = end*size; (i < (end+1)*size) && (i < 1205); i++)
	{
		mean += array[i];
	}
	return (mean/size);
}

double ComputeVariance(int end, double array[], double meanValue, int size)
{
	double var = 0;
	for (int i = end*size; (i < (end+1)*size) && (i < 1205); i++)
	{
		var += SQUARE(array[i] - meanValue);
	}
	return (var/size);
}

double CalculateRotation(int end, double array[], int size)
{
	double angle=0;

	for (int i = end*size; (i < (end+1)*size) && (i < 1205); i++)
	{
		angle += (array[i]);
	}
	printf("rotation = %lf\n", angle/15);
	return (angle/15);
	
}

double CalculateDistance(int end, double velocity[], double acceleration[], int size)
{
	double sum=0;
	velocity[end*size-1] = 0;
	double avg = 0;

	for (int i = end*size; (i < (end+1)*size) && (i < 1205); i++)
	{
		velocity[i] = velocity[i-1] + acceleration[i]*9.8/15;
		avg = (velocity[i]+velocity[i-1])/2;
		sum += avg/size;
	}
	return sum;
}

void main(int argc, char *argv[])
{

	FILE *fptr;
	double timer[1205], acc_x[1205], acc_y[1205], acc_z[1205], yaw[1205], pitch[1205], roll[1205];
	double *mean_x, *mean_y, *mean_z, *mean_yaw, *mean_pitch, *mean_roll;
	double *variance_x, *variance_y, *variance_z, *variance_yaw, *variance_pitch, *variance_roll;

	double *velocity_x, *average_velocity_x, *distance_x, *cumulative_x;
	double *velocity_y, *average_velocity_y, *distance_y, *cumulative_y;
	double *velocity_z, *average_velocity_z, *distance_z, *cumulative_z;

	double *rotation_yaw, *rotation_pitch, *rotation_roll;
	double *angle_yaw, *mean_angle_yaw, *variance_angle_yaw;
	
	int index = 0;
	int window = 15;

	mean_x = (double*)calloc(1205/window, sizeof(double));
	mean_y = (double*)calloc(1205/window, sizeof(double));
	mean_z = (double*)calloc(1205/window, sizeof(double));
	mean_yaw = (double*)calloc(1205/window, sizeof(double));
	mean_pitch = (double*)calloc(1205/window, sizeof(double));
	mean_roll = (double*)calloc(1205/window, sizeof(double));

	variance_x = (double*)calloc(1205/window, sizeof(double));
	variance_y = (double*)calloc(1205/window, sizeof(double));
	variance_z = (double*)calloc(1205/window, sizeof(double));
	variance_yaw = (double*)calloc(1205/window, sizeof(double));
	variance_pitch = (double*)calloc(1205/window, sizeof(double));
	variance_roll = (double*)calloc(1205/window, sizeof(double));

	rotation_yaw = (double*)calloc(1205/window, sizeof(double));
	rotation_pitch = (double*)calloc(1205/window, sizeof(double));
	rotation_roll = (double*)calloc(1205/window, sizeof(double));
	angle_yaw = (double*)calloc(1205, sizeof(double));
	mean_angle_yaw = (double*)calloc(1205/window, sizeof(double));
	variance_angle_yaw = (double*)calloc(1205/window, sizeof(double));

	velocity_x = (double*)calloc(1205, sizeof(double));
	average_velocity_x = (double*)calloc(1205, sizeof(double));
	distance_x = (double*)calloc(1205, sizeof(double));
	cumulative_x = (double*)calloc(1205, sizeof(double));

	velocity_y = (double*)calloc(1205, sizeof(double));
	average_velocity_y = (double*)calloc(1205, sizeof(double));
	distance_y = (double*)calloc(1205, sizeof(double));
	cumulative_y = (double*)calloc(1205, sizeof(double));

	velocity_z = (double*)calloc(1205, sizeof(double));
	average_velocity_z = (double*)calloc(1205, sizeof(double));
	distance_z = (double*)calloc(1205, sizeof(double));
	cumulative_z = (double*)calloc(1205, sizeof(double));
	
	printf("Initialization done!\n");
	

	if ((fptr=fopen("acc_gyro.txt","r"))==NULL)
	{
		printf("Unable to open accelerometer gyroscope table for reading\n");
		exit(0);
	}

	printf("Input Check done!\n");

	//Open and load iPhone data
	fptr = fopen("acc_gyro.txt","r");
	while(fscanf(fptr,"%lf %lf %lf %lf %lf %lf %lf\n", &timer[index], &acc_x[index], &acc_y[index], &acc_z[index], &yaw[index], &pitch[index], &roll[index]) != EOF)
	{
		index++;
	}
	fclose(fptr);

	velocity_x[0] = 0;
	velocity_y[0] = 0;
	velocity_z[0] = 0;
	average_velocity_x[0] = 0;
	average_velocity_y[0] = 0;
	average_velocity_z[0] = 0;
	distance_x[0] = 0;
	distance_y[0] = 0;
	distance_z[0] = 0;

	for (int i = 0; i < 1205/window; i++)
	{
		//printf("i = %d\n", i);
		mean_x[i] = ComputeMean(i, acc_x, window);
		mean_y[i] = ComputeMean(i, acc_y, window);
		mean_z[i] = ComputeMean(i, acc_z, window);
		mean_yaw[i] = ComputeMean(i, yaw, window);
		mean_pitch[i] = ComputeMean(i, pitch, window);
		mean_roll[i] = ComputeMean(i, roll, window);

		variance_x[i] = ComputeVariance(i, acc_x, mean_x[i], window);
		variance_y[i] = ComputeVariance(i, acc_y, mean_y[i], window);
		variance_z[i] = ComputeVariance(i, acc_z, mean_z[i], window);
		variance_yaw[i] = ComputeVariance(i, yaw, mean_yaw[i], window);
		variance_pitch[i] = ComputeVariance(i, pitch, mean_pitch[i], window);
		variance_roll[i] = ComputeVariance(i, roll, mean_roll[i], window);

		double sum_dist = 0;
		if (fabs(variance_x[i]) > 0.035)
		{
			sum_dist = CalculateDistance(i, velocity_x, acc_x, window);
			printf("time = %lf\n",timer[i*window]);
			printf("distance moved in m = %lf\n", sum_dist);
		}

		if (variance_y[i] > 0.045)
		{
			sum_dist = CalculateDistance(i, velocity_y, acc_y, window);
			printf("time = %lf\n",timer[i*window]);
			printf("distance moved in m = %lf\n", sum_dist);
		}

		if (variance_z[i] > 0.024)
		{
			sum_dist = CalculateDistance(i, velocity_z, acc_z, window);
			printf("time = %lf\n",timer[i*window]);
			printf("distance moved in m = %lf\n", sum_dist);
		}

		if (fabs(mean_yaw[i]) > 11)
		{
			printf("time = %lf\n",timer[i*window]);
			rotation_yaw[i] = CalculateRotation(i, yaw, window);

		}

		if (abs(mean_pitch[i]) > 23)
		{
			printf("time = %lf\n",timer[i*window]);
			rotation_pitch[i] = CalculateRotation(i, pitch, window);
		}

		if (abs(mean_roll[i]) > 15)
		{
			printf("time = %lf\n",timer[i*window]);
			rotation_roll[i] = CalculateRotation(i, roll, window);
		}

	}	

}