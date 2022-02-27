/**
 * ParserFFN.cc
 */
#include <iostream>

#include <FileOperations.h>
#include <LogOperations.h>
#include <TimeOperations.h>

#include <ParserFFN.h>

namespace black_library {

namespace core {

namespace parsers {

namespace FFN {

namespace BlackLibraryCommon = black_library::core::common;

ParserFFN::ParserFFN() : IndexEntryParser(parser_t::FFN_PARSER)
{
    source_url_ = BlackLibraryCommon::FFN::source_url;
    network_ = std::make_shared<SeleniumAdapter>(10);
}

ParserIndexEntry ParserFFN::ExtractIndexEntry(xmlNodePtr root_node) 
{
    (void) root_node;
    ParserIndexEntry index_entry;
    return index_entry;
}

void ParserFFN::FindIndexEntries(xmlNodePtr root_node) 
{
    ParserXmlNodeSeek select_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "select", 
                                        pattern_seek_t::XML_ATTRIBUTE, "id=chap_select");
    if (!select_seek.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed select seek for UUID: {}", uuid_);
        return;
    }

    uint16_t index_num = 0;
    xmlNodePtr current_node = NULL;
    for (current_node = select_seek.seek_node->children; current_node; current_node = current_node->next) 
    {
        ParserIndexEntry index_entry;
        std::stringstream ss;
        ss << target_url_ << index_num + 1;
        index_entry.data_url = ss.str();
        index_entry.index_num = index_num;

        ParserXmlContentResult name_content_result = GetXmlNodeContent(current_node);
        if (name_content_result.found)
        {
            std::string option_content = name_content_result.result;
            index_entry.name = option_content.substr(option_content.find(".") + 1);
        }


        index_entries_.emplace_back(index_entry);
        ++index_num;
    }
}

void ParserFFN::FindMetaData(xmlNodePtr root_node) 
{
    ParserXmlNodeSeek profile_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "div", 
                                        pattern_seek_t::XML_ATTRIBUTE, "id=profile_top");
    if (!profile_seek.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed profile seek for UUID: {}", uuid_);
        return;
    }
    ParserXmlNodeSeek title_seek = SeekToNodeByPattern(profile_seek.seek_node, pattern_seek_t::XML_NAME, "b", 
                                        pattern_seek_t::XML_ATTRIBUTE, "class=xcontrast_txt");
    if (title_seek.found) 
    {
        ParserXmlContentResult title_content_result = GetXmlNodeContent(title_seek.seek_node);
        if (title_content_result.found)
        {
            title_ = title_content_result.result;
        }
    }
    ParserXmlNodeSeek author_seek = SeekToNodeByPattern(profile_seek.seek_node, pattern_seek_t::XML_NAME, "a", 
                                        pattern_seek_t::XML_ATTRIBUTE, "class=xcontrast_txt");
    if (author_seek.found) 
    {
        ParserXmlContentResult author_content_seek = GetXmlNodeContent(author_seek.seek_node);
        if (author_content_seek.found)
        {
            author_ = author_content_seek.result;
        }
    }
}

ParseSectionInfo ParserFFN::ParseSection() {
    ParseSectionInfo output;
    const auto index_entry = index_entries_[index_];
    const auto index_entry_url = "https://" + source_url_ + index_entry.data_url;
    BlackLibraryCommon::LogDebug(parser_name_, "ParseSection: {} section_url: {} - {}", GetParserBehaviorName(parser_behavior_), index_entry_url, index_entry.name);

    const auto curl_request_result = network_.get()->NetworkRequest(index_entry_url);
    if (curl_request_result.has_error)
    {
        return output;
    }

    xmlDocPtr section_doc_tree = htmlReadDoc((xmlChar*) curl_request_result.html.c_str(), NULL, NULL,
    HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    if (section_doc_tree == NULL)
    {
        BlackLibraryCommon::LogError(parser_name_, "Unable to parse into doc_tree for UUID: {}", uuid_);
        return output;
    }

    xmlNodePtr root_node = xmlDocGetRootElement(section_doc_tree);

    ParserXmlNodeSeek content_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "div", 
                                        pattern_seek_t::XML_ATTRIBUTE, "id=storytextp");
    if (!content_seek.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Unable to find story content for chapter: {}", index_entry.name);
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    ParserXmlContentResult story_content_result = GetXmlNodeContent(content_seek.seek_node);
    if (story_content_result.found)
    {
        output.length = story_content_result.result.length();
    }

    const auto sanatized_section_name = BlackLibraryCommon::SanitizeFileName(index_entry.name);

    const auto section_file_name = GetSectionFileName(index_entry.index_num, sanatized_section_name);

    FILE* section_output_file;
    std::string file_path = local_des_ + section_file_name;
    BlackLibraryCommon::LogDebug(parser_name_, "FILEPATH: {}", file_path);
    section_output_file = fopen(file_path.c_str(), "w+");

    if (section_output_file == NULL)
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed to open file with path: {}", file_path);
        xmlFreeDoc(section_doc_tree);
        return output;
    }

    xmlElemDump(section_output_file, section_doc_tree, content_seek.seek_node);
    fclose(section_output_file);

    xmlFreeDoc(section_doc_tree);
    output.has_error = false;
    return output;
}

std::string ParserFFN::PreprocessTargetUrl(const ParserJob &parser_job) 
{
    std::vector<std::string> tokens;
    std::stringstream ss;
    std::string intermediate;

    ss << parser_job.url;

    while (getline(ss, intermediate, '/'))
    {
        tokens.push_back(intermediate);
    }

    if (tokens.size() < 5) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed preprocess url - less than 5 parts for UUID: {}", uuid_);
        return parser_job.url;
    }

    return "https://" + source_url_ + "/" + tokens[3] + "/" + tokens[4] + "/";

}

ParserTimeResult ParserFFN::GetPublishedTime(xmlNodePtr root_node) {
    ParserTimeResult result;

    ParserXmlNodeSeek profile_seek = SeekToNodeByPattern(root_node, pattern_seek_t::XML_NAME, "div", 
                                        pattern_seek_t::XML_ATTRIBUTE, "id=profile_top");
    if (!profile_seek.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed profile seek for UUID: {}", uuid_);
        return result;
    }

    ParserXmlNodeSeek span_seek = SeekToNodeByPattern(profile_seek.seek_node, pattern_seek_t::XML_NAME, "span", 
                                        pattern_seek_t::XML_ATTRIBUTE, "class=xgray xcontrast_txt");
    if (!span_seek.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed span seek for UUID: {}", uuid_);
        return result;
    }

    ParserXmlNodeSeek time_seek = SeekToNodeByPattern(span_seek.seek_node, pattern_seek_t::XML_NAME, "span");
    if (!time_seek.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed time seek for UUID: {}", uuid_);
        return result;
    }

    auto time_result = GetXmlAttributeContentByName(time_seek.seek_node, "data-uxtime");

    if (!time_result.found) 
    {
        BlackLibraryCommon::LogError(parser_name_, "Failed time content seek for UUID: {}", uuid_);
        return result;
    }

    time_t time_struct(std::stol(time_result.result) * 1000);
    result.time = time_struct;
    result.found = true;

    return result;
}

} // namespace FFN

} // namespace parsers
} // namespace core
} // namespace black_library
/*
https://www.fanfiction.net/s/13805484/1/Clone-Effect
https://www.fanfiction.net/s/13805484/1/
https://www.fanfiction.net/s/13805484/1
https://www.fanfiction.net/s/13805484/
https://www.fanfiction.net/s/13805484
*/
