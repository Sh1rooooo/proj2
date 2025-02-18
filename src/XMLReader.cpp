#include "XMLReader.h"
#include <expat.h>
#include <iostream>
#include <vector>
#include <cstring>

struct CXMLReader::SImplementation {
    std::shared_ptr<CDataSource> DSource;
    XML_Parser DParser;
    bool DEndReached;
    std::vector<SXMLEntity> DEntityQueue;

    static void StartElementHandler(void *userData, const char *name, const char **atts) {
        auto *Impl = static_cast<SImplementation *>(userData);
        SXMLEntity Entity;
        Entity.DType = SXMLEntity::EType::StartElement;
        Entity.DNameData = name;

        for (size_t i = 0; atts[i]; i += 2) {
            Entity.DAttributes.emplace_back(atts[i], atts[i + 1]);
        }

        Impl->DEntityQueue.push_back(Entity);
    }

    static void EndElementHandler(void *userData, const char *name) {
        auto *Impl = static_cast<SImplementation *>(userData);
        SXMLEntity Entity;
        Entity.DType = SXMLEntity::EType::EndElement;
        Entity.DNameData = name;
        Impl->DEntityQueue.push_back(Entity);
    }

    static void CharacterDataHandler(void *userData, const char *data, int len) {
        auto *Impl = static_cast<SImplementation *>(userData);
        std::string CharData(data, len);
        
        if (CharData.find_first_not_of(" \t\n\r") == std::string::npos) {
            return;
        }

        SXMLEntity Entity;
        Entity.DType = SXMLEntity::EType::CharData;
        Entity.DNameData = CharData;
        Impl->DEntityQueue.push_back(Entity);
    }

    explicit SImplementation(std::shared_ptr<CDataSource> src)
        : DSource(std::move(src)), DEndReached(false) {
        DParser = XML_ParserCreate(nullptr);
        XML_SetUserData(DParser, this);
        XML_SetElementHandler(DParser, StartElementHandler, EndElementHandler);
        XML_SetCharacterDataHandler(DParser, CharacterDataHandler);
    }

    ~SImplementation() {
        XML_ParserFree(DParser);
    }

    bool ReadEntity(SXMLEntity &entity, bool skipcdata) {
        if (!DEntityQueue.empty()) {
            entity = DEntityQueue.front();
            DEntityQueue.erase(DEntityQueue.begin());

            if (skipcdata && entity.DType == SXMLEntity::EType::CharData) {
                return ReadEntity(entity, skipcdata);
            }
            return true;
        }

        if (!DSource || DSource->End()) {
            return false;
        }

        char Buffer[1024];
        std::vector<char> DataBuffer;
        while (!DSource->End() && DataBuffer.size() < sizeof(Buffer)) {
            char ch;
            if (DSource->Get(ch)) {
                DataBuffer.push_back(ch);
            }
        }

        if (DataBuffer.empty()) {
            return false;
        }

        bool ParseSuccess = XML_Parse(DParser, DataBuffer.data(), DataBuffer.size(), DSource->End()) != XML_STATUS_ERROR;
        return ParseSuccess ? ReadEntity(entity, skipcdata) : false;
    }

    bool End() const {
        return DSource->End() && DEntityQueue.empty();
    }
};

CXMLReader::CXMLReader(std::shared_ptr<CDataSource> src)
    : DImplementation(std::make_unique<SImplementation>(std::move(src))) {}

CXMLReader::~CXMLReader() = default;

bool CXMLReader::End() const {
    return DImplementation->End();
}

bool CXMLReader::ReadEntity(SXMLEntity &entity, bool skipcdata) {
    return DImplementation->ReadEntity(entity, skipcdata);
}
