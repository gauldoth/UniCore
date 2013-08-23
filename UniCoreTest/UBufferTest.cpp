#include "stdafx.h"

#include "gtest/gtest.h"
#include "../UniCore/UBuffer.h"

using namespace uni;

class UBufferTest : public ::testing::Test
{
public:
    UBufferTest() :buffer_(0){}
protected:
    void SetUp()
    {
        buffer_ = new UBuffer;
    }
    void TearDown()
    {
        delete buffer_;
    }
    UBuffer *buffer_;
};


TEST_F(UBufferTest,UBuffer_appendHexPattern_EmptyString_sizeEqualsZero)
{
    buffer_->appendHexPattern("");
    ASSERT_EQ(0,buffer_->size());
}

TEST_F(UBufferTest,UBuffer_appendHexPattern_SingleChar_dataValid)
{
    //"A" treated as "0A".
    buffer_->appendHexPattern("A");
    ASSERT_STRCASEEQ("\x0A",buffer_->data());
}

TEST_F(UBufferTest,UBuffer_appendHexPattern_SingleChar_sizeValid)
{
    buffer_->appendHexPattern("A");
    ASSERT_EQ(1,buffer_->size());
}

//不是太够.
//TEST_F(UBufferTest,UBuffer_appendHexPattern_OddChars_dataValid)
//{
//    buffer_->appendHexPattern("ABC");
//    ASSERT_STRCASEEQ("\xAB\xC",buffer_->data());
//}

//错误的测试.
// TEST_F(UBufferTest,UBuffer_data_NoData_ReturnsEmptyString)
// {
//     ASSERT_STRCASEEQ("",buffer_->data());
// }

TEST_F(UBufferTest,UBuffer_data_NormalString_ReturnsNormalString)
{
    buffer_->appendString("in side the moon.");
    ASSERT_STRCASEEQ("in side the moon.",buffer_->data());
}

TEST_F(UBufferTest,UBuffer_size_NoDataAppended_Returns0)
{
    int size = buffer_->size();
    ASSERT_EQ(0,size);
}

TEST_F(UBufferTest,UBuffer_size_WithData_ReturnsDataSize)
{
    const char *content = "content in buffer";
    buffer_->appendString(content);
    int size = buffer_->size();
    ASSERT_EQ(strlen(content),size);
}

//错误的测试
//TEST_F(UBufferTest,UBuffer_appendChar_SingleChar_DataCorrect)
//{
//    buffer_->appendChar('C');
//    ASSERT_STRCASEEQ("C",buffer_->data());
//}


