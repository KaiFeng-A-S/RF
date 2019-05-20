#include "metrics.hpp"

float accuracy_score(int *y_true, int *y_pred, int length, bool normalize){
   int count = 0;
   for(int i = 0; i < length; i++){
       if(y_true[i] == y_pred[i]){
           count++;
       }
   }

   return normalize ? ((float) count / length) : (float) count;
}
