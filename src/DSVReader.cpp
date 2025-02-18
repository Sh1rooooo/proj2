#include "DSVReader.h"
#include <sstream>
#include <iostream>
#include <vector>

struct CDSVReader::SImplementation {
    std::shared_ptr<CDataSource> DSource;
    char DDelimiter;

    SImplementation(std::shared_ptr<CDataSource> src, char delimiter)
        : DSource(std::move(src)), DDelimiter(delimiter) {}

    bool ReadRow(std::vector<std::string>& row) {
        row.clear();
        if (!DSource || DSource->End()) {
            return false;
        }

        std::string currentField;
        bool inQuotes = false;
        char ch;

        while (!DSource->End()) {
            if (!DSource->Get(ch)) {
                break;
            }

            if (ch == '"') { 
                char nextChar;
                if (DSource->Peek(nextChar) && nextChar == '"') {
                    DSource->Get(nextChar); 
                    currentField += '"';
                } else {
                    inQuotes = !inQuotes; 
                }
            } else if (ch == DDelimiter && !inQuotes) {
                row.push_back(currentField);
                currentField.clear();
            } else if (ch == '\n' && !inQuotes) {
                row.push_back(currentField);
                return true; 
            } else {
                currentField += ch;
            }
        }
        row.push_back(currentField);
        return true;
    }
};

CDSVReader::CDSVReader(std::shared_ptr<CDataSource> src, char delimiter)
    : DImplementation(std::make_unique<SImplementation>(std::move(src), delimiter)) {}

CDSVReader::~CDSVReader() = default;

bool CDSVReader::End() const {
    return DImplementation->DSource->End();
}

bool CDSVReader::ReadRow(std::vector<std::string>& row) {
    return DImplementation->ReadRow(row);
}
