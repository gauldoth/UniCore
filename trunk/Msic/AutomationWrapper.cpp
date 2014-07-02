#include "AutomationWrapper.h"
#include <exception>
#include <stdexcept>
#include <locale.h>

void DebugMessage(const wchar_t *format,...)
{
	va_list ap;
	va_start(ap,format);
	//该函数返回格式化后的字符串长度，不包括0结束符。
	_locale_t loc = _create_locale(LC_ALL,"");
	const int len = _vscwprintf_p_l(format,loc,ap)+1;
	wchar_t *buffer = new wchar_t[len];

	_vswprintf_p_l(buffer,len,format,loc,ap);
	_free_locale(loc);
	va_end(ap);
	OutputDebugStringW(buffer);

	delete[] buffer;
}


using namespace std;

AutomationWrapper::AutomationWrapper( const wchar_t *progId ) 
	:hr_(S_OK)
    ,pDispatch_(0)
{
    CLSID clsid;
    hr_ = CLSIDFromProgID(progId,&clsid);
    if(hr_ != ERROR_SUCCESS)
    {
        DebugMessage(L"CLSIDFromProgID failed. hr:%ld progId:%ws",hr_,progId);
        throw runtime_error("CLSIDFromProgID failed.");
        return;
    }
    hr_ = ::CoCreateInstance(clsid,NULL,CLSCTX_SERVER,IID_IDispatch,(void **)&pDispatch_);
    if(hr_ != ERROR_SUCCESS)
    {
        DebugMessage(L"CoCreateInstance failed. hr:%ld",hr_);
        throw runtime_error("CoCreateInstance failed.");
        return;
    }
}

AutomationWrapper::AutomationWrapper( IDispatch *pDispatch ) :hr_(S_OK)
    ,pDispatch_(pDispatch)
{
}

AutomationWrapper::~AutomationWrapper()
{
    if(pDispatch_)
    {
        pDispatch_->Release();
        pDispatch_ = 0;
    }
}

ATL::CComVariant AutomationWrapper::Get( BSTR propertyName )
{
    CComVariant result;
    DISPID dispid = DISPID_UNKNOWN;
    hr_ = pDispatch_->GetIDsOfNames(IID_NULL,&propertyName,1,GetUserDefaultLCID(),&dispid);
    if(FAILED(hr_))
    {
        throw runtime_error("AutomationWrapper::Get  GetIDsOfNames failed.");
        return result;
    }

    DISPPARAMS dispParams = {0,NULL,0,0};
    CComExcepInfo excepInfo;

    hr_ = pDispatch_->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_PROPERTYGET,&dispParams,&result,&excepInfo,NULL);
    if(FAILED(hr_))
    {
        throw runtime_error("AutomationWrapper::Get  Invoke failed.");
        return result;
    }
    //结果为IDispatch *的时候,CComVariant会使计数少1.
    //这里手动增加IDispatch *的计数.
    if(result.vt == VT_DISPATCH)
    {
        result.pdispVal->AddRef();
    }
    return result;
}

AutomationWrapper AutomationWrapper::GetChild( BSTR propertyName )
{
    CComVariant result = Get(propertyName);
    return AutomationWrapper(result.pdispVal);
}

ATL::CComVariant AutomationWrapper::Put( BSTR propertyName,CComVariant property )
{
    CComVariant result;

    DISPID dispid = DISPID_UNKNOWN;
    hr_ = pDispatch_->GetIDsOfNames(IID_NULL,&propertyName,1,GetUserDefaultLCID(),&dispid);
    if(FAILED(hr_))
    {
        throw runtime_error("AutomationWrapper::Put  GetIDsOfNames failed.");
    }
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPPARAMS dispParams = {&property,&dispidNamed,1,1};
    CComExcepInfo excepInfo;
    hr_ = pDispatch_->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_PROPERTYPUT,&dispParams,&result,&excepInfo,NULL);
    if(FAILED(hr_))
    {
        throw runtime_error("AutomationWrapper::Put  Invoke failed.");
    }
    //结果为IDispatch *的时候,CComVariant会使计数少1.
    //这里手动增加IDispatch *的计数.
    if(result.vt == VT_DISPATCH)
    {
        result.pdispVal->AddRef();
    }
    return result;
}

ATL::CComVariant AutomationWrapper::MethodV( BSTR methodName,CComVariant *params,int paramCount )
{
    DISPID dispid = DISPID_UNKNOWN;
    hr_ = pDispatch_->GetIDsOfNames(IID_NULL,&methodName,1,GetUserDefaultLCID(),&dispid);
    if(FAILED(hr_))
    {
        throw runtime_error("AutomationWrapper::MethodV  GetIDsOfNames failed.");
    }
    DISPPARAMS dispParams = {params,NULL,paramCount,0};
    CComVariant result;
    CComExcepInfo excepInfo;
    hr_ = pDispatch_->Invoke(dispid,IID_NULL,GetUserDefaultLCID(),DISPATCH_METHOD,&dispParams,&result,&excepInfo,NULL);
    if(FAILED(hr_))
    {
        throw runtime_error("AutomationWrapper::MethodV  Invoke failed");
    }
    if(result.vt == VT_DISPATCH)
    {
        result.pdispVal->AddRef();
    }
    return result;
}
