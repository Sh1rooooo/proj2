#include <gtest/gtest.h>
#include "DSVReader.h"
#include "StringDataSource.h"

TEST(DSVReaderTest, ReadSimpleCSV) {
    std::string csvData = "Hello,World,Test\nOne,Two,Three\n";
    auto source = std::make_shared<CStringDataSource>(csvData);
    CDSVReader reader(source, ',');

    std::vector<std::string> row;
    
    EXPECT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "Hello");
    EXPECT_EQ(row[1], "World");
    EXPECT_EQ(row[2], "Test");

    EXPECT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "One");
    EXPECT_EQ(row[1], "Two");
    EXPECT_EQ(row[2], "Three");

    EXPECT_FALSE(reader.ReadRow(row)); // No more rows
}

TEST(DSVReaderTest, ReadQuotedValues) {
    std::string csvData = "\"Hello, World\",\"Value with \"\"quotes\"\"\",Normal\n";
    auto source = std::make_shared<CStringDataSource>(csvData);
    CDSVReader reader(source, ',');

    std::vector<std::string> row;
    EXPECT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "Hello, World");
    EXPECT_EQ(row[1], "Value with \"quotes\"");
    EXPECT_EQ(row[2], "Normal");

    EXPECT_FALSE(reader.ReadRow(row)); // No more rows
}

TEST(DSVReaderTest, HandleEmptyFields) {
    std::string csvData = "a,,c\n,,\n";
    auto source = std::make_shared<CStringDataSource>(csvData);
    CDSVReader reader(source, ',');

    std::vector<std::string> row;
    
    EXPECT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "a");
    EXPECT_EQ(row[1], "");
    EXPECT_EQ(row[2], "c");

    EXPECT_TRUE(reader.ReadRow(row));
    ASSERT_EQ(row.size(), 3);
    EXPECT_EQ(row[0], "");
    EXPECT_EQ(row[1], "");
    EXPECT_EQ(row[2], "");

    EXPECT_FALSE(reader.ReadRow(row)); // No more rows
}
