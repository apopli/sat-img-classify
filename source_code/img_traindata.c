#include <stdlib.h>
#include <stdio.h>
#include "imageio.h"

/* Dynamic memory allocation of 2-D array */
double **alloc_array(int rows, int columns)
{
        int i;
        int j;
        /* Allocate an array of pointers with size equal to number of rows */
        double **twoDary = (double **) (calloc(rows,sizeof(double *)));
        double *currentrow;

	/* For each row, allocate an array with size equal to number of columns */
        for ( i = 0; i < rows; i++ ){
                *(twoDary + i) = (double *) (calloc(columns,sizeof(double)));
        }

	/* Initialize the 2D array with zeros */
        for (j = 0; j < rows; j++) {
                currentrow = *(twoDary + j);
                for ( i = 0; i < columns; i++ ) {
                        *(currentrow + i) = 0;
                }
        }
        return twoDary;
}

/* De-allocation of dynamic 2-D array */
void dealloc_array_fl(double **arr,int m)
{
        int k;
        /* Free memory corresponding to each row */
        for(k=0;k<m;k++)
        {
                free(arr[k]);
        }
        /* Free memory corresponding to the array of pointers to rows */
        free(arr);
}

int main () {
	double *input_image;
    	int columns,rows;
    	
    	/* Given images of 4 bands are read and stored in 2-D arrays */
    	
    	const char* imagefile = "RED_BAND.tif";
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);
    	
    	int i;
	int j;
	double** image_red = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_red[i][j] = input_image[i*columns+j];
        	}
        }
        
        imagefile = "GREEN_BAND.tif";
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);        
    	double** image_green = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_green[i][j] = input_image[i*columns+j];
        	}
        }
        imagefile = "BLUE_BAND.tif";
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);        
    	double** image_blue = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_blue[i][j] = input_image[i*columns+j];
        	}
        }
        imagefile = "INFRARED_BAND.tif";
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);        
    	double** image_infrared = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_infrared[i][j] = input_image[i*columns+j];
        	}
        }
    	
    	/* Data from the 7 training windows of 4 images is written to a file */
    	
    	FILE* myfile;
    	char datafile[30];
  	sprintf(datafile,"traindata");
  	myfile = fopen(datafile,"w");
	for (i=829; i<840; i++) {
		for (j=799; j<810; j++) {
			fprintf(myfile,"1 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	for (i=559; i<570; i++) {
		for (j=759; j<770; j++) {
			fprintf(myfile,"2 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	for (i=824; i<835; i++) {
		for (j=944; j<955; j++) {
			fprintf(myfile,"3 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	for (i=204; i<215; i++) {
		for (j=739; j<750; j++) {
			fprintf(myfile,"4 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	for (i=924; i<935; i++) {
		for (j=299; j<310; j++) {
			fprintf(myfile,"5 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	for (i=344; i<355; i++) {
		for (j=464; j<475; j++) {
			fprintf(myfile,"6 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	for (i=819; i<830; i++) {
		for (j=149; j<160; j++) {
			fprintf(myfile,"7 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	
	dealloc_array_fl(image_red,rows);
	dealloc_array_fl(image_green,rows);
	dealloc_array_fl(image_blue,rows);
	dealloc_array_fl(image_infrared,rows);
	free(input_image);
	fclose(myfile);
	
	return 0;
}
