/**
 * parser_driver.cc
 */

#include <Parser.h>

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

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
