#pragma once

#include <qlib/EventManager.hpp>

namespace embr {

class EmTimerImpl : public qlib::TimerImpl
{
private:
public:
    EmTimerImpl() = default;
    virtual ~EmTimerImpl() = default;

    virtual void start(qlib::time_value nperiod) {}

    virtual void stop() {}
};

}  // namespace embr
