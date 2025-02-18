#include <gtest/gtest.h>
#include "DSVWriter.h"
#include "StringDataSink.h"

TEST(DSVWriterTest, WriteSimpleCSV) {
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row1 = {"Hello", "World", "Test"};
    std::vector<std::string> row2 = {"One", "Two", "Three"};

    EXPECT_TRUE(writer.WriteRow(row1));
    EXPECT_TRUE(writer.WriteRow(row2));

    std::string expected = "Hello,World,Test\nOne,Two,Three\n";
    EXPECT_EQ(sink->String(), expected);
}

TEST(DSVWriterTest, WriteQuotedValues) {
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row = {"Hello, World", "Value with \"quotes\"", "Normal"};

    EXPECT_TRUE(writer.WriteRow(row));

    std::string expected = "\"Hello, World\",\"Value with \"\"quotes\"\"\",Normal\n";
    EXPECT_EQ(sink->String(), expected);
}

TEST(DSVWriterTest, HandleEmptyFields) {
    auto sink = std::make_shared<CStringDataSink>();
    CDSVWriter writer(sink, ',');

    std::vector<std::string> row = {"a", "", "c"};
    std::vector<std::string> row2 = {"", "", ""};

    EXPECT_TRUE(writer.WriteRow(row));
    EXPECT_TRUE(writer.WriteRow(row2));

    std::string expected = "a,,c\n,,\n";
    EXPECT_EQ(sink->String(), expected);
}
