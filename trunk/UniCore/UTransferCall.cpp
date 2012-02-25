#include "UTransferCall.h"

#include <boost/foreach.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "ULog.h"

using namespace std;
using namespace boost::interprocess;

namespace uni
{

TransferCallManager * TransferCallManager::s_manager = 0;

TransferCallManager & TransferCallManager::GetInstance()
{
    if(!s_manager)
    {
        s_manager = new TransferCallManager();
    }
    return *s_manager;
}


TransferCallManager::TransferCallManager()
{

}


void TransferCallManager::requestAndWait(TransferCallFunctor *functor)
{
    UTRACE<<"请求";
    TransferCallRequest request;
    request.functor = functor;

    {
    scoped_lock<interprocess_mutex> lock(queueLock_);
    requests_.push(request);
    }
    
    UTRACE<<"等待请求完成。";
    {
    scoped_lock<interprocess_mutex> lock(conditionLock_);
    processingCondition_.wait(lock);
    }
    UTRACE<<"请求完成。";
}

void TransferCallManager::processRequests()
{
    {
        scoped_lock<interprocess_mutex> lock(queueLock_);
        while(!requests_.empty())
        {
            UTRACE("临时")<<"处理函数。";
            (*(requests_.front().functor))();
            requests_.pop();
        }
    }

    processingCondition_.notify_all();
}

}//namespace uni