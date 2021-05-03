/**
 * parser_worker_driver.cc
 */

#include <iostream>
#include <memory>
#include <signal.h>

#include <Parser.h>
#include <ParserWorker.h>

#include <ParserRR.h>

black_library::core::parsers::ParserWorker *parser_worker;

void SigHandler(int sig)
{
    if (sig == SIGTERM || sig == SIGINT)
        parser_worker->Stop();
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

    using dummy_parser = black_library::core::parsers::Parser;

    auto dummy_one = std::make_shared<dummy_parser>(black_library::core::parsers::_NUM_PARSERS_TYPE);
    dummy_one->SetParserIndex(0);

    auto dummy_two = std::make_shared<dummy_parser>(black_library::core::parsers::_NUM_PARSERS_TYPE);
    dummy_two->SetParserIndex(1);

    std::cout << "dummy_one: " << dummy_one->GetParserIndex() << std::endl;
    std::cout << "dummy_two: " << dummy_two->GetParserIndex() << std::endl;

    black_library::core::parsers::ParserWorker worker(std::make_shared<black_library::core::parsers::RR::ParserRR>(), black_library::core::parsers::RR_PARSER, 1);

    parser_worker = &worker;

    black_library::core::parsers::ParserJob job;

    job.uuid = "some-uuid-0";
    job.url = "https://www.royalroad.com/fiction/21220/mother-of-learning";

    parser_worker->AddJob(job);
    parser_worker->Run();

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
