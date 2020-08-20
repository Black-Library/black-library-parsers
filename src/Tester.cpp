/*extern "C" {
    #include <curl/curl.h>
}*/

#include "../include/Parser.h"
//#include "ParsersConfig.h"

#include "../include/ParserAO3.h"

int main(int argc, char* argv[]) {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    ParserAO3 parser;
    // parser.setUrl("https://archiveofourown.org/works/23088748");
    parser.parse();

    curl_global_cleanup();
    return 0;
}
