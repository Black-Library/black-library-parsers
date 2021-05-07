/**
 * parser_driver.cc
 */

#include <iostream>

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

    black_library::core::parsers::RR::ParserRR parser;
    black_library::core::parsers::ParserJob parser_job;
    parser_job.url = "https://www.royalroad.com/fiction/21220/mother-of-learning";
    parser.Parse(parser_job);

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
