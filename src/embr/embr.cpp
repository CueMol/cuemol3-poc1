#include "embr.hpp"

#include "EmProgObjMgr.hpp"

extern void embr_regClasses();
extern void embr_unregClasses();

namespace embr {

bool init()
{
    embr_regClasses();
    EmProgObjMgr::init();
    return true;
}

void fini()
{
    EmProgObjMgr::fini();
    embr_unregClasses();
}

}  // namespace embr
