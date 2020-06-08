//
// Created by stefano on 08/06/20.
//

#ifndef PDS_LAB4_ROW_H
#define PDS_LAB4_ROW_H

#include <iostream>
#include <string>

class Row {
private:
    std::string file_name;
    int line_number;
    std::string line_content;

public:
    Row(const std::string &fileName, int lineNumber, const std::string &lineContent);

    const std::string &getFileName() const;

    int getLineNumber() const;

    const std::string &getLineContent() const;
};


#endif //PDS_LAB4_ROW_H
