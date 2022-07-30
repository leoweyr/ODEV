#pragma once

#include <vector>

#include "../AftermathList/AftermathList.h" //instantiate

template <class T> struct S_SortedData{
    int weight;
    T *data;
};

template <typename T> S_SortedData<T> MinData(const S_SortedData<T> dataX, const S_SortedData<T> dataY, int *which = nullptr);

template <typename T> S_SortedData<T> MaxData(const S_SortedData<T> dataX, const S_SortedData<T> dataY, int *which = nullptr);

template <typename T> void MergeSortData(const std::vector<S_SortedData<T>> &data);