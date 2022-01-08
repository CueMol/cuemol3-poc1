// -*-Mode: C++;-*-
//
//  log class
//   (length of string is limited to TagStr::MAX_LEN)
//
#include <common.h>

#include "LMsgLog.hpp"
#include "LLogEvent.hpp"
#include "LVarArgs.hpp"
#include "LScrCallBack.hpp"
#include "LUnicode.hpp"

#ifdef WIN32
#  include <windows.h>
#endif

#include "MthrEventCaster.hpp"

namespace qlib {

  class LLogEventCaster
    : public LMthrEventCaster<LLogEvent, LLogEventListener>
  {
    virtual void execute(LLogEvent &ev, LLogEventListener *p)
    {
      p->logAppended(ev);
    }
  };
  
  class LMsgLogImpl
  {
  public:
      LString m_filepath;
      FILE *m_fp;
      LLogEventCaster m_evcaster;
      LString m_accumMsg;
      bool m_bAccumMsg;
  };
}

using namespace qlib;


LMsgLog::LMsgLog()
    : m_pImpl( new LMsgLogImpl() )
{
    // printf("LMsgLog::LMsgLog()\n");
    resetOutput();
    m_pImpl->m_bAccumMsg = true;
    // printf("LMsgLog::LMsgLog() OK\n");
}

void LMsgLog::resetOutput()
{
#if defined(WIN32)
  HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
  //printf("hStdErr=%p\n", hStdErr);
  if (hStdErr) {
    // has console --> output to stderr
    m_pImpl->m_fp = stderr;
  }
  else {
    m_pImpl->m_fp = NULL;
  }
#else
  // posix: Always write msgs to stderr
  m_pImpl->m_fp = stderr;
#endif

}

LMsgLog::~LMsgLog()
{
  delete m_pImpl;
}

void LMsgLog::init()
{
    // printf("void LMsgLog::init\n");
    s_pLog = MB_NEW LMsgLog();
    // printf("void LMsgLog::init OK\n");
}

void LMsgLog::fini()
{
  delete s_pLog;
  s_pLog = NULL;
}

LMsgLog *LMsgLog::s_pLog;

///////////////////////////////////////////

void LMsgLog::setRedirect(FILE *fp)
{
  m_pImpl->m_fp = fp;
}

void LMsgLog::setFileRedirPath(const LString &path)
{
  // close the current stream
  if (!m_pImpl->m_filepath.isEmpty() && m_pImpl->m_fp)
    ::fclose(m_pImpl->m_fp);
  m_pImpl->m_filepath = "";

  if (path.isEmpty()) {
    // reset to default
    resetOutput();
    return;
  }
  else {
#ifdef _WIN32
    m_pImpl->m_fp = qlib::fopen_utf8(path.c_str(), "wb");
#else
    m_pImpl->m_fp = qlib::fopen_utf8(path.c_str(), "w");
#endif
    if (m_pImpl->m_fp==NULL) {
      MB_THROW(IOException, ("Cannot open file:"+path));
    }
  }    
}

LString LMsgLog::getFileRedirPath() const
{
    return m_pImpl->m_filepath;
}

int LMsgLog::addListener(LLogEventListener *plsn)
{
    return 0;
    // return m_pImpl->m_evcaster.add(plsn);
}

bool LMsgLog::removeListener(int nid)
{
  // if (m_pImpl->m_evcaster.remove(nid)==NULL)
  //   return false;
  return true;
}

void LMsgLog::writeLog(int nlev, const char *msg, bool bNL /*=false*/ )
{
  if (m_pImpl->m_fp) {
    fputs(msg, m_pImpl->m_fp);
    if (bNL)
      fputc('\n', m_pImpl->m_fp);
    fflush(m_pImpl->m_fp);
  }

#ifdef WIN32
  OutputDebugStringA(msg);
  if (bNL)
    OutputDebugStringA("\n");
#endif

  if (nlev>LMsgLog::DL_WARN) return;

  if (m_pImpl->m_bAccumMsg) {
    m_pImpl->m_accumMsg += msg;
    if (bNL)
      m_pImpl->m_accumMsg += "\n";
    return;
  }
  
  // prevent the re-entrance
  if (m_pImpl->m_evcaster.isLocked())
    return;

  LLogEvent evt(nlev, bNL, msg);
  m_pImpl->m_evcaster.lockedFire(evt);
}

LString LMsgLog::getAccumMsg() const
{
  return m_pImpl->m_accumMsg;
}


void LMsgLog::removeAccumMsg()
{
  m_pImpl->m_accumMsg = "";
  m_pImpl->m_bAccumMsg = false;
}

