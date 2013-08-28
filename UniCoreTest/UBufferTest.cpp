#include "stdafx.h"

#include "gtest/gtest.h"
#include "../UniCore/UBuffer.h"
#include <iostream>

using namespace std;
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


//UBuffer::data
TEST_F(UBufferTest,data_NormalString_DataRight)
{
    buffer_->appendString("str",3);
    const char *data = buffer_->data();
    if(data[0] == 's'
        && data[1] == 't'
        && data[2] == 'r')
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(UBufferTest,data_StringContainsZero_DataRight)
{
    buffer_->appendChar('A');
    buffer_->appendChar(0);
    buffer_->appendChar('B');
    buffer_->appendChar(0);
    buffer_->appendChar(0);
    buffer_->appendChar('C');
    buffer_->appendChar('D');
    const char *data = buffer_->data();
    if(data[0] == 'A'
        && data[1] == 0
        && data[2] == 'B'
        && data[3] == 0
        && data[4] == 0
        && data[5] == 'C'
        && data[6] == 'D')
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}



TEST_F(UBufferTest,size_NoDataAppended_Returns0)
{
    int size = buffer_->size();
    ASSERT_EQ(0,size);
}

TEST_F(UBufferTest,size_WithData_ReturnsDataSize)
{
    char content[] = "content in buffer \0\0 data after \0\0";

    buffer_->appendString(content,sizeof(content));
    int size = buffer_->size();
    int expectedSize = sizeof(content);
    ASSERT_EQ(expectedSize,size);
}

TEST_F(UBufferTest,appendChar_WithZero_DataRight)
{
    buffer_->appendChar(0);
    buffer_->appendChar(0xFF);
    buffer_->appendChar('A');
    buffer_->appendChar('a');
    const char *data = buffer_->data();
    int size = buffer_->size();
    EXPECT_EQ(4,size);
    EXPECT_EQ(0,data[0]);
    EXPECT_EQ('\xFF',data[1]);
    EXPECT_EQ('A',data[2]);
    EXPECT_EQ('a',data[3]);
    if(HasNonfatalFailure())
    {
        FAIL();
    }
    else
    {
        SUCCEED();
    }
}

TEST_F(UBufferTest,appendShort_DataRight)
{
    short shortNumber1 = 6000;
    short shortNumber2 = -1;
    buffer_->appendShort(shortNumber1);
    buffer_->appendShort(shortNumber2);
    const char *data = buffer_->data();
    EXPECT_EQ(4,buffer_->size());
    short shortNumber1InBuffer = *(short *)(data+0);
    short shortNumber2InBuffer = *(short *)(data+2);
    EXPECT_EQ(shortNumber1,shortNumber1InBuffer);
    EXPECT_EQ(shortNumber2,shortNumber2InBuffer);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(UBufferTest,appendInt_DataRight)
{
    buffer_->appendInt(-1);
    const char *data = buffer_->data();
    int actualData = *(int *)data;
    ASSERT_EQ(-1,actualData);
}

TEST_F(UBufferTest,appendHexPattern_SingleChar_dataRight)
{
    //"A" treated as "0A".
    buffer_->appendHexPattern("A");
    const char *data = buffer_->data();
    ASSERT_EQ(0x0A,data[0]);
}


TEST_F(UBufferTest,appendHexPattern_SingleChar_sizeValid)
{
    buffer_->appendHexPattern("A");
    ASSERT_EQ(1,buffer_->size());
}


TEST_F(UBufferTest,appendHexPattern_EmptyString_sizeEqualsToZero)
{
    buffer_->appendHexPattern("");
    ASSERT_EQ(0,buffer_->size());
}

TEST_F(UBufferTest,appendHexPattern_StringWithPunctuation_DataRight)
{
    buffer_->appendHexPattern("FD#435da^&C8");
    const char *data = buffer_->data();
    EXPECT_EQ(5,buffer_->size());
    EXPECT_EQ('\xFD',data[0]);
    EXPECT_EQ('\x43',data[1]);
    EXPECT_EQ('\x5D',data[2]);
    EXPECT_EQ('\x0A',data[3]);
    EXPECT_EQ('\xC8',data[4]);
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(UBufferTest,appendHexPattern_OnePunctuationChar_NoData)
{
    buffer_->appendHexPattern(";");
    ASSERT_EQ(0,buffer_->size());
}

TEST_F(UBufferTest,appendHexPattern_LettersNotHexNumber_DataValid)
{
    buffer_->appendHexPattern("AFGGCSDVH");
    const char *data = buffer_->data();
    EXPECT_EQ('\xAF',data[0]);
    EXPECT_EQ('\xC',data[1]);
    EXPECT_EQ('\xD',data[2]);
    EXPECT_EQ(3,buffer_->size());
    if(!HasNonfatalFailure())
    {
        SUCCEED();
    }
    else
    {
        FAIL();
    }
}

TEST_F(UBufferTest,appendString_CStrDataContainsZero_DataRight)
{
    char testData[] = {0x25,0x23,0x00,0x00,'C','B','A'};
    buffer_->appendString(testData,sizeof(testData));
    const char *data = buffer_->data();
    if(data[0] == 0x25
        && data[1] == 0x23
        && data[2] == 0x00
        && data[3] == 0x00
        && data[4] == 'C'
        && data[5] == 'B'
        && data[6] == 'A')
    {
        SUCCEED();
    }
    else
    {
        FAIL()<<"data:      "<<hex<<showbase<<data[0]<<data[1]<<data[2]
        <<data[3]<<data[4]<<data[5]<<data[6]<<"\r\ndata expected:   "
        <<testData[0]<<testData[1]<<testData[2]
        <<testData[3]<<testData[4]<<testData[5]<<testData[6];
    }
}

