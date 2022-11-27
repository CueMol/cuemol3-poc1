#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <qlib/LRegExpr.hpp>
#include <string>

namespace node_jsbr {

namespace stdfs = std::filesystem;

class ShaderLoader
{
private:
    qlib::LRegExpr m_incl_re;
    int m_max_recur_lev;

public:
    ShaderLoader()
        : m_incl_re("^\\#include\\s*\"([_0-9a-zA-Z\\/\\.]+)\""), m_max_recur_lev(15)
    {
    }

    ShaderLoader(const std::string& incl_re) : m_incl_re(incl_re), m_max_recur_lev(15)
    {
    }

    std::string load(const std::string& path, int n_recur_lev = 0)
    {
        stdfs::path in_path(path);
        auto par_dir = in_path.parent_path();

        std::string fullSourceCode = "";
        std::ifstream file(path);

        if (!file.is_open()) {
            std::cerr << "ERROR: could not open the shader at: " << path << "\n"
                      << std::endl;
            return fullSourceCode;
        }

        std::string lineBuffer;
        while (std::getline(file, lineBuffer)) {
            if (m_incl_re.match(lineBuffer)) {
                // LOG_DPRINTLN(">>> include %s", lineBuffer.c_str());
                if (m_incl_re.getSubstrCount() != 2) {
                    LOG_DPRINTLN("XXX fatal error %d", m_incl_re.getSubstrCount());
                    return "";
                }
                auto substr = m_incl_re.getSubstr(1);
                auto incl_path = par_dir / substr.data();
                LOG_DPRINTLN(">>> include %s", substr.c_str());
                LOG_DPRINTLN(">>> include path %s", incl_path.string().c_str());
                if (!stdfs::exists(incl_path)) {
                    // ERROR!!
                    LOG_DPRINTLN("XXX file not found: %s", incl_path.string().c_str());
                    return "";
                }
                if (n_recur_lev > m_max_recur_lev) {
                    // ERROR!!
                    LOG_DPRINTLN("XXX recur lev exceeds: %d", m_max_recur_lev);
                    return "";
                }

                auto subsrc = load(incl_path.string(), n_recur_lev + 1);
                fullSourceCode += subsrc;
            } else {
                fullSourceCode += lineBuffer + '\n';
            }
        }

        file.close();
        return fullSourceCode;
    }
};

}  // namespace node_jsbr
