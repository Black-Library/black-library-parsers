#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <iostream>
#include <string>

#include "SourceInformation.h"

class Parser {
public:
  Parser(std::string url);
  Parser() : Parser(""){};
  ~Parser();

  void setUrl(std::string url);
  void setLocalFilePath(std::string local_des);

protected:
  void parseUrl();

private:
  std::string source;
  std::string url;
  std::string local_des;

};
#endif
