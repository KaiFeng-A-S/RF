#pragma once
#ifndef METRICS_H
#define METRICS_H

float accuracy_score(int *y_true, int *y_pred, int length, bool normalize);

void roc_curve(float *y_true, float *y_score, int length, float *fprs, float *tprs);

float auc(float *x, float * y, int length);

#endif
