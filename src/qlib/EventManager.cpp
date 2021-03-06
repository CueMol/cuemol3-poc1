//
// Event manager
//
// $Id: EventManager.cpp,v 1.2 2010/09/12 12:52:38 rishitani Exp $

#include <common.h>

#include "EventManager.hpp"

#include <thread>

#include "LEvent.hpp"
#include "LExceptions.hpp"

namespace qlib {

struct EMThreadImpl
{
    std::thread::id m_mainthr;
    mutable std::mutex m_mu;
};

SINGLETON_BASE_IMPL(EventManager);

}  // namespace qlib

using namespace qlib;

EventManager::EventManager()
{
    m_pthr = NULL;
    m_pImpl = NULL;
    m_pthr = new EMThreadImpl();

    m_pthr->m_mainthr = std::this_thread::get_id();
}

EventManager::~EventManager()
{
    delete m_pthr;
}

bool EventManager::isMainThread() const
{
    if (m_pthr->m_mainthr != std::this_thread::get_id()) {
        return false;
    } else {
        return true;
    }
}

void EventManager::delegateEventFire(const LEvent *pEvent, LEventCasterBase *pCaster)
{
    std::scoped_lock lk(m_pthr->m_mu);
    m_pending.push_back(tuple_t(static_cast<LEvent *>(pEvent->clone()), pCaster));
}

void EventManager::messageLoop()
{
    std::scoped_lock lk(m_pthr->m_mu);

    while (m_pending.size() > 0) {
        tuple_t tup = m_pending.front();
        m_pending.pop_front();

        LEvent *pEvent = tup.first;
        LEventCasterBase *pCaster = tup.second;

        pCaster->fireEvent(pEvent);
        delete pEvent;
    }
}

////////////////////////////////////////////////////

void EventManager::setTimer(TimerListener *pobj, qlib::time_value dur_msec)
{
    qlib::time_value curr = m_pImpl->getCurrentTime();
    TimerTuple tt;
    tt.start = curr;
    tt.end = curr + dur_msec;
    tt.pobj = pobj;
    m_timerq.push_back(tt);

    // timerEntryMethod();
    // checkQueueAndSetupTimer(false);
}

void EventManager::removeTimer(TimerListener *pobj)
{
    TimerQueue::iterator iter = m_timerq.begin();
    for (; iter != m_timerq.end();) {
        const TimerTuple &rtt = *iter;
        if (rtt.pobj == pobj) {
            MB_DPRINTLN("TimerListener %p removed from queue", pobj);
            iter = m_timerq.erase(iter);
            continue;
        }
        ++iter;
    }
}

void EventManager::initTimer(TimerImpl *pimpl)
{
    MB_ASSERT(m_pImpl == NULL);
    m_pImpl = pimpl;

    // m_pImpl->start();
}

void EventManager::finiTimer()
{
    m_pImpl->stop();

    if (m_pImpl != NULL) delete m_pImpl;
    m_pImpl = NULL;
}

void EventManager::checkTimerQueue()
{
    if (m_timerq.empty()) return;
    qlib::time_value curr = m_pImpl->getCurrentTime();
    MB_DPRINTLN("EventManager::checkTimerQueue() curr=%ld", curr);

    TimerQueue::iterator iter = m_timerq.begin();
    // TimerQueue::iterator eiter = m_timerq.end();
    // for (; iter!=eiter;) {

    for (; iter != m_timerq.end();) {
        const TimerTuple &rtt = *iter;
        TimerListener *pobj = rtt.pobj;
        qlib::time_value dur_end = rtt.end - curr;
        if (dur_end <= 0) {
            // process ended timer (last event)
            iter = m_timerq.erase(iter);
            pobj->onTimer(1.0, curr, true);
            continue;
        } else {
            // process active timer
            double rho = double(curr - rtt.start) / double(rtt.end - rtt.start);
            if (!pobj->onTimer(rho, curr, false)) {
                // timer iteration is canceled
                iter = m_timerq.erase(iter);
                MB_DPRINTLN("EvtMgr> timer canceled");
                continue;
            }
        }
        ++iter;
    }
}

//////////

TimerImpl::~TimerImpl() {}

// #ifdef HAVE_BOOST_CHRONO
// #include <boost/chrono/chrono.hpp>
// #endif
#include <chrono>

qlib::time_value TimerImpl::getCurrentTime()
{
    auto tp = std::chrono::high_resolution_clock::now();
    // time_value is in nano-sec rep with int64 precision
    qlib::time_value t1 =
        std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch())
            .count();
    return t1;

    // #ifdef HAVE_BOOST_CHRONO
    //     using namespace boost::chrono;

    //     high_resolution_clock::time_point tp = high_resolution_clock::now();

    //     // time_value is in nano-sec rep with int64 precision
    //     qlib::time_value t1 =
    //     duration_cast<nanoseconds>(tp.time_since_epoch()).count();

    //     // LOG_DPRINTLN("getCurrentTime() = %llu", t1);
    //     return t1;
    // #else
    //     return qlib::time_value(0);
    // #endif
}

//////////

#include "LPerfMeas.hpp"

IdleTask::~IdleTask() {}

void EventManager::performIdleTasks()
{
    try {
        qlib::AutoPerfMeas apm(PM_IDLE_TIMER);

        // process events
        messageLoop();

        // process timer events
        checkTimerQueue();

        BOOST_FOREACH (IdleTask *pTask, m_idleTasks) {
            pTask->doIdleTask();
        }
    } catch (qlib::LException &e) {
        LOG_DPRINTLN("Exception occured in performIdleTask: %s", e.getFmtMsg().c_str());
        throw;
    } catch (std::exception &e) {
        LOG_DPRINTLN("Exception occured in performIdleTask: %s", e.what());
        throw;
    } catch (...) {
        LOG_DPRINTLN("Unknown exception occured in performIdleTask");
        throw;
    }
}
