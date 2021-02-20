/*extern "C" {
    #include <curl/curl.h>
}*/

#include <signal.h>

#include <ParserManager.h>

librarycore::ParserManager *parser_manager;

void SigHandler(int sig)
{
    if (sig == SIGTERM || sig == SIGINT)
        parser_manager->Stop();
}

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

    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    librarycore::ParserManager manager(4, "");

    parser_manager = &manager;

    parser_manager->AddUrl("https://archiveofourown.org/works/505809");

    parser_manager->Run();

    curl_global_cleanup();

    std::cout << "parser_manager_driver exit successful" << std::endl;

    return 0;
}
