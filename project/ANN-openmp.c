#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

struct _ANN{
    int size;
    int size_layer2;
    int size_layer3;
    double **weights_into_hidden;
    double **weights_outof_hidden;
    double **d_weights_into_hidden;
    double **d_weights_outof_hidden;
    double eta;
    double alpha;
    double **cases;
    double **case_features;
};


typedef struct _ANN ANN_t;

double sigmoid(double x){
  double result;
  result=1.0/(1.0 + exp(-x));
  return result;
}

double dsigmoid(double x){
  double result;
  result=x * (1.0 - x) ;
  return result;
}
//needs to malloc in coming and outputs
void ANN_init(ANN_t* ANN, int size, int size_layer2, int size_layer3){
  ANN->size =size;
  ANN->size_layer2=size_layer2;
  ANN->size_layer3=size_layer3;
  ANN->cases =(double*)malloc(sizeof(double*)*683);
  for (int m=0; m < 683; m++){
    ANN->cases[m]= (double*)malloc(sizeof(double*)*size_layer3);
  }
  ANN->case_features =(double*)malloc(sizeof(double*)*683);
  for (int n=0; n < 683; n++){
    ANN->case_features[n]= (double*)malloc(sizeof(double*)*10);
  }
  ANN->alpha= 0.01;
  ANN->eta = 0.05;

}//end of init

void initialize_weights(ANN_t* ANN, int size, int size_layer2, int size_layer3){

  //malloc weights and delta weights between first and hidden layer
  ANN->weights_into_hidden=(double*)malloc(sizeof(double*)*size);
  ANN->d_weights_into_hidden=(double*)malloc(sizeof(double*)*size);
  for (int n=0; n < size; n++){
    ANN->weights_into_hidden[n]= (double*)malloc(sizeof(double*)*size_layer2);
    ANN->d_weights_into_hidden[n]= (double*)malloc(sizeof(double*)*size_layer2);
  }
  //malloc weights and delta weights between hidden and output layer
  ANN->weights_outof_hidden=(double*)malloc(sizeof(double*)*size_layer2);
  ANN->d_weights_outof_hidden=(double*)malloc(sizeof(double*)*size_layer2);
  for (int n=0; n < size_layer2; n++){
    ANN->weights_outof_hidden[n]= (double*)malloc(sizeof(double*)*size_layer3);
    ANN->d_weights_outof_hidden[n]= (double*)malloc(sizeof(double*)*size_layer3);
  }

  //initialized the weights between the input and hidden layers
  for (int i=0;i<size; i++){
    for (int j=0; j < size_layer2;j++){
        (ANN->weights_into_hidden)[i][j]=(double) rand() / RAND_MAX;
        (ANN->d_weights_into_hidden)[i][j]=0.0;

    }
  }

  //initialized the weights between the hidden and output layers
  for (int i=0;i<size_layer2; i++){
    for (int j=0; j < size_layer3;j++){
        (ANN->weights_outof_hidden)[i][j]=(double) rand() / RAND_MAX;
        (ANN->d_weights_outof_hidden)[i][j]=0.0;

    }
  }


}//end of initialize weights

