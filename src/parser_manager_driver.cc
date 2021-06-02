/**
 * parser_manager_driver.cc
 */

#include <getopt.h>

#include <iostream>
#include <signal.h>

#include <ParserManager.h>

black_library::core::parsers::ParserManager *parser_manager;


struct options
{
    std::string path = "";
    bool intialize_db = false;
};

static void Usage(const char *prog)
{
    const char *p = strchr(prog, '/');
    printf("usage: %s --(p)ath [-h]\n", p ? (p + 1) : prog);
}

static int ParseOptions(int argc, char **argv, struct options *opts)
{
    static const char *const optstr = "hp:";
    static const struct option long_opts[] = {
        { "help", no_argument, 0, 'h' },
        { "path", required_argument, 0, 'p' },
        { "verbose", no_argument, 0, 'v' },
        { 0, 0, 0, 0 }
    };

    if (!argv || !opts)
        return -1;

    int opt = 0;
    while ((opt = getopt_long(argc, argv, optstr, long_opts, 0)) >= 0)
    {
        switch (opt)
        {
            case 'h':
                Usage(argv[0]);
                exit(0);
                break;
            case 'p':
                opts->path = std::string(optarg);
                break;
            default:
                exit(1);
                break;
        }
    }

    if (optind < argc)
    {
        fprintf(stderr, "trailing options..\n");
        exit(1);
    }

    return 0;
}

void SigHandler(int sig)
{
    if (sig == SIGTERM || sig == SIGINT)
        parser_manager->Stop();
}

int main(int argc, char* argv[])
{
    struct options opts;

    if (argc < 2)
    {
        Usage(argv[0]);
        exit(1);
    }

    if (ParseOptions(argc, argv, &opts))
    {
        Usage(argv[0]);
        exit(1);
    }

    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);

    curl_global_init(CURL_GLOBAL_DEFAULT);

    black_library::core::parsers::ParserManager manager(opts.path, "");

    parser_manager = &manager;

    parser_manager->RegisterChapterNumberCallback(
        [](const std::string &uuid, size_t chapter_num)
        {
            std::cout << "ChapterNumberCallback: " << uuid << " - " << chapter_num << std::endl;
        }
    );
    parser_manager->RegisterDatabaseStatusCallback(
        [](black_library::core::parsers::ParserJobResult result)
        {
            std::cout << "DatabaseStatusCallback: " << result.metadata.uuid << std::endl;
        }
    );

    parser_manager->AddJob("some-uuid-0", "https://www.royalroad.com/fiction/21220/mother-of-learning");
    parser_manager->AddJob("some-uuid-1", "https://www.royalroad.com/fiction/42266/elysium");
    parser_manager->AddJob("some-uuid-2", "https://www.royalroad.com/fiction/17731/i-never-wanted-you-dead");
    parser_manager->AddJob("some-uuid-3", "https://www.royalroad.com/fiction/30692/divinity");
    parser_manager->AddJob("some-uuid-4", "https://www.royalroad.com/fiction/14167/metaworld-chronicles");

    parser_manager->AddJob("some-uuid-10", "https://forums.spacebattles.com/threads/commander-pa-multicross-si.309838/");
    parser_manager->AddJob("some-uuid-11", "https://forums.spacebattles.com/threads/be-thou-my-good.867883/");
    parser_manager->AddJob("some-uuid-12", "https://forums.spacebattles.com/threads/mha-worlds-finest-bnhaxbatman-complete.868273/");
    parser_manager->AddJob("some-uuid-13", "https://forums.spacebattles.com/threads/perchance-to-dream-mass-effect-commander.664360/");

    parser_manager->AddJob("some-uuid-20", "https://www.fanfiction.net/s/7347955/1/Dreaming-of-Sunshine");

    parser_manager->Run();

    curl_global_cleanup();

    std::cout << "parser_manager_driver exit successful" << std::endl;

    return 0;
}
