#pragma once

#include "atlbase.h"
#include "atlcom.h"

//! AutomationWrapper用于从c++代码使用Automation.
class AutomationWrapper
{
public:
    //! 从ProgId构造一个对象.
    explicit AutomationWrapper(const wchar_t *progId);
    //! 从IDispatch *构造对象.
    /*!
        析构时会释放IDispatch指针.
    */
    explicit AutomationWrapper(IDispatch *pDispatch);
    AutomationWrapper(const AutomationWrapper &rhs)
    {
        hr_ = rhs.hr_;
        pDispatch_ = rhs.pDispatch_;
        if(pDispatch_)
        {
            pDispatch_->AddRef();
        }
    }
    ~AutomationWrapper();

    bool IsValid()
    {
        return !FAILED(hr_);
    }

    CComVariant Get(BSTR propertyName);

    AutomationWrapper GetChild(BSTR propertyName);

    CComVariant Put(BSTR propertyName,CComVariant property);

    CComVariant Method(BSTR methodName)
    {
        return MethodV(methodName,0,0);
    }

    CComVariant Method(BSTR methodName,CComVariant param1)
    {
        CComVariant params[2] = {param1,0};
        return MethodV(methodName,params,1);
    }

    CComVariant Method(BSTR methodName,CComVariant param1,CComVariant param2)
    {
        CComVariant params[3] = {param1,param2,0};
        return MethodV(methodName,params,2);
    }
private:
    AutomationWrapper &operator = (const AutomationWrapper &other);
    HRESULT hr_;
    IDispatch *pDispatch_;

    CComVariant MethodV(BSTR methodName,CComVariant *params,int paramCount);
};
