#include "SortData_instantiate.h"

template <typename T> S_SortedData<T> MinData(const S_SortedData<T> dataX, const S_SortedData<T> dataY, int *which){
    if(dataX.weight == dataY.weight){
        *which = 0;
        return dataX;
    }
    *which = (dataX.weight < dataY.weight)?(1):(2);
    return (dataX.weight < dataY.weight)?(dataX):(dataY);
}

template <typename T> S_SortedData<T> MaxData(const S_SortedData<T> dataX, const S_SortedData<T> dataY, int *which){
    if(dataX.weight == dataY.weight){
        *which = 0;
        return dataX;
    }
    *which = (dataX.weight > dataY.weight)?(1):(2);
    return (dataX.weight > dataY.weight)?(dataX):(dataY);
}

template <typename T> void MergeSortData(const std::vector<S_SortedData<T>> &data){
    std::vector<S_SortedData<T>> dataPart, dataPartX, dataPartY;
    int data_index = 1;
    for(typename std::vector<S_SortedData<T>>::iterator data_iter = const_cast<std::vector<S_SortedData<T>> &>(data).begin(); data_iter != data.end(); data_iter++){
        if(data_index > data.size() / 2){
            dataPartY.push_back(*data_iter);
        }else{
            dataPartX.push_back(*data_iter);
        }
        data_index++;
    }
    if(data.size() > 2){
        MergeSortData(dataPartX);
        MergeSortData(dataPartY);
    }
    typename std::vector<S_SortedData<T>>::iterator dataPartX_iter = dataPartX.begin(), dataPartY_iter = dataPartY.begin();
    int whichMin;
    while (dataPartX_iter != dataPartX.end() && dataPartY_iter != dataPartY.end()){
        MinData(*dataPartX_iter,*dataPartY_iter,&whichMin);
        if(whichMin == 0){
            dataPart.push_back(*dataPartX_iter);
            dataPart.push_back(*dataPartY_iter);
            dataPartX_iter++;
            dataPartY_iter++;
        }else if(whichMin == 1){
            dataPart.push_back(*dataPartX_iter);
            dataPartX_iter++;
        }else if(whichMin == 2){
            dataPart.push_back(*dataPartY_iter);
            dataPartY_iter++;
        }
    }
    while(dataPartX_iter == dataPartX.end() && dataPartY_iter != dataPartY.end()){
        dataPart.push_back(*dataPartY_iter);
        dataPartY_iter++;
    }
    while(dataPartY_iter == dataPartY.end() && dataPartX_iter != dataPartX.end()){
        dataPart.push_back(*dataPartX_iter);
        dataPartX_iter++;
    }
    const_cast<std::vector<S_SortedData<T>> &>(data) = dataPart;
}

template void MergeSortData<C_Aftermath>(const std::vector<S_SortedData<C_Aftermath>> &data); //instantiate