//run the whole neural network, where most of the calculations are
void run(ANN_t* ANN,int iterations){
  int data_inputs = 683;
  double sum_layer2[data_inputs][ANN->size_layer2];
  double sum_layer2_back[ANN->size_layer2];
  double sum_layer3[data_inputs][ANN->size_layer3];
  double hidden_layer[data_inputs][ANN->size_layer2];
  double output_layer[data_inputs][ANN->size_layer3];
  double d_hidden_layer[ANN->size_layer2];
  double d_output_layer[ANN->size_layer3];
  #pragma omp parallel for num_threads(8)
  for( int epoch = 0 ; epoch < iterations ; epoch++) {
    double error=0.0;

    //run it as many times as the amount of inputs we have from out data
    #pragma omp parallel for num_threads(8)
    for( int input = 0 ; input < data_inputs ; input++ ) {

      //calculate the hidden activation
      pragma omp parallel for num_threads(8)
      for( int j = 0 ; j < ANN->size_layer2 ; j++ ) {
        sum_layer2[input][j] = ANN->weights_into_hidden[0][j] ;
        #pragma omp parallel for reduction(+:sum_layer2)
        for( int i = 0 ; i < ANN->size ; i++ ) {
          sum_layer2[input][j] += ANN->case_features[input][i] * ANN->weights_into_hidden[i][j] ;
        }
        hidden_layer[input][j] = 1.0/(1.0 + exp(-sum_layer2[input][j])) ;
      }//end of computing hidden activation loop

      //calculate the output activation
      for( int k = 0 ; k < ANN->size_layer3 ; k++ ) {
        sum_layer3[input][k] = ANN->weights_outof_hidden[0][k] ;
        #pragma omp parallel for reduction(+:sum_layer3)
        for( int j = 0 ; j < ANN->size_layer2 ; j++ ) {
          sum_layer3[input][k] += hidden_layer[input][j] * ANN->weights_outof_hidden[j][k] ;
        }
        // sigmoid the output layer
        output_layer[input][k] = 1.0/(1.0 + exp(-sum_layer3[input][k])) ;
        //calculate errors
        error += 0.5 * (ANN->cases[input][k] - output_layer[input][k]) * (ANN->cases[input][k] - output_layer[input][k]) ;
        //sigmoid the delta of output
        d_output_layer[k] = (ANN->cases[input][k] - output_layer[input][k]) * output_layer[input][k] * (1.0 - output_layer[input][k]) ;
       }//end of computing out activation

      #pragma omp parallel for num_threads(8)
       //back propagate to hidden layer
      for( int l = 0 ; l < ANN->size_layer2 ; l++ ) {
        sum_layer2_back[l] = 0.0 ;
        #pragma omp parallel for reduction(+:sum_layer2_back)
        for( int m = 1 ; m < ANN->size_layer3 ; m++ ) {
          sum_layer2_back[l] += ANN->weights_outof_hidden[l][m] * d_output_layer[m] ;
        }
        d_hidden_layer[l] = sum_layer2_back[l] * hidden_layer[input][l] * (1.0 - hidden_layer[input][l]) ;
      }//end of backpropagation for loop

      //update the weights between input and hidden layer
      #pragma omp parallel for num_threads(8)
      #pragma omp parallel for reduction(+:ANN->weights_into_hidden)
      for( int n = 0 ; n < ANN->size_layer2 ; n++ ) {
          ANN->d_weights_into_hidden[0][n] = ANN->eta * d_hidden_layer[n] + ANN->alpha * ANN->d_weights_into_hidden[0][n] ;
          ANN->weights_into_hidden[0][n] += ANN->d_weights_into_hidden[0][n] ;
          #pragma omp parallel for reduction(+:ANN->weights_into_hidden)
          for( int i = 0 ; i < ANN->size ; i++ ) {
              ANN->d_weights_into_hidden[i][n] = ANN->eta * ANN->case_features[input][i] * d_hidden_layer[n] + ANN->alpha * ANN->d_weights_into_hidden[i][n];
              ANN->weights_into_hidden[i][n] += ANN->d_weights_into_hidden[i][n] ;
          }
      }// end of updating weights between input and hidden layer

      //update weights between hidden and output layer
      #pragma omp parallel for reduction(+:ANN->ANN->weights_outof_hidden)
      for( int o = 0 ; o < ANN->size_layer3 ; o ++ ) {
               ANN->d_weights_outof_hidden[0][o] = ANN->eta * d_output_layer[o] + ANN->alpha * ANN->d_weights_outof_hidden[0][o] ;
               ANN->weights_outof_hidden[0][o] += ANN->d_weights_outof_hidden[0][o] ;
               #pragma omp parallel for reduction(+:ANN->ANN->weights_outof_hidden)
               for( int j = 0 ; j < ANN->size_layer2 ; j++ ) {
                   ANN->d_weights_outof_hidden[j][o] = ANN->eta * hidden_layer[input][j] * d_output_layer[o] + ANN->alpha * ANN->d_weights_outof_hidden[j][o] ;
                   ANN->weights_outof_hidden[j][o] += ANN->d_weights_outof_hidden[j][o] ;
               }
      }//end of updating weights between hidden and output layer


    }//end of input loop
    //printf("\n Iteration: %d", epoch);
    if( epoch%1000 == 0 ) {
      fprintf(stdout, "\nEpoch %-5d :   Error = %f", epoch, error) ;
    }
    if( error < 0.005 ) {
      break ;
    }
  }//end of epoch for loop


}//end of run


void loadDataset(ANN_t* ANN){
  FILE* stream = fopen("breast_cancer.csv", "r");

    double cases[683];
    double tmp_array[11];
    char line[1024];
    int counter=0;
    ANN->case_features[0][0]= 2.22;
    while (fgets(line, 1024, stream))
    {
        char* tmp = strdup(line);
        if(counter!=0){
          //printf("\n%s", tmp);
          int counter2=0;
          while (tmp != NULL){
            char* tmp2=strsep(&tmp, ",");
            tmp_array[counter2]= (double)atoi(tmp2);
            counter2+=1;
          }//end of inner while loop
          ANN->cases[counter-1][0]=(tmp_array[10]/2.0)-1.0;
          //the first item in the array is divide by 10 milion, the rest by 10
          ANN->case_features[counter-1][0]=tmp_array[0]/10000000.0;
          for(int j=1; j<10;j++){
            ANN->case_features[counter-1][j]=tmp_array[j]/10.0;
          }

        }//end of if
        counter+=1;
        //printf("\n Full added aray");
        for (int i=0;i<684; i++){
          //printf("%f", tmp_array[i]);
          //printf("%f", ANN->cases[i]);
        }
        //(value-min)/(max-min)
    }//end of outer while
    //printf("Testing:%f", ANN->case_features[682][0]);
    printf("\n End of load dataset\n");
}//end of load data set

int main(int argc, char** argv) {
  int a;
  int b;
  int c;
  int iterations;
  int nodes;
  struct timespec start_time;
  struct timespec end_time;
  iterations=atoi(argv[1]);
  nodes=atoi(argv[2]);
  a=10;
  b=nodes;
  c=1;
  ANN_t *ANN;
  ANN = (ANN_t*)malloc(sizeof(ANN_t));
  //double f_weight [a][b];
  ANN_init(ANN,a,b,c);
  initialize_weights(ANN,a,b,c);
  //Test initialize_weights
  loadDataset(ANN);
  clock_gettime(CLOCK_MONOTONIC,&start_time);
  run(ANN, iterations);
  clock_gettime(CLOCK_MONOTONIC,&end_time);
  long msec = (end_time.tv_sec - start_time.tv_sec)*1000 + (end_time.tv_nsec - start_time.tv_nsec)/1000000;
  printf("\nRun time in %dms\n",msec);
}
