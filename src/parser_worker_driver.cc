/**
 * parser_worker_driver.cc
 */

#include <iostream>
#include <memory>
#include <signal.h>

#include <Parser.h>
#include <ParserCommon.h>
#include <ParserFactory.h>
#include <ParserWorker.h>

#include <ParserRR.h>

namespace ParserCommon = black_library::core::parsers;

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
        exit(1);
    }

    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    using dummy_worker = black_library::core::parsers::ParserWorker;

    auto factory = std::make_shared<black_library::core::parsers::ParserFactory>();

    dummy_worker dummy_worker_0(factory, "/mnt/store", black_library::core::parsers::ERROR_PARSER, 3);
    dummy_worker dummy_worker_1(factory, "/mnt/store", black_library::core::parsers::RR_PARSER, 2);

    parser_worker = &dummy_worker_1;

    black_library::core::parsers::ParserJob job_0;
    black_library::core::parsers::ParserJob job_1;
    black_library::core::parsers::ParserJob job_2;

    job_0.uuid = "some-uuid-0";
    job_1.uuid = "some-uuid-1";
    job_2.uuid = "some-uuid-2";

    job_0.url = "https://www.royalroad.com/fiction/21220/mother-of-learning";
    job_1.url = "https://www.royalroad.com/fiction/17731/i-never-wanted-you-dead";
    job_2.url = "https://www.royalroad.com/fiction/16946/azarinth-healer";

    parser_worker->RegisterJobStatusCallback(
            [](const std::string &uuid, ParserCommon::job_status_rep job_status)
            {
                std::cout << "JobStatusCallback uuid: " << uuid << " - " << ParserCommon::GetStatusName(job_status) << std::endl;
            }
    );
    parser_worker->RegisterManagerNotifyCallback(
            [](ParserCommon::ParserJobResult result)
            {
                std::cout << "ManagerNotifyCallback: " << result.metadata.uuid << " - " << result.metadata.url << std::endl;
            }
    );

    parser_worker->AddJob(job_0);
    parser_worker->AddJob(job_1);
    parser_worker->AddJob(job_2);

    parser_worker->Run();

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
