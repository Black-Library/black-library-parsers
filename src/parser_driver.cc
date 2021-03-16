/**
 * parser_driver.cc
 */

/*extern "C" {
    #include <curl/curl.h>
}*/

#include <Parser.h>
//#include "ParsersConfig.h"

#include <ParserAO3.h>
#include <ParserRR.h>

int main(int argc, char* argv[])
{
    // check to see if there are any extra arguments
    if (argc > 1)
    {
        std::cout << "extra arguments ";
        for (int i = 0; i < argc; ++i)
        {
            std::cout << std::string(argv[i]);
        }
        std::cout << std::endl;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    // black_library::core::parsers::AO3::ParserAO3 parser;
    // parser.SetUrl("https://archiveofourown.org/works/505809");
    // parser.Parse();

    black_library::core::parsers::RR::ParserRR parser2;
    parser2.SetUrl("https://www.royalroad.com/fiction/21220/mother-of-learning");
    parser2.Parse();

    std::string doc_string_0 = parser2.TrimWhitespace("\n\n\n\n\n\n\n");
    std::cout << "size: " << doc_string_0.size() << " - string: " << doc_string_0 << std::endl;

    std::string doc_string_1 = parser2.TrimWhitespace("a\n\n\n\n\n\n");
    std::cout << "size: " << doc_string_1.size() << " - string: " << doc_string_1 << std::endl;

    std::string doc_string_2 = parser2.TrimWhitespace("\n\n\n\n\n\nb");
    std::cout << "size: " << doc_string_2.size() << " - string: " << doc_string_2 << std::endl;

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
