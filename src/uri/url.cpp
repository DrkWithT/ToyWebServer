/**
 * @file url.cpp
 * @author DrkWithT
 * @brief Implements any URL helpers.
 * @date 2024-07-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "uri/url.hpp"

namespace ToyServer::Uri
{
    template <>
    constexpr UrlItemTag url_item_tag_v<NullUrlItem> = UrlItemTag::ur_none;

    template <>
    constexpr UrlItemTag url_item_tag_v<std::string> = UrlItemTag::ur_file;

    template <>
    constexpr UrlItemTag url_item_tag_v<std::map<std::string, std::string>> = UrlItemTag::ur_params;
}