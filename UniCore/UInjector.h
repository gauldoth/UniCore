#ifndef UNICORE_UINJECTOR_H
#define UNICORE_UINJECTOR_H

#include "UCommon.h"

namespace uni
{

//! 注入器。
class UInjector
{
public:
    virtual void inject() = 0;
    virtual void setTarget(unsigned int pid) = 0;
private:
    DISALLOW_COPY_AND_ASSIGN(UInjector);
};

}//namespace uni

#endif//UNICORE_UINJECTOR_H