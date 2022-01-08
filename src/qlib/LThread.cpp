// -*-Mode: C++;-*-
//
//  Thread object
//

#include <common.h>

#include "LThread.hpp"

#include <future>
#include <thread>

namespace qlib {
class LTThreadImpl
{
public:
    std::thread *m_pthr;
    std::promise<bool> m_exitOk;
};
}  // namespace qlib

using namespace qlib;

LThread::LThread()
{
    m_pimp = new LTThreadImpl();
    m_pimp->m_pthr = nullptr;
}

LThread::~LThread()
{
    if (m_pimp->m_pthr) delete m_pimp->m_pthr;
    delete m_pimp;
}

void LThread::kick()
{
    MB_ASSERT(m_pimp->m_pthr == nullptr);
    // m_pimp->m_pthr = MB_NEW boost::thread(boost::bind(&LThread::run, this));
    m_pimp->m_pthr = MB_NEW std::thread([this] {
        this->run();
        this->m_pimp->m_exitOk.set_value(true);
    });
}

void LThread::waitTermination()
{
    MB_DPRINTLN("wait termination ...");
    if (m_pimp->m_pthr != nullptr) {
        m_pimp->m_pthr->join();
        delete m_pimp->m_pthr;
        m_pimp->m_pthr = nullptr;
    }
    MB_DPRINTLN("wait termination OK.");
}

// TODO: test impl using std::promise/future
bool LThread::waitTermination(int nsec)
{
    MB_DPRINTLN("wait (%d sec) termination ...", nsec);

    if (m_pimp->m_pthr == nullptr) {
        MB_DPRINTLN("OK (thread is not running).");
        return true;
    }

    std::future<bool> f = m_pimp->m_exitOk.get_future();
    std::future_status result = f.wait_for(std::chrono::seconds(nsec));
    // if (m_pimp->m_pthr->timed_join(boost::posix_time::seconds(nsec)))
    if (result == std::future_status::timeout) {
        // timeout
        return false;
    } else {
        // OK
        m_pimp->m_pthr->join();
        delete m_pimp->m_pthr;
        m_pimp->m_pthr = nullptr;
        return true;
    }
}

// TODO: test impl using std::promise/future
bool LThread::isRunning() const
{
    if (m_pimp->m_pthr == nullptr) {
        MB_DPRINTLN("thread is not running.");
        return false;
    }

    std::future<bool> f = m_pimp->m_exitOk.get_future();
    std::future_status result = f.wait_for(std::chrono::seconds(0));
    if (result == std::future_status::timeout) {
        // timeout (running)
        return true;
    } else {
        return false;
    }
}
