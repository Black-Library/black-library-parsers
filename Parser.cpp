#include "Parser.h"

Parser::Parser(std::string url) {
  this.url = url;
  parseUrl();
}

~Parser::Parser() {

}

Parser::setUrl(std::string url) {
  this.url = url;
}

Parser::setLocalFilePath(std::string local_des) {
  this.local_des = local_des;
}

Parser::parseUrl() {
  if(url.find(AO3::url) != std::string::npos) {
    source = AO3.name;
  }
}
