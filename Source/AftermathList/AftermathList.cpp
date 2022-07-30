#include "AftermathList.h"

void C_AftermathList::QueryAftermath(const C_Aftermath *condition, std::vector<C_Aftermath *> &results) {
    for(std::vector<C_Aftermath>::iterator aftermaths_iter = m_aftermaths.begin(); aftermaths_iter != m_aftermaths.end(); aftermaths_iter++){
        if(condition != NULL){
            if(((*aftermaths_iter).m_priority != condition->m_priority || condition->m_priority != NULL) && ((*aftermaths_iter).m_from != condition->m_from || condition->m_from != NULL) && ((*aftermaths_iter).m_name != condition->m_name || condition->m_name.size() != 0) && ((*aftermaths_iter).m_method != condition->m_method || condition->m_method.size() != 0)){
                continue;
            }
        }
        results.push_back(&(*aftermaths_iter));
    }
}

void C_AftermathList::AddAftermath(const C_Aftermath aftermath) {
    m_aftermaths.push_back(aftermath);
}

void C_AftermathList::RemoveProgram(const C_Aftermath *aftermath) {
    for(std::vector<C_Aftermath>::iterator aftermaths_iter = m_aftermaths.begin(); aftermaths_iter != m_aftermaths.end(); aftermaths_iter++){
        if(((*aftermaths_iter).m_priority == aftermath->m_priority || aftermath->m_priority == NULL) && ((*aftermaths_iter).m_from == aftermath->m_from || aftermath->m_from == NULL) && ((*aftermaths_iter).m_name == aftermath->m_name || aftermath->m_name.size() == 0) && ((*aftermaths_iter).m_method == aftermath->m_method || aftermath->m_method.size() == 0)){
            m_aftermaths.erase(aftermaths_iter);
        }
    }
}

void C_AftermathList::Adjust() {
    //Sort aftermaths according to priority from small to large.
    S_SortedData<C_Aftermath> sortedAftermath;
    std::vector<S_SortedData<C_Aftermath>> sortedAftermaths;
    std::vector<C_Aftermath> aftermaths_new;

    for(std::vector<C_Aftermath>::iterator aftermaths_iter = m_aftermaths.begin(); aftermaths_iter != m_aftermaths.end(); aftermaths_iter++){
        sortedAftermath.weight = (*aftermaths_iter).m_priority;
        sortedAftermath.data = &(*aftermaths_iter);
        sortedAftermaths.push_back(sortedAftermath);
    }
    MergeSortData(sortedAftermaths);
    //Write aftermaths handler to the already sorted queue.
    HMODULE hDLL;
    std::string handlerDllPath;
    void (*handlerFunction)(Json::Value);
    for(std::vector<S_SortedData<C_Aftermath>>::iterator sortedAftermaths_iter = sortedAftermaths.begin(); sortedAftermaths_iter != sortedAftermaths.end(); sortedAftermaths_iter++){
        if((*sortedAftermaths_iter).data->m_from == FROM_PRIVATE){
            handlerDllPath = g_currentProjectPath + "\\" + g_privatePath_aftermath + "\\" + (*sortedAftermaths_iter).data->m_name + ".dll";
        }else if((*sortedAftermaths_iter).data->m_from == FROM_PUBLIC){
            handlerDllPath = g_publicPath_aftermath + "\\" + (*sortedAftermaths_iter).data->m_name + ".dll";
        }
        hDLL = LoadLibrary(handlerDllPath.data());
        handlerFunction = (void(*)(Json::Value))GetProcAddress(hDLL, (*sortedAftermaths_iter).data->m_method.data());
        (*sortedAftermaths_iter).data->m_Handler = handlerFunction;
        aftermaths_new.push_back(*((*sortedAftermaths_iter).data));
        FreeLibrary(hDLL);
    }
    m_aftermaths = aftermaths_new;
}

void C_AftermathList::Handle(const Json::Value buildWayReturn) {
    for(std::vector<C_Aftermath>::reverse_iterator aftermaths_iter = m_aftermaths.rbegin(); aftermaths_iter != m_aftermaths.rend(); aftermaths_iter++){
        (*aftermaths_iter).m_Handler(buildWayReturn);
    }
}