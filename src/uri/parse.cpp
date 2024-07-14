/**
 * @file parse.cpp
 * @author DrkWithT
 * @brief Implements simple parser for relative URLs.
 * @date 2024-07-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "uri/parse.hpp"
#include <stdexcept>
#include <utility>
#include <string>
#include <string_view>
#include <vector>

namespace ToyServer::Uri
{
    static constexpr std::string_view placeholder_src = "";

    /// Token helper impl.

    std::string getLexeme(const Token& token, const std::string& source)
    {
        return source.substr(token.start, token.length);
    }

    /// Lexer private impl.

    bool UrlLexer::atEOS() const
    {
        return pos >= limit;   
    }

    Token UrlLexer::lexSingle(TokenTag tag)
    {
        std::size_t temp_start = pos++;
        return {temp_start, 1, tag};
    }

    Token UrlLexer::lexWord()
    {
        std::size_t start = pos;
        std::size_t count = 0;

        while (atEOS())
        {
            char s = symbols[pos];

            if (!isWordSymbol(s))
                break;

            count++;
            pos++;
        }

        return {start, count, TokenTag::url_word};
    }

    /// Lexer public impl.

    UrlLexer::UrlLexer(const std::string& url_str)
    : symbols {url_str}, pos {0}, limit {url_str.length()} {}

    void UrlLexer::swapState(UrlLexer&& other) noexcept
    {
        std::string_view temp_src = placeholder_src;
        std::swap(temp_src, other.symbols);

        std::size_t temp_pos = 0;
        std::swap(temp_pos, other.pos);

        std::size_t temp_limit = 0;
        std::swap(temp_limit, other.limit);

        symbols = temp_src;
        pos = temp_pos;
        limit = temp_limit;
    }

    Token UrlLexer::lexNext()
    {
        if (atEOS())
            return {limit, 1, TokenTag::url_eos};

        char temp = symbols[pos];

        switch (temp)
        {
            case ':':
                return lexSingle(TokenTag::url_colon);
            case '/':
                return lexSingle(TokenTag::url_slash);
            case '.':
                return lexSingle(TokenTag::url_dot);
            case '?':
                return lexSingle(TokenTag::url_question);
            case '=':
                return lexSingle(TokenTag::url_equals);
            case '&':
                return lexSingle(TokenTag::url_ampersand);
            default:
                break;
        }

        if (isWordSymbol(temp))
            return lexWord();

        std::size_t unknown_pos = pos++;

        return {unknown_pos, 1, TokenTag::url_unknown};
    }

    /// Parser private impl.

    const Token& UrlParser::peekBack() const
    {
        return previous;
    }

    const Token& UrlParser::peekCurrent() const
    {
        return current;
    }

    bool UrlParser::atEOF() const
    {
        return peekCurrent().tag == TokenTag::url_eos;
    }

    bool UrlParser::matchToken(TokenTag tag) const
    {
        return tag == TokenTag::url_any || peekCurrent().tag == tag;
    }

    Token UrlParser::advance()
    {
        Token temp;
        bool found_bad_token = false;

        do
        {
            temp = lexer.lexNext();

            if (temp.tag == TokenTag::url_eos || temp.tag != TokenTag::url_unknown)
                break;

            found_bad_token = true;
        }
        while (!found_bad_token);

        return temp;
    }

    void UrlParser::consume(TokenTag tag)
    {
        if (atEOF())
            return;

        if (matchToken(tag))
        {
            previous = current;
            current = advance();
            return;
        }

        throw std::runtime_error {"Invalid URL token found."};
    }

    UrlFragment UrlParser::parsePath()
    {
        std::string temp {};

        while (!atEOF())
        {
            if (!matchToken(TokenTag::url_slash) && !matchToken(TokenTag::url_word) && !matchToken(TokenTag::url_dot))
                break;

            temp += getLexeme(peekCurrent(), symbols);
            consume(TokenTag::url_any);
        }

        return {temp, FragmentTag::url_path};
    }

    UrlFragment UrlParser::parseParamPair()
    {
        if (atEOF())
            return {};

        std::string key {};
        std::string value {};

        consume(TokenTag::url_word);
        key = getLexeme(peekBack(), symbols);

        consume(TokenTag::url_equals);
        consume(TokenTag::url_word);
        value = getLexeme(peekBack(), symbols);

        std::pair<std::string, std::string> temp {std::move(key), std::move(value)};

        return {temp};
    }

    /// Parser public impl.

    UrlParser::UrlParser()
    : symbols {}, previous {}, current {}, lexer {std::string {}}
    {
        current.tag = TokenTag::url_eos;
    }

    void UrlParser::reset(const std::string& url_str)
    {
        symbols = url_str;
        previous = {0, 0, TokenTag::url_unknown};
        current = {0, 0, TokenTag::url_unknown};
        lexer.swapState( UrlLexer {url_str});
    }

    Url UrlParser::parseAll()
    {
        // 1. parse first thing- the path!
        UrlFragment path_frag = parsePath();
        auto path_str = unpackFragment<FragmentTag::url_path>(path_frag);
        
        // 2. handle optional query if needed...
        if (matchToken(TokenTag::url_question))
        {
            consume(TokenTag::url_question);
            std::map<std::string, std::string> params {};

            while (true)
            {
                auto frag = parseParamPair();

                if (frag.tag == FragmentTag::url_chunkless)
                    break;

                auto [key, value] = unpackFragment<FragmentTag::url_param>(frag);

                params[key] = std::move(value);
            }

            return {path_str, params};
        }

        return {path_str};
    }
}
