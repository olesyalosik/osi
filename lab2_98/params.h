#ifndef LAB2_98_PARAMS_H
#define LAB2_98_PARAMS_H

struct MinMaxThreadParams {
    int arrayMin;
    int arrayMax;
    int *arr;
    int numberOfElements;
};

struct AverageThreadParams {
    int *arr;
    int numberOfElements;
    int arrayAverage;
};

#endif //LAB2_98_PARAMS_H
