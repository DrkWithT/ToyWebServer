#ifndef PARSE_HPP
#define PARSE_HPP

#include <type_traits>
#include <stdexcept>
#include <utility>
#include <string>
#include <string_view>
#include <variant>
#include "uri/url.hpp"

namespace ToyServer::Uri
{
    /**
     * @brief relative URL token types
     */
    enum class TokenTag
    {
        url_any,
        url_unknown,
        url_colon,
        url_slash,
        url_dot,
        url_question,
        url_equals,
        url_ampersand,
        url_word,
        url_eos
    };

    /**
     * @brief lexical fragment of a relative URL
     */
    struct Token
    {
        std::size_t start;
        std::size_t length;
        TokenTag tag;
    };

    std::string getLexeme(const Token& token, const std::string& source);

    /// @brief Lexing helper function.
    constexpr bool isAlphaNum(char c)
    {
        return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
    }

    /**
     * @brief Tokenizer for relative URL string.
     */
    class UrlLexer
    {
    private:
        std::string_view symbols;
        std::size_t pos;
        std::size_t limit;

        [[nodiscard]] bool atEOS() const;

        Token lexSingle(TokenTag tag);
        Token lexWord();

    public:
        UrlLexer(const std::string& url_str);

        void swapState(UrlLexer&& other) noexcept;

        [[nodiscard]] Token lexNext();
    };

    /// @brief Type tag for parsed URL chunks.
    enum class FragmentTag
    {
        url_chunkless,
        url_path,
        url_param
    };

    struct NullChunk {};

    /// @brief Helper for UrlFragment content type deduction.
    template <FragmentTag FragTag>
    struct UrlFragChooser
    {
        using type = void;
    };

    template <>
    struct UrlFragChooser<FragmentTag::url_chunkless>
    {
        using type = NullChunk;
    };

    template <>
    struct UrlFragChooser<FragmentTag::url_path>
    {
        using type = std::string;
    };

    template <>
    struct UrlFragChooser<FragmentTag::url_param>
    {
        using type = std::pair<std::string, std::string>;
    };

    /// @brief Alias for UrlFragment native type helper.
    template <FragmentTag FragTag>
    using url_frag_choose_t = UrlFragChooser<FragTag>::type;

    /// @brief Aggregate for parsed URL chunk.
    struct UrlFragment
    {
        std::variant<NullChunk, std::string, std::pair<std::string, std::string>> chunk;
        FragmentTag tag;

        constexpr UrlFragment()
        : chunk (NullChunk {}), tag {FragmentTag::url_chunkless} {}

        constexpr UrlFragment(std::string text_, FragmentTag tag_)
        : chunk (std::move(text_)), tag {tag_} {}

        constexpr UrlFragment(std::pair<std::string, std::string> pair_)
        : chunk (std::move(pair_)), tag {FragmentTag::url_param} {}
    };

    /// @brief Helper to unpack UrlFragment tag.
    template <FragmentTag FragTag>
    constexpr auto unpackFragment(const UrlFragment& fragment) -> url_frag_choose_t<FragTag>
    {
        if constexpr (std::is_void_v<url_frag_choose_t<FragTag>>)
            throw std::invalid_argument {"Invalid unpack type for UrlFragment."};

        if constexpr (FragTag == FragmentTag::url_chunkless)
            return std::get<0>(fragment.chunk);

        if constexpr (FragTag == FragmentTag::url_path)
            return std::get<1>(fragment.chunk);

        if constexpr (FragTag == FragmentTag::url_param)
            return std::get<2>(fragment.chunk);
        
        throw std::runtime_error {"Unreachable FragTag case for UrlFragment."};
    }

    /**
     * @brief Recursive descent parser for relative URLs.
     * Rules:
     * relative-url = path ?after;
     * after = params;
     * path = "/" (word ?"/")* ?file;
     * file = word "." word;
     * params = param ("&" param)*;
     * param = word "=" word;
     * @note The parsing grammar is a CFG one, but the simple format of a relative URL keeps it easy- path goes first and then params or file last.
     */
    class UrlParser
    {
    private:
        std::string symbols;
        Token previous;
        Token current;
        UrlLexer lexer;

        const Token& peekBack() const;
        const Token& peekCurrent() const;
        [[nodiscard]] bool atEOF() const;
        [[nodiscard]] bool matchToken(TokenTag tag) const;

        Token advance();
        void consume(TokenTag tag);

        UrlFragment parsePath();
        UrlFragment parseParamPair();

    public:
        /// @note Partial initialization is desired here, since I don't want to recreate a whopping "big" parser everytime (104B). Just reset it when needed!
        UrlParser();

        void reset(const std::string& url_str);

        [[nodiscard]] Url parseAll();
    };
}

#endif