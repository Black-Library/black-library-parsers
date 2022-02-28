#include <Result.hh>

namespace librarycore {

Result::Result(int code, std::string message) {
    code_ = code;
    message_ = message;
}

Result::~Result() {

}

void Result::SetCode(int code) {
    code_ = code;
}

void Result::SetMessage(std::string message) {
    message_ = message;
}

int Result::GetCode() {
    return code_;
}

std::string Result::GetMessage() {
    return message_;
}

} // namespace librarycore
