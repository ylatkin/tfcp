//======================================================================
// 2019 (c) Evgeny Latkin
// License: Apache 2.0 (http://www.apache.org/licenses/)
//======================================================================

#ifndef TEST_UTILS_H
#define TEST_UTILS_H
//======================================================================

#include <iostream>
#include <string>

namespace tfcp {

//----------------------------------------------------------------------
//
// GTest cannot pretty-print parameters of the char* or std::string type
// (for me this looks like the bug at the GTest)
//
// So we define the parameter type which enables GTest pretty-printing
//
//----------------------------------------------------------------------

class TypeName {
private:
    std::string m_name;
public:
    std::string get() { return m_name; }
    TypeName(const char name[]) { m_name = name; }
public:
    friend
        std::ostream& operator<< (std::ostream& out, const TypeName& typeName) {
        return out << typeName.m_name;
    }
};

} // namespace tfcp

//======================================================================
#endif // TEST_UTILS_H
