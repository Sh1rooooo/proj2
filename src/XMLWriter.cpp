#include "XMLWriter.h"
#include <sstream>
#include <iostream>


struct CXMLWriter::SImplementation {
    std::shared_ptr<CDataSink> DSink;
    std::vector<std::string> OpenElements;

    explicit SImplementation(std::shared_ptr<CDataSink> sink)
        : DSink(std::move(sink)) {}

    bool WriteEntity(const SXMLEntity &entity) {
        std::ostringstream OutputBuffer;

        switch (entity.DType) {
            case SXMLEntity::EType::StartElement:
                OutputBuffer << "<" << entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    OutputBuffer << " " << attr.first << "=\"" << Escape(attr.second) << "\"";
                }
                OutputBuffer << ">";
                OpenElements.push_back(entity.DNameData);
                break;

            case SXMLEntity::EType::EndElement:
                if (!OpenElements.empty() && OpenElements.back() == entity.DNameData) {
                    OpenElements.pop_back();
                    OutputBuffer << "</" << entity.DNameData << ">";
                }
                break;

            case SXMLEntity::EType::CharData:
                OutputBuffer << Escape(entity.DNameData);
                break;

            case SXMLEntity::EType::CompleteElement:
                OutputBuffer << "<" << entity.DNameData;
                for (const auto &attr : entity.DAttributes) {
                    OutputBuffer << " " << attr.first << "=\"" << Escape(attr.second) << "\"";
                }
                OutputBuffer << " />";
                break;
        }

        OutputBuffer << "\n"; 

        std::string OutputString = OutputBuffer.str();
        std::vector<char> CharBuffer(OutputString.begin(), OutputString.end());

        return DSink->Write(CharBuffer);
    }

    bool Flush() {
        while (!OpenElements.empty()) {
            std::string Tag = OpenElements.back();
            OpenElements.pop_back();

            std::ostringstream OutputBuffer;
            OutputBuffer << "</" << Tag << ">\n";

            std::string OutputString = OutputBuffer.str();
            std::vector<char> CharBuffer(OutputString.begin(), OutputString.end());

            if (!DSink->Write(CharBuffer)) {
                return false;
            }
        }
        return true;
    }

    std::string Escape(const std::string &text) const {
        std::ostringstream EscapedText;
        for (char ch : text) {
            switch (ch) {
                case '&': EscapedText << "&amp;"; break;
                case '<': EscapedText << "&lt;"; break;
                case '>': EscapedText << "&gt;"; break;
                case '"': EscapedText << "&quot;"; break;
                case '\'': EscapedText << "&apos;"; break;
                default: EscapedText << ch; break;
            }
        }
        return EscapedText.str();
    }
};

CXMLWriter::CXMLWriter(std::shared_ptr<CDataSink> sink)
    : DImplementation(std::make_unique<SImplementation>(std::move(sink))) {}

CXMLWriter::~CXMLWriter() = default;

bool CXMLWriter::WriteEntity(const SXMLEntity &entity) {
    return DImplementation->WriteEntity(entity);
}

bool CXMLWriter::Flush() {
    return DImplementation->Flush();
}
