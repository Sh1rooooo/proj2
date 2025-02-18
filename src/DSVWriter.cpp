#include "DSVWriter.h"
#include <sstream>
#include <iostream>


struct CDSVWriter::SImplementation {
    std::shared_ptr<CDataSink> DSink;
    char DDelimiter;
    bool DQuoteAll;

    SImplementation(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
        : DSink(std::move(sink)), DDelimiter(delimiter), DQuoteAll(quoteall) {}

    bool WriteRow(const std::vector<std::string>& row) {
        if (!DSink) {
            return false;
        }

        std::ostringstream OutputBuffer;
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) {
                OutputBuffer << DDelimiter;
            }
            OutputBuffer << FormatField(row[i]);
        }

        std::string OutputString = OutputBuffer.str();
        std::vector<char> CharBuffer(OutputString.begin(), OutputString.end());
        CharBuffer.push_back('\n'); 

        return DSink->Write(CharBuffer);
    }

    std::string FormatField(const std::string& field) const {
        bool NeedsQuotes = DQuoteAll || field.find(DDelimiter) != std::string::npos ||
                           field.find('"') != std::string::npos || field.find('\n') != std::string::npos;

        if (!NeedsQuotes) {
            return field;
        }

        std::ostringstream QuotedField;
        QuotedField << '"';
        for (char ch : field) {
            if (ch == '"') {
                QuotedField << "\"\""; 
            } else {
                QuotedField << ch;
            }
        }
        QuotedField << '"';

        return QuotedField.str();
    }
};

CDSVWriter::CDSVWriter(std::shared_ptr<CDataSink> sink, char delimiter, bool quoteall)
    : DImplementation(std::make_unique<SImplementation>(std::move(sink), delimiter, quoteall)) {}

CDSVWriter::~CDSVWriter() = default;

bool CDSVWriter::WriteRow(const std::vector<std::string>& row) {
    return DImplementation->WriteRow(row);
}
