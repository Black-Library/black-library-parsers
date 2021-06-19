#ifndef __BLACK_LIBRARY_CORE_PARSERS_PARSERHTML_PARSER_STRATEGY_H__
#define __BLACK_LIBRARY_CORE_PARSERS_PARSERHTML_PARSER_STRATEGY_H__

namespace black_library {

namespace core {

namespace parsers {

namespace html_strategy {

class HTMLStrategy
{
public:
    virtual std::string getHTMLRequest(const std::string& url);
private:

};

class CURLStrategy : public HTMLStrategy
{
public:
    CURLStrategy();
    ~CURLStrategy();

    std::string getHTMLRequest(const std::string& url);
private:

};

class SeleniumStrategy : public HTMLStrategy
{
public:
    SeleniumStrategy();
    ~SeleniumStrategy();

    std::string getHTMLRequest(const std::string& url);
private:

};

} // namespace html_strategy
} // namespace parsers
} // namespace core
} // namespace black_library

#endif
