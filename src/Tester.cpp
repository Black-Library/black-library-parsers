/*extern "C" {
    #include <curl/curl.h>
}*/

#include <Parser.h>
//#include "ParsersConfig.h"

#include <ParserAO3.h>

int main(int argc, char* argv[]) {

    if (argc > 0) {
        std::cout << "extra arguments ";
        for (int i = 0; i < argc; ++i) {
            std::cout << std::string(argv[i]);
        }
        std::cout << std::endl;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    AO3::ParserAO3 parser;
    // parser.setUrl("https://archiveofourown.org/works/23088748");
    parser.parse();

    curl_global_cleanup();
    return 0;
}
