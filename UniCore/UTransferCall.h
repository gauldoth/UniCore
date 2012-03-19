/*! \file UTransferCall.h
    \brief 用于转移调用函数的位置。

    \author uni
    \date 2011-11-11
*/
#ifndef UNICORE_UTRANSFER_CALL_H
#define UNICORE_UTRANSFER_CALL_H

#include <queue>

#define BOOST_DATE_TIME_NO_LIB
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

#define AUTO_LINK_LIB_NAME "UniCore"
#include "AutoLink.h"

//! 单例模式。
#define theTransferCallManager uni::TransferCallManager::GetInstance()

namespace uni
{

class TransferCallFunctor;

//! 转移调用管理器。
/*!
    该管理器用于维护调用队列，同步调用等。
*/
class TransferCallManager
{
public:
    static TransferCallManager &GetInstance();
    //! 将functor放入请求队列并等待处理完成。
    /*!
        该函数由TransferCall调用，用户不会用到。
    */
    void requestAndWait(TransferCallFunctor *functor);
    //! 处理队列中所有请求。
    void processRequests();
protected:
    TransferCallManager();
private:
    struct TransferCallRequest
    {
        TransferCallFunctor *functor;
    };
    TransferCallManager(const TransferCallManager &);
    void operator = (const TransferCallManager &);
    static TransferCallManager *s_manager;
    std::queue<TransferCallRequest> requests_;
    boost::interprocess::interprocess_mutex queueLock_;
    boost::interprocess::interprocess_mutex conditionLock_;
    boost::interprocess::interprocess_condition processingCondition_;
};

//! 基础的仿函数。
class TransferCallFunctor
{
public:
    virtual void operator()() = 0;
};

//! 保存0个参数函数的仿函数。
template<class F,class R>
class TransferCallFunctor_0 : public TransferCallFunctor
{
public:
    TransferCallFunctor_0(F f)
        :f_(f)
    {
    }
    virtual void operator()()
    {
        ret_ = f_();
    }
    R ret_;
private:
    F f_;
};

//! 0个参数函数版本。
template<class R>
R
TransferCall(R (*f)())
{
    typedef R(*F)();
    TransferCallFunctor_0<F,R> actualFunctor(f);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存1个参数函数的仿函数。
template<class F,class R,class A1>
class TransferCallFunctor_1 : public TransferCallFunctor
{
public:
    TransferCallFunctor_1(F f,A1 a1)
        :f_(f)
        ,a1_(a1)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
};

template<class F,class A1>
class TransferCallFunctor_1<F,void,A1> : public TransferCallFunctor
{
public:
    TransferCallFunctor_1(F f,A1 a1)
        :f_(f)
        ,a1_(a1)
    {
    }
    virtual void operator()()
    {
        f_(a1_);
    }
private:
    F f_;
    A1 a1_;
};

//! 1个参数函数版本。
template<class R,class B1,class A1>
R
TransferCall(R (*f)(B1),A1 a1)
{
    UTRACE("临时")<<delim<<f<<a1;
    typedef R(*F)(B1);
    TransferCallFunctor_1<F,R,A1> actualFunctor(f,a1);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

template<class B1,class A1>
void
TransferCall(void (*f)(B1),A1 a1)
{
    typedef void(*F)(B1);
    TransferCallFunctor_1<F,void,A1> actualFunctor(f,a1);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
}


//! 保存2个参数函数的仿函数。
template<class F,class R,class A1,class A2>
class TransferCallFunctor_2 : public TransferCallFunctor
{
public:
    TransferCallFunctor_2(F f,A1 a1,A2 a2)
        :f_(f),a1_(a1),a2_(a2)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
};

//! 2个参数函数版本。
template<class R,class B1,class B2,class A1,class A2>
R
TransferCall(R (*f)(B1,B2),A1 a1,A2 a2)
{
    typedef R(*F)(B1,B2);
    TransferCallFunctor_2<F,R,A1,A2> actualFunctor(f,a1,a2);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存3个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3>
class TransferCallFunctor_3 : public TransferCallFunctor
{
public:
    TransferCallFunctor_3(F f,A1 a1,A2 a2,A3 a3)
        :f_(f),a1_(a1),a2_(a2),a3_(a3)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
};

//! 3个参数函数版本。
template<class R,class B1,class B2,class B3,class A1,class A2,class A3>
R
TransferCall(R (*f)(B1,B2,B3),A1 a1,A2 a2,A3 a3)
{
    typedef R(*F)(B1,B2,B3);
    TransferCallFunctor_3<F,R,A1,A2,A3> actualFunctor(f,a1,a2,a3);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存4个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3,class A4>
class TransferCallFunctor_4 : public TransferCallFunctor
{
public:
    TransferCallFunctor_4(F f,A1 a1,A2 a2,A3 a3,A4 a4)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_,a4_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
};

//! 4个参数函数版本。
template<class R,class B1,class B2,class B3,class B4,
    class A1,class A2,class A3,class A4>
R
TransferCall(R (*f)(B1,B2,B3,B4),A1 a1,A2 a2,A3 a3,A4 a4)
{
    typedef R(*F)(B1,B2,B3,B4);
    TransferCallFunctor_4<F,R,A1,A2,A3,A4> actualFunctor(f,a1,a2,a3,a4);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存5个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3,class A4,class A5>
class TransferCallFunctor_5 : public TransferCallFunctor
{
public:
    TransferCallFunctor_5(F f,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_,a4_,a5_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
};

//! 5个参数函数版本。
template<class R,class B1,class B2,class B3,class B4,class B5,
    class A1,class A2,class A3,class A4,class A5>
R
TransferCall(R (*f)(B1,B2,B3,B4,B5),A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)
{
    typedef R(*F)(B1,B2,B3,B4,B5);
    TransferCallFunctor_5<F,R,A1,A2,A3,A4,A5> actualFunctor(f,a1,a2,a3,a4,a5);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存6个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3,class A4,class A5,class A6>
class TransferCallFunctor_6 : public TransferCallFunctor
{
public:
    TransferCallFunctor_6(F f,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_,a4_,a5_,a6_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
};

//! 6个参数函数版本。
template<class R,class B1,class B2,class B3,class B4,class B5,class B6,
    class A1,class A2,class A3,class A4,class A5,class A6>
R
TransferCall(R (*f)(B1,B2,B3,B4,B5,B6),A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6)
{
    typedef R(*F)(B1,B2,B3,B4,B5,B6);
    TransferCallFunctor_6<F,R,A1,A2,A3,A4,A5,A6> actualFunctor(f,a1,a2,a3,a4,a5,a6);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存7个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3,class A4,class A5,class A6,class A7>
class TransferCallFunctor_7 : public TransferCallFunctor
{
public:
    TransferCallFunctor_7(F f,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6),a7_(a7)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_,a4_,a5_,a6_,a7_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
    A7 a7_;
};

//! 7个参数函数版本。
template<class R,class B1,class B2,class B3,class B4,class B5,class B6,class B7,
class A1,class A2,class A3,class A4,class A5,class A6,class A7>
    R
    TransferCall(R (*f)(B1,B2,B3,B4,B5,B6,B7),A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7)
{
    typedef R(*F)(B1,B2,B3,B4,B5,B6,B7);
    TransferCallFunctor_7<F,R,A1,A2,A3,A4,A5,A6,A7> actualFunctor(f,a1,a2,a3,a4,a5,a6,a7);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存8个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8>
class TransferCallFunctor_8 : public TransferCallFunctor
{
public:
    TransferCallFunctor_8(F f,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6),a7_(a7),a8_(a8)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_,a4_,a5_,a6_,a7_,a8_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
    A7 a7_;
    A8 a8_;
};

//! 8个参数函数版本。
template<class R,class B1,class B2,class B3,class B4,class B5,class B6,class B7,class B8,
class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8>
    R
    TransferCall(R (*f)(B1,B2,B3,B4,B5,B6,B7,B8),A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8)
{
    typedef R(*F)(B1,B2,B3,B4,B5,B6,B7,B8);
    TransferCallFunctor_8<F,R,A1,A2,A3,A4,A5,A6,A7,A8> actualFunctor(f,a1,a2,a3,a4,a5,a6,a7,a8);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存9个参数函数的仿函数。
template<class F,class R,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
class TransferCallFunctor_9 : public TransferCallFunctor
{
public:
    TransferCallFunctor_9(F f,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6),a7_(a7),a8_(a8),a9_(a9)
    {
    }
    virtual void operator()()
    {
        ret_ = f_(a1_,a2_,a3_,a4_,a5_,a6_,a7_,a8_,a9_);
    }
    R ret_;
private:
    F f_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
    A7 a7_;
    A8 a8_;
    A9 a9_;
};

//! 9个参数函数版本。
template<class R,class B1,class B2,class B3,class B4,class B5,class B6,class B7,class B8,class B9,
class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    R
    TransferCall(R (*f)(B1,B2,B3,B4,B5,B6,B7,B8,B9),A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9)
{
    typedef R(*F)(B1,B2,B3,B4,B5,B6,B7,B8,B9);
    TransferCallFunctor_9<F,R,A1,A2,A3,A4,A5,A6,A7,A8,A9> actualFunctor(f,a1,a2,a3,a4,a5,a6,a7,a8,a9);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存0个参数成员函数的仿函数。
template<class F,class R,class Ptr>
class TransferCallFunctor_0mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_0mf(F f,Ptr ptr)
        :f_(f),ptr_(ptr)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)();
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
};

//! 0个参数成员函数版本。
template<class R,class T,class Ptr>
R
TransferCall(R (T::*f)(),Ptr p)
{
    typedef R(T::*F)();
    TransferCallFunctor_0mf<F,R,Ptr> actualFunctor(f,p);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存1个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1>
class TransferCallFunctor_1mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_1mf(F f,Ptr ptr,A1 a1)
        :f_(f)
        ,ptr_(ptr)
        ,a1_(a1)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
};

//! 1个参数成员函数版本。
template<class R,class T,class B1,class Ptr,class A1>
R
TransferCall(R (T::*f)(B1),Ptr p,A1 a1)
{
    typedef R(T::*F)(B1);
    TransferCallFunctor_1mf<F,R,Ptr,A1> actualFunctor(f,p,a1);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存2个参数函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2>
class TransferCallFunctor_2mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_2mf(F f,Ptr ptr,A1 a1,A2 a2)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
};

//! 2个参数函数版本。
template<class R,class T,class B1,class B2,class Ptr,class A1,class A2>
R
TransferCall(R (T::*f)(B1,B2),Ptr p,A1 a1,A2 a2)
{
    typedef R(T::*F)(B1,B2);
    TransferCallFunctor_2mf<F,R,Ptr,A1,A2> actualFunctor(f,p,a1,a2);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存3个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3>
class TransferCallFunctor_3mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_3mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2),a3_(a3)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
};

//! 3个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class Ptr,class A1,class A2,class A3>
R
TransferCall(R (T::*f)(B1,B2,B3),Ptr p,A1 a1,A2 a2,A3 a3)
{
    typedef R(T::*F)(B1,B2,B3);
    TransferCallFunctor_3mf<F,R,Ptr,A1,A2,A3> actualFunctor(f,p,a1,a2,a3);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存4个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3,class A4>
class TransferCallFunctor_4mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_4mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3,A4 a4)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2),a3_(a3),a4_(a4)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_,a4_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
};

//! 4个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class B4,
class Ptr,class A1,class A2,class A3,class A4>
    R
    TransferCall(R (T::*f)(B1,B2,B3,B4),Ptr p,A1 a1,A2 a2,A3 a3,A4 a4)
{
    typedef R(T::*F)(B1,B2,B3,B4);
    TransferCallFunctor_4mf<F,R,Ptr,A1,A2,A3,A4> actualFunctor(f,p,a1,a2,a3,a4);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存5个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3,class A4,class A5>
class TransferCallFunctor_5mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_5mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)
        :f_(f),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_,a4_,a5_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
};

//! 5个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class B4,class B5,
class Ptr,class A1,class A2,class A3,class A4,class A5>
    R
    TransferCall(R (T::*f)(B1,B2,B3,B4,B5),Ptr p,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5)
{
    typedef R(T::*F)(B1,B2,B3,B4,B5);
    TransferCallFunctor_5mf<F,R,Ptr,A1,A2,A3,A4,A5> actualFunctor(f,p,a1,a2,a3,a4,a5);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存6个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3,class A4,class A5,class A6>
class TransferCallFunctor_6mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_6mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_,a4_,a5_,a6_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
};

//! 6个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class B4,class B5,class B6,
class Ptr,class A1,class A2,class A3,class A4,class A5,class A6>
    R
    TransferCall(R (T::*f)(B1,B2,B3,B4,B5,B6),Ptr p,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6)
{
    typedef R(T::*F)(B1,B2,B3,B4,B5,B6);
    TransferCallFunctor_6mf<F,R,Ptr,A1,A2,A3,A4,A5,A6> actualFunctor(f,p,a1,a2,a3,a4,a5,a6);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存7个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3,class A4,class A5,class A6,class A7>
class TransferCallFunctor_7mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_7mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6),a7_(a7)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_,a4_,a5_,a6_,a7_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
    A7 a7_;
};

//! 7个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class B4,class B5,class B6,class B7,
class Ptr,class A1,class A2,class A3,class A4,class A5,class A6,class A7>
    R
    TransferCall(R (T::*f)(B1,B2,B3,B4,B5,B6,B7),Ptr p,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7)
{
    typedef R(T::*F)(B1,B2,B3,B4,B5,B6,B7);
    TransferCallFunctor_7mf<F,R,Ptr,A1,A2,A3,A4,A5,A6,A7> actualFunctor(f,p,a1,a2,a3,a4,a5,a6,a7);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存8个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8>
class TransferCallFunctor_8mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_8mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6),a7_(a7),a8_(a8)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_,a4_,a5_,a6_,a7_,a8_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
    A7 a7_;
    A8 a8_;
};

//! 8个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class B4,class B5,class B6,class B7,class B8,
class Ptr,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8>
    R
    TransferCall(R (T::*f)(B1,B2,B3,B4,B5,B6,B7,B8),Ptr p,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8)
{
    typedef R(T::*F)(B1,B2,B3,B4,B5,B6,B7,B8);
    TransferCallFunctor_8mf<F,R,Ptr,A1,A2,A3,A4,A5,A6,A7,A8> actualFunctor(f,p,a1,a2,a3,a4,a5,a6,a7,a8);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

//! 保存9个参数成员函数的仿函数。
template<class F,class R,class Ptr,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
class TransferCallFunctor_9mf : public TransferCallFunctor
{
public:
    TransferCallFunctor_9mf(F f,Ptr ptr,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9)
        :f_(f),ptr_(ptr),a1_(a1),a2_(a2),a3_(a3),a4_(a4),a5_(a5),a6_(a6),a7_(a7),a8_(a8),a9_(a9)
    {
    }
    virtual void operator()()
    {
        ret_ = (ptr_->*f_)(a1_,a2_,a3_,a4_,a5_,a6_,a7_,a8_,a9_);
    }
    R ret_;
private:
    F f_;
    Ptr ptr_;
    A1 a1_;
    A2 a2_;
    A3 a3_;
    A4 a4_;
    A5 a5_;
    A6 a6_;
    A7 a7_;
    A8 a8_;
    A9 a9_;
};

//! 9个参数成员函数版本。
template<class R,class T,class B1,class B2,class B3,class B4,class B5,class B6,class B7,class B8,class B9,
class Ptr,class A1,class A2,class A3,class A4,class A5,class A6,class A7,class A8,class A9>
    R
    TransferCall(R (T::*f)(B1,B2,B3,B4,B5,B6,B7,B8,B9),Ptr p,A1 a1,A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9)
{
    typedef R(T::*F)(B1,B2,B3,B4,B5,B6,B7,B8,B9);
    TransferCallFunctor_9mf<F,R,Ptr,A1,A2,A3,A4,A5,A6,A7,A8,A9> actualFunctor(f,p,a1,a2,a3,a4,a5,a6,a7,a8,a9);
    TransferCallFunctor *functor = &actualFunctor;
    theTransferCallManager.requestAndWait(functor);
    return actualFunctor.ret_;
}

}//namespace uni

#endif//UNICORE_UTRANSFER_CALL_H