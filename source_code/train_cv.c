#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "svm.h"
#define Malloc(type,n) (type *)malloc((n)*sizeof(type))

/* Reads training data from file and stores in structure 'svm_problem' named prob */
void read_problem(const char *filename);

/* Stores cost, gamma and nf parameters in structure 'svm_parameter' named param */
void train_params(double cost, double gamma, int nf);

/* Performs cross validation */
void do_cross_validation();

struct svm_parameter param;

struct svm_problem prob;		// set by read_problem - this structure stores length of dataset,
                                        // array of x_space(s) defined below, and a corresponding array of target classes.

struct svm_model *model;

struct svm_node *x_space;               // x_space is an array corresponding to 1 data (vector) from the training set,
                                        // it is an array of svm_nodes. An svm_node stores the index-value
                                        // pair of a vector.
int cross_validation;
int nr_fold;

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

int main () {
	const char* input_file_name = "traindata";
	const char* model_file_name = "traindata.model";
	
	/* Cross validate with cost=8, gamma=0.5 and 5-fold */
	
	train_params(8,0.5,5);
	read_problem(input_file_name);
	
	do_cross_validation();
	
	svm_destroy_param(&param);
	free(prob.y);
	free(prob.x);
	free(x_space);
	free(line);
	
	return 0;	
}

void do_cross_validation()
{
	int i;
	int total_correct = 0;
	double total_error = 0;
	double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
	double *target = Malloc(double,prob.l);

	svm_cross_validation(&prob,&param,nr_fold,target);
	
	for(i=0;i<prob.l;i++)
		if(target[i] == prob.y[i])
			++total_correct;
	printf("Cross Validation Accuracy = %g%%\n",100.0*total_correct/prob.l);
		
	free(target);
}

void train_params (double cost, double gamma, int nf) {
	param.svm_type = C_SVC;
	param.kernel_type = RBF;
	param.degree = 3;
	param.gamma = gamma;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = cost;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
	cross_validation = 1;
	nr_fold = nf;
}

void read_problem(const char *filename)
{
	int max_index, inst_max_index, i;
	size_t elements, j;
	FILE *fp = fopen(filename,"r");
	char *endptr;
	char *idx, *val, *label;

	prob.l = 0;
	elements = 0;

	max_line_len = 1024;
	line = Malloc(char,max_line_len);
	while(readline(fp)!=NULL)
	{
		char *p = strtok(line," \t"); // label

		// features
		while(1)
		{
			p = strtok(NULL," \t");
			if(p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
				break;
			++elements;
		}
		++elements;
		++prob.l;
	}
	rewind(fp);

	prob.y = Malloc(double,prob.l);
	prob.x = Malloc(struct svm_node *,prob.l);
	x_space = Malloc(struct svm_node,elements);

	max_index = 0;
	j=0;
	for(i=0;i<prob.l;i++)
	{
		inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
		readline(fp);
		prob.x[i] = &x_space[j];
		label = strtok(line," \t\n");
		
		prob.y[i] = strtod(label,&endptr);
		
		while(1)
		{
			idx = strtok(NULL,":");
			val = strtok(NULL," \t");

			if(val == NULL)
				break;

			errno = 0;
			x_space[j].index = (int) strtol(idx,&endptr,10);
			if(!(endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index))
				inst_max_index = x_space[j].index;

			errno = 0;
			x_space[j].value = strtod(val,&endptr);
			
			++j;
		}

		if(inst_max_index > max_index)
			max_index = inst_max_index;
		x_space[j++].index = -1;
	}

	if(param.gamma == 0 && max_index > 0)
		param.gamma = 1.0/max_index;

	fclose(fp);
}
