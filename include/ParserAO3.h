 #ifndef PARSER_AO3_H
#define PARSER_AO3_H

#include "Parser.h"

namespace AO3 {

class ParserAO3 : public Parser {
public:
    ParserAO3();
    ~ParserAO3();

    void parse();
protected:
    void parseChapter();

private:
};

} // namespace AO3

#endif
