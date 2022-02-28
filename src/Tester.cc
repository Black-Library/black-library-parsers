/*extern "C" {
    #include <curl/curl.h>
}*/

#include <Parser.h>
#include "ParsersConfig.h"
#include <ParserAO3.h>
#include <ParserFFN.h>

int main(int argc, char* argv[]) {

    // check to see if there are any extra arguments
    if (argc > 1) {
        std::cout << "extra arguments ";
        for (int i = 0; i < argc; ++i) {
            std::cout << std::string(argv[i]);
        }
        std::cout << std::endl;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    njson j;
    black_library::core::parsers::FFN::ParserFFN parser(j);
    black_library::core::parsers::ParserJob testJob = {
        "TESTUUID",
        "https://www.fanfiction.net/s/8019901",
        ""
    };
    parser.Parse(testJob);

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
