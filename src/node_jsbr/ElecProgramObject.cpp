#include "ElecProgramObject.hpp"

#include <qlib/FileStream.hpp>

#include "ElecView.hpp"

namespace node_jsbr {

ElecProgramObject::~ElecProgramObject()
{
    // MB_DPRINTLN("OglProgramObj %d destroyed", m_progObjID);
    // glDeleteProgram(m_hPO);
}

bool ElecProgramObject::loadShaders(const qlib::MapTable<qlib::LString> &file_names)
{
    auto peer = m_pView->getPeerObj();
    auto env = peer.Env();
    auto args = Napi::Object::New(env);

    for (const auto &i : file_names) {
        printf("file: %s\n", i.second.c_str());
        qlib::LString src = loadFile(i.second);
        printf("src: %s\n", src.c_str());
        args.Set(i.first.c_str(), src.c_str());
    }

    auto method = peer.Get("createShader").As<Napi::Function>();
    auto rval =
        method.Call(peer, {Napi::String::New(env, getName().c_str()), args});

    // int shader_id = rval.As<Napi::Number>().Int32Value();
    // printf("shader ID: %d\n", shader_id);
    // m_progObjID = shader_id;
    bool result = rval.As<Napi::Boolean>().Value();

    return result;
}

qlib::LString ElecProgramObject::loadFile(const qlib::LString &filename)
{
    // SysConfig *pconf = SysConfig::getInstance();
    // LString fnam = pconf->convPathName(filename);
    qlib::LString fnam = filename;

    // read source file
    qlib::FileInStream fis;
    fis.open(fnam);
    char sbuf[1024];
    qlib::LString source;
    while (fis.ready()) {
        int n = fis.read(sbuf, 0, sizeof sbuf - 1);
        sbuf[n] = '\0';
        source += sbuf;
    }
    return source;
}

void ElecProgramObject::enable()
{
    auto peer = m_pView->getPeerObj();
    auto env = peer.Env();
    auto method = peer.Get("enableShader").As<Napi::Function>();
    method.Call(peer, {Napi::String::New(env, getName().c_str())});
    // printf("ElecProgramObject::enable %d OK\n", m_progObjID);
}

void ElecProgramObject::disable()
{
    auto peer = m_pView->getPeerObj();
    auto env = peer.Env();
    auto method = peer.Get("disableShader").As<Napi::Function>();
    method.Call(peer, {});
    // printf("ElecProgramObject::disable %d OK\n", m_progObjID);
}

// void ElecProgramObject::destroy()
// {
//     auto peer = m_pView->getPeerObj();
//     auto env = peer.Env();
//     auto method = peer.Get("deleteShader").As<Napi::Function>();
//     method.Call(peer, {Napi::String::New(env, getName().c_str())});
// }

}  // namespace node_jsbr
