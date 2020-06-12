//
// Created by stefano on 08/06/20.
//

#include "Row.h"

Row::Row(const std::string &fileName, int lineNumber, const std::string &lineContent) : file_name(fileName),
                                                                                        line_number(lineNumber),
                                                                                        line_content(lineContent) {}
const std::string &Row::getFileName() const {
    return file_name;
}

int Row::getLineNumber() const {
    return line_number;
}

const std::string &Row::getLineContent() const {
    return line_content;
}

