/**
 * @file messages.cpp
 * @author DrkWithT
 * @brief Stub impls. of Request and Response ctors.
 * @date 2024-08-07
 */

#include "http1/messages.hpp"

namespace ToyServer::Http1
{
    Request::Request() noexcept
    : schema {Schema::http_unknown}, method {Method::h1_unknown}, route {"/"}, headers {}, body {0} {}

    Request::Request(Schema schema_, Method method_, Uri::Url route_, std::map<std::string, std::string> headers_, NetIO::FixedBuffer body_) noexcept
    : schema {schema_}, method {method_}, route {route_}, headers(std::move(headers_)), body (std::move(body_)) {}

    Response::Response() noexcept
    : schema {Schema::http_unknown}, status {Status::stat_unknown}, status_txt {""}, headers {}, body {0} {}

    Response::Response(Schema schema_, Status status_, std::string_view status_txt_, std::map<std::string, std::string> headers_, NetIO::FixedBuffer body_) noexcept
    : schema {schema_}, status {status_}, status_txt {status_txt_}, headers (std::move(headers_)), body (std::move(body_)) {}
}