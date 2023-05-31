//
// Created by Alexis Lavaud on 31/05/2023.
//

#include "HttpRegistry.h"
#include <map>

HttpMethod HttpRegistry::GetMethodByName(const std::string &name) {
    std::map<std::string, HttpMethod> methods;
    std::map<std::string, HttpMethod>::const_iterator it;

    methods.insert(std::make_pair("GET", kHttpMethod_Get));
    methods.insert(std::make_pair("PUT", kHttpMethod_Put));
    methods.insert(std::make_pair("PATCH", kHttpMethod_Patch));
    methods.insert(std::make_pair("HEAD", kHttpMethod_Head));
    methods.insert(std::make_pair("DELETE", kHttpMethod_Delete));
    methods.insert(std::make_pair("POST", kHttpMethod_Post));
    methods.insert(std::make_pair("OPTIONS", kHttpMethod_Options));

    if ((it = methods.find(name)) != methods.end())
        return it->second;
    return kHttpMethod_Invalid;
}

std::string HttpRegistry::GetMethodName(HttpMethod method) {
    std::map<HttpMethod, std::string> methods;
    std::map<HttpMethod, std::string>::const_iterator it;

    methods.insert(std::make_pair(kHttpMethod_Get, "GET"));
    methods.insert(std::make_pair(kHttpMethod_Put, "PUT"));
    methods.insert(std::make_pair(kHttpMethod_Patch, "PATCH"));
    methods.insert(std::make_pair(kHttpMethod_Head, "HEAD"));
    methods.insert(std::make_pair(kHttpMethod_Delete, "DELETE"));
    methods.insert(std::make_pair(kHttpMethod_Post, "POST"));
    methods.insert(std::make_pair(kHttpMethod_Options, "OPTIONS"));

    if ((it = methods.find(method)) != methods.end())
        return it->second;
    return "";
}
