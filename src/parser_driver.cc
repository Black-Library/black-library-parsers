/**
 * parser_driver.cc
 */

#include <getopt.h>
#include <stdlib.h>

#include <iostream>
#include <memory>
#include <unordered_map>

#include <Parser.h>

#include <ParserAO3.h>
#include <ParserRR.h>
#include <ParserSBF.h>

static constexpr const char AO3_SHORT_URL[] = "https://archiveofourown.org/works/505809";
static constexpr const char RR_0_URL[] = "https://www.royalroad.com/fiction/15614/daedalus";
static constexpr const char RR_SHORT_URL[] = "https://www.royalroad.com/fiction/17731/i-never-wanted-you-dead";
static constexpr const char RR_LONG_URL[] = "https://www.royalroad.com/fiction/21220/mother-of-learning";
static constexpr const char SBF_SHORT_URL[] = "https://forums.spacebattles.com/threads/new-operational-parameters.815612/";
static constexpr const char SBF_LONG_URL[] = "https://forums.spacebattles.com/threads/intrepid-worm-au.337516/";

namespace ParserCommon = black_library::core::parsers;

using ParserJob = black_library::core::parsers::ParserJob;

using Parser = black_library::core::parsers::Parser;
using ParserAO3 = black_library::core::parsers::AO3::ParserAO3;
using ParserRR = black_library::core::parsers::RR::ParserRR;
using ParserSBF = black_library::core::parsers::SBF::ParserSBF;

template <typename T>
inline std::shared_ptr<Parser> ParserCast(T const &p)
{
    return std::static_pointer_cast<Parser>(p);
}

struct options
{
    ParserCommon::parser_rep source;
    uint8_t length;
};

static void Usage(const char *prog)
{
    const char *p = strchr(prog, '/');
    printf("usage: %s --(s)ource_target -(l)ength 0-2 [-h]\n", p ? (p + 1) : prog);
}

static int ParseOptions(int argc, char **argv, struct options *opts)
{
    static const char *const optstr = "hl:s:";
    static const struct option long_opts[] = {
        { "help", no_argument, 0, 'h' },
        { "length", required_argument, 0, 'l' },
        { "source_target", required_argument, 0, 's' },
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
            case 'l':
                if (atoi(optarg) > 2 || atoi(optarg) < 0)
                {
                    std::cout << "Length out of range" << std::endl;
                    Usage(argv[0]);
                    exit(1);
                }
                opts->length = atoi(optarg);
                break;
            case 's':
                if (std::string(optarg) == "ao3")
                {
                    opts->source = ParserCommon::AO3_PARSER;
                }
                else if (std::string(optarg) == "rr")
                {
                    opts->source = ParserCommon::RR_PARSER;
                }
                else if (std::string(optarg) == "sbf")
                {
                    opts->source = ParserCommon::SBF_PARSER;
                }
                else
                {
                    std::cout << "Could not match source" << std::endl;
                    Usage(argv[0]);
                    exit(1);
                }
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

    std::unordered_map<std::string, std::string> url_map;
    url_map.emplace(ParserCommon::GetParserName(ParserCommon::AO3_PARSER) + "1", std::string(AO3_SHORT_URL));
    url_map.emplace(ParserCommon::GetParserName(ParserCommon::RR_PARSER) + "0", std::string(RR_0_URL));
    url_map.emplace(ParserCommon::GetParserName(ParserCommon::RR_PARSER) + "1", std::string(RR_SHORT_URL));
    url_map.emplace(ParserCommon::GetParserName(ParserCommon::RR_PARSER) + "2", std::string(RR_LONG_URL));
    url_map.emplace(ParserCommon::GetParserName(ParserCommon::SBF_PARSER) + "1", std::string(SBF_SHORT_URL));
    url_map.emplace(ParserCommon::GetParserName(ParserCommon::SBF_PARSER) + "2", std::string(SBF_LONG_URL));

    std::shared_ptr<Parser> parser;

    if (opts.source == ParserCommon::AO3_PARSER)
    {
        parser = ParserCast(std::make_shared<ParserAO3>());
    }
    else if (opts.source == ParserCommon::RR_PARSER)
    {
        parser = ParserCast(std::make_shared<ParserRR>());
    }
    else if (opts.source == ParserCommon::SBF_PARSER)
    {
        parser = ParserCast(std::make_shared<ParserSBF>());
    }
    else
    {
        std::cout << "could not match parser source" << std::endl;
        Usage(argv[0]);
        exit(1);
    }

    auto iter = url_map.find(ParserCommon::GetParserName(parser->GetParserType()) + std::to_string(opts.length));

    if (iter == url_map.end())
    {
        std::cout << "could not match url" << std::endl;
        Usage(argv[0]);
        exit(1);
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    ParserJob parser_job;
    parser_job.url = iter->second;
    parser_job.uuid = "some-uuid";
    parser->Parse(parser_job);

    curl_global_cleanup();

    std::cout << "Tester exit successful" << std::endl;

    return 0;
}
