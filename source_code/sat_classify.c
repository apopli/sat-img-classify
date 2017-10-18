#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "svm.h"

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

struct svm_node *x;                     // x is an array corresponding to 1 data (vector) from the training set,
                                        // it is an array of svm_nodes. An svm_node stores the index-value
                                        // pair of a vector.

int max_nr_attr = 64;

struct svm_model* model;

static char *line = NULL;
static int max_line_len;

/* Reads one line of data (i.e. one vector) from the training file */
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

/* Classifies the training data with the classifier model and computes accuracy, confusion matrix and kappa coefficient */
void predict(FILE *input, FILE *output, FILE * acc)
{
	int correct = 0;
	int total = 0;
	double error = 0;
	double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

	int svm_type=svm_get_svm_type(model);
	int nr_class=svm_get_nr_class(model);
	int j;

	/* Read data (vector) from file line by line and classify using the classifier model */
	
	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	double** confusion_mat = alloc_array(7,7);
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
		fprintf(output,"%g\n",predict_label);
		/* Compute confusion matrix */
		confusion_mat[(int)target_label-1][(int)predict_label-1]++;

		if(predict_label == target_label)
			++correct;
		error += (predict_label-target_label)*(predict_label-target_label);
		sump += predict_label;
		sumt += target_label;
		sumpp += predict_label*predict_label;
		sumtt += target_label*target_label;
		sumpt += predict_label*target_label;
		++total;
	}
	
	/* Write confusion matrix to file */
	int m,n;
	for (m=0; m<7; m++) {
		for (n=0; n<7; n++) {
			fprintf(acc, "%3d ", (int)confusion_mat[m][n]);
		}
		fprintf(acc, "\n");
	}
	
	/* Compute kappa coefficient by first calculating observed agreement and expected agreement */
	
	double expected[7];
	for (m=0; m<7; m++) {
		double colsum = 0, rowsum = 0;
		for (n=0; n<7; n++) {
			colsum += confusion_mat[n][m];
		}
		for (n=0; n<7; n++) {
			rowsum += confusion_mat[m][n];
		}
		expected[m] = colsum*rowsum/total;
	}
	double obs_agr=0;
	for (m=0; m<7; m++) {
		obs_agr += confusion_mat[m][m];
	}
	obs_agr /= total;
	double exp_agr=0;
	for (m=0; m<7; m++) {
		exp_agr += expected[m];
	}
	exp_agr /= total;
	double kappa = (obs_agr-exp_agr)/(1-exp_agr);
	
	info("Accuracy = %g%% (%d/%d)\n",
		(double)correct/total*100,correct,total);
	info("Kappa coefficient = %g\n", kappa);
	
	dealloc_array_fl(confusion_mat,7);
}

int main()
{
	FILE *input, *output, *acc;

	input = fopen("traindata","r");

	output = fopen("traindata.classified","w");
	acc = fopen("confusion","w");

	model=svm_load_model("traindata.model");

	x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));

	/* Classify training data from input, store classified data in output, store confusion mat in acc */

	predict(input,output,acc);
	svm_free_and_destroy_model(&model);
	free(x);
	free(line);
	fclose(input);
	fclose(output);
	fclose(acc);
	return 0;
}
