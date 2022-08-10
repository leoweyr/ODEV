#pragma once

#include <string>

#include "Jsoncpp.h"

namespace Json{
    class Comparer{
        private:
            Json::Value m_compared;
            Json::Value m_referred;
            //Prevent conflict between asynchronous simultaneous outputs (member:public)<differences> and (member:public)<same> and ongoing method tasks.
            int m_jsonLevel = 0;
            Json::Value m_differences_roll_compare;
            Json::Value m_differences_roll_isSame;
            Json::Value m_same_roll_compare;

        public:
            //The public member & method name design caters to the original habit of Jsoncpp ontology.
            Json::Value differences;
            Json::Value same;

            void compare(const Json::Value compared, const Json::Value referred);
            bool isIncluded(const std::string key = "");
            bool isSame(const std::string key = "");
    };
}