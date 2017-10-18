#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "svm.h"
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
                *(twoDary + i) = (double*) (calloc(columns,sizeof(double)));
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

static int (*info)(const char *fmt,...) = &printf;

struct svm_node *x;                     // x is an array corresponding to 1 data (vector) from the test dataset,
                                        // it is an array of svm_nodes. An svm_node stores the index-value
                                        // pair of a vector.

int max_nr_attr = 64;
int rows, columns;

struct svm_model* model;

static char *line = NULL;
static int max_line_len;

/* Reads one line of data (i.e. one vector) from the test data file */
static char* readline(FILE *input)
{
	int len;

	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

/* Classifies the test data with the classifier model and generates classified output image */
void predict(FILE *input, FILE *output)
{
	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	int j;

	/* Read data (vector) from file line by line and classify using the classifier model,
	   store labels corresponding to pixels in file testdata.classified, and assign the 
	   specific class color to the corresponding pixel in the output image being generated. */
	
	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	int cc=0;
	double * output_image = (double*) calloc(rows*columns*3,sizeof(double));
	while(readline(input) != NULL)
	{		
		int i = 0;
		double target_label, predict_label;
		char *idx, *val, *label, *endptr;
		int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

		label = strtok(line," \t\n");

		target_label = strtod(label,&endptr);

		while(1)
		{
			if(i>=max_nr_attr-1)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
			}

			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;
			errno = 0;
			x[i].index = (int) strtol(idx,&endptr,10);
			if(!(endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index))
				inst_max_index = x[i].index;

			errno = 0;
			x[i].value = strtod(val,&endptr);
			
			++i;
		}
		x[i].index = -1;

		/* Classify vector x using model */
		predict_label = svm_predict(model,x);
		
		/* Write classified data labels to file */
		if (cc%columns == columns-1)
			fprintf(output,"%g\n",predict_label);
		else
			fprintf(output,"%g ",predict_label);
		
		/* Assign class specific colors to corresponding pixels in output image */
		if (predict_label == 1) {
			/* blue */
			output_image[cc] = 0;
			output_image[cc+rows*columns] = 0;
			output_image[cc+rows*columns*2] = 1;
		} else if (predict_label == 2) {
			/* green */
			output_image[cc] = 0;
			output_image[cc+rows*columns] = 1;
			output_image[cc+rows*columns*2] = 0;
		} else if (predict_label == 3) {
			/* red */
			output_image[cc] = 1;
			output_image[cc+rows*columns] = 0;
			output_image[cc+rows*columns*2] = 0;
		} else if (predict_label == 4) {
			/* cyan */
			output_image[cc] = 0;
			output_image[cc+rows*columns] = 1;
			output_image[cc+rows*columns*2] = 1;
		} else if (predict_label == 5) {
			/* yellow */
			output_image[cc] = 1;
			output_image[cc+rows*columns] = 1;
			output_image[cc+rows*columns*2] = 0;
		} else if (predict_label == 6) {
			/* magenta */
			output_image[cc] = 1;
			output_image[cc+rows*columns] = 0;
			output_image[cc+rows*columns*2] = 1;
		} else if (predict_label == 7) {
			/* black */
			output_image[cc] = 0;
			output_image[cc+rows*columns] = 0;
			output_image[cc+rows*columns*2] = 0;
		}

		if(predict_label == target_label)
			++correct;
		error += (predict_label-target_label)*(predict_label-target_label);
		sump += predict_label;
		sumt += target_label;
		sumpp += predict_label*predict_label;
		sumtt += target_label*target_label;
		sumpt += predict_label*target_label;
		++total;
		
		cc++;
	}
	
	/* Write to output classified image */
	write_image(output_image, columns, rows, "classified.jpg", IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_RGB, 100);
	
}

/* Reads data from test images of 4 bands and writes to file testdata */
void image2data (char** argv, const char * test) {
	double *input_image;
    	const char* imagefile = argv[1];
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);
    	
    	int i;
	int j;
	double** image_red = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_red[i][j] = input_image[i*columns+j];
        	}
        }
        
        imagefile = argv[2];
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);        
    	double** image_green = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_green[i][j] = input_image[i*columns+j];
        	}
        }
        imagefile = argv[3];
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);        
    	double** image_blue = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_blue[i][j] = input_image[i*columns+j];
        	}
        }
        imagefile = argv[4];
    	input_image = (double *)read_image(&columns, &rows, imagefile, IMAGEIO_DOUBLE | IMAGEIO_PLANAR | IMAGEIO_GRAYSCALE);        
    	double** image_infrared = alloc_array(rows,columns);
	for (i=0; i<rows; i++) {
        	for (j=0; j<columns; j++) {
        		image_infrared[i][j] = input_image[i*columns+j];
        	}
        }
    	
    	FILE* myfile;
  	myfile = fopen(test,"w");
	for (i=0; i<rows; i++) {
		for (j=0; j<columns; j++) {
			fprintf(myfile,"0 1:%.16lf 2:%.16lf 3:%.16lf 4:%.16lf\n",image_red[i][j],image_green[i][j],image_blue[i][j],image_infrared[i][j]);
		}
	}
	
	dealloc_array_fl(image_red,rows);
	dealloc_array_fl(image_green,rows);
	dealloc_array_fl(image_blue,rows);
	dealloc_array_fl(image_infrared,rows);
	free(input_image);
	fclose(myfile);
}

int main(int argc, char* argv[])
{
	/* Convert test images to test data */
	image2data(argv,"testdata");	
	
	FILE *input, *output;
	int i;

	input = fopen("testdata","r");                       //test data as input

	output = fopen("testdata.classified","w");           //output classified test data

	model=svm_load_model("traindata.model");             //classifier model to be used   

	x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));

	/* Classify test data */
	predict(input,output);
	svm_free_and_destroy_model(&model);
	free(x);
	free(line);
	fclose(input);
	fclose(output);
	return 0;
}
