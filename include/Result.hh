#ifndef __LIBRARYCORE_RESULT_H__
#define __LIBRARYCORE_RESULT_H__

#include <stdlib.h>
#include <string>

namespace librarycore {

class Result
{
public:
    Result(int code, std::string message);
    Result(int code) : Result(code, ""){};
    Result(std::string message) : Result(0, message){};
    Result() : Result(0, ""){};
    ~Result();

    void SetCode(int code);
    void SetMessage(std::string message);

    int GetCode();
    std::string GetMessage();

protected:

private:
    int code_;
    std::string message_;

    bool is_error_;
};

} // namespace librarycore

#endif
