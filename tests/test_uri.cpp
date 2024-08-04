/**
 * @file test_uri.cpp
 * @author DrkWithT
 * @brief Implements unit test for URI helpers.
 * @date 2024-07-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>
#include "uri/parse.hpp"
#include "uri/url.hpp"

using namespace ToyServer::Uri;

int main()
{
    std::string base_uri {"/"};
    std::string path_uri {"/api/foo"};
    std::string query_uri {"/api/echo?msg=test&count=10"};

    UrlParser parser {};

    std::cout << "P1...\n";
    parser.reset(base_uri);
    auto base_res = parser.parseAll();

    if (base_res.path != "/")
    {
        std::cerr << "Invalid path for base_uri: " << base_res.path << '\n';
        return 1;
    }

    std::cout << "P2...\n";
    parser.reset(path_uri);
    auto path_res = parser.parseAll();

    if (path_res.path != "/api/foo")
    {
        std::cout << "Invalid path for path_uri: " << path_res.path << '\n';
        return 1;
    }

    std::cout << "P3\n";
    parser.reset(query_uri);
    auto query_res = parser.parseAll();

    if (query_res.path != "/api/echo")
    {
        std::cout << "Invalid path for query_uri: " << path_res.path << '\n';
        return 1;
    }

    auto query_args = unpackItem<UrlItemTag::ur_params>(query_res);

    std::string param1_copy = query_args.at("msg");
    std::string param2_copy = query_args.at("count");

    if (param1_copy != "test")
    {
        std::cout << "Invalid value for query param 'msg': " << param1_copy << '\n';
        return 1;
    }

    if (param2_copy != "10")
    {
        std::cout << "Invalid value for query param 'count': " << param2_copy << '\n';
        return 1;
    }
}
