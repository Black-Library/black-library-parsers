/**
 * LinkedListParser.cc
 */

#include <functional>
#include <iostream>
#include <thread>

#include <LinkedListParser.h>
#include <ShortTimeGenerator.h>

namespace black_library {

namespace core {

namespace parsers {

namespace BlackLibraryCommon = black_library::core::common;

LinkedListParser::LinkedListParser(parser_t parser_type) : 
    Parser(parser_type),
    last_url_(),
    next_url_(),
    last_update_date_(0),
    reached_end_(false)
{
    parser_behavior_ = parser_behavior_t::LINKED_LIST;
}

void LinkedListParser::ParseLoop(ParserResult &parser_result)
{
    size_t seconds_counter = 0;
    size_t wait_time = 0;
    size_t wait_time_total = 0;
    size_t remaining_attempts = 5;

    while (!done_)
    {
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000);

        if (done_)
            break;

        if (seconds_counter >= wait_time)
            seconds_counter = 0;

        if (seconds_counter == 0)
        {
            // let the fake reader finish waiting before exiting
            if (reached_end_)
            {
                done_ = true;
                std::cout << GetParserName(parser_type_) << " - " << uuid_ << " reached end" << std::endl;
                continue;
            }

            if (remaining_attempts <= 0)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse - index: " << index_ << std::endl;
                remaining_attempts = 5;
                break;
            }

            ParseSectionInfo parse_section_info = ParseSection();
            --remaining_attempts;

            wait_time = time_generator_->GenerateWaitTime(parse_section_info.length);
            wait_time_total += wait_time;

            if (parse_section_info.has_error)
            {
                std::cout << "Error: " << GetParserName(parser_type_) << " failed to parse index: " << index_ << " - remaining attempts: " << remaining_attempts
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (remaining_attempts == 0 && progress_number_callback_)
                    progress_number_callback_(uuid_, index_ + 1, true);
            }
            else
            {
                std::cout << GetParserName(parser_type_) << ": " << title_ << " - " << index_ << " parse behavior info length is " << parse_section_info.length
                        << " - waiting " << wait_time << " seconds - wait time total: "  << wait_time_total << " seconds" << std::endl;

                if (progress_number_callback_)
                    progress_number_callback_(uuid_, index_ + 1, false);

                parser_result.metadata.series_length = index_ + 1;

                remaining_attempts = 5;
                ++index_;
            }
        }

        ++seconds_counter;

        std::this_thread::sleep_until(deadline);
    }
}

std::string LinkedListParser::PreprocessTargetUrl(const std::string &job_url)
{
    next_url_ = job_url;

    return job_url;
}

void LinkedListParser::SaveLastUrl(ParserResult &parser_result)
{
    parser_result.metadata.last_url = last_url_;
}

void LinkedListParser::SaveUpdateDate(ParserResult &parser_result)
{
    parser_result.metadata.update_date = last_update_date_;
}

} // namespace parsers
} // namespace core
} // namespace black_library
