#ifndef URL_HPP
#define URL_HPP

#include <type_traits>
#include <utility>
#include <stdexcept>
#include <string>
#include <map>
#include <variant>

namespace ToyServer::Uri
{
    /**
     * @brief Enumeration for type of URL item(s).
     */
    enum class UrlItemTag
    {
        ur_none,
        ur_params
    };

    /**
     * @brief Placeholder aggregate for non-resource URL.
     */
    struct NullUrlItem {};

    /**
     * @brief Type selection helper for Url item results.
     */
    template <UrlItemTag UTag>
    struct UrlItemChooser
    {
        using type = void;
    };

    template <>
    struct UrlItemChooser<UrlItemTag::ur_none>
    {
        using type = NullUrlItem;
    };

    template <>
    struct UrlItemChooser<UrlItemTag::ur_params>
    {
        using type = std::map<std::string, std::string>;
    };

    /**
     * @brief Alias for `UrlItemChooser`.
     */
    template <UrlItemTag UTag>
    using url_item_t = UrlItemChooser<UTag>::type;

    /**
     * @brief Aggregate for modeling a relative URL.
     */
    struct Url
    {
        std::string path;
        std::variant<NullUrlItem, std::map<std::string, std::string>> content;
        UrlItemTag tag;

        constexpr Url(std::string path_)
        : path {path_}, content (NullUrlItem {}), tag {UrlItemTag::ur_none} {}

        Url(std::string path_, std::map<std::string, std::string> params_)
        : path {path_}, content (std::move(params_)), tag {UrlItemTag::ur_params} {}
    };

    template <UrlItemTag UTag>
    constexpr auto unpackItem(const Url& url) -> url_item_t<UTag>
    {
        if constexpr (std::is_void_v<url_item_t<UTag>>)
            throw std::invalid_argument {"Invalid tag type passed to Uri::unpackItem(const Url&)!"};

        constexpr int var_pos = static_cast<int>(UTag);

        return std::get<var_pos>(url.content);
    }
}

#endif