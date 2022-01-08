// -*-Mode: C++;-*-
//
//  Test renderer
//
//  $Id: TestRenderer.hpp,v 1.10 2011/01/08 18:28:29 rishitani Exp $

#ifndef TEST_RENDERER_HPP_
#define TEST_RENDERER_HPP_

#include <gfx/DrawAttrArray.hpp>
#include <gfx/SolidColor.hpp>
#include <random_generator.hpp>

#include "Renderer.hpp"
#include "qsys.hpp"

namespace qsys {

class QSYS_API TestRenderer : public Renderer
{
    MC_SCRIPTABLE;
    MC_CLONEABLE;

private:
    typedef qsys::Renderer super_t;

    struct DrawAttr
    {
        float x, y, z;
        float r, g, b, a;
    };

    using DrawArray = gfx::DrawAttrArray<DrawAttr>;

    DrawArray *m_pDrawData;

    bool m_bUpdate;

public:

    gfx::ColorPtr m_col1;

    TestRenderer();
    TestRenderer(const TestRenderer &r) : Renderer(r) {}
    virtual ~TestRenderer();

    //////////////////////////////////////////////////////
    // Renderer implementation

    virtual void display(DisplayContext *pdc);
    virtual const char *getTypeName() const;
    virtual LString toString() const;
    virtual qlib::Vector4D getCenter() const;

    virtual bool isCompatibleObj(ObjectPtr pobj) const
    {
        return false;
    }

    virtual void unloading() {}

    virtual bool isHitTestSupported() const
    {
        return false;
    }

    // virtual bool isHitTestSupported() const { return true; }
    // virtual void displayHit(DisplayContext *pdc);
    // virtual LString interpHit(const gfx::RawHitData &rhit);

    static constexpr int VERTEX_NUMS = 6 * 10000;  // * 1000;
    static constexpr float scl = 1.0f;
    static constexpr float tri_size = 10.0;
    static constexpr float vertices_orig[] = {
        -tri_size, tri_size,  0.0, 1.0, 0.0, 0.0, 1.0,
        -tri_size, -tri_size, 0.0, 0.0, 1.0, 0.0, 1.0,
        tri_size,  tri_size,  0.0, 0.0, 0.0, 1.0, 1.0,
        -tri_size, -tri_size, 0.0, 0.0, 1.0, 0.0, 1.0,
        tri_size,  -tri_size, 0.0, 0.0, 0.0, 0.0, 1.0,
        tri_size,  tri_size,  0.0, 0.0, 0.0, 1.0, 1.0,
    };

    static inline float clamp(float min, float max, float val)
    {
        return std::min<float>(std::max<float>(min, val), max);
    }

    /// Random number generator
    gfx_render::RandomGenerator rng_;

    inline double randomUniform()
    {
        return rng_.randomUniform();
    }

    inline void initData()
    {
        const size_t elem_size = m_pDrawData->getElemSize() / sizeof(float);
        float *pbuf =
            const_cast<float *>(static_cast<const float *>(m_pDrawData->getData()));
        for (size_t i = 0; i < VERTEX_NUMS / 6; ++i) {
            const size_t bias = i * 6 * elem_size;
            for (int j = 0; j < 6 * elem_size; ++j) {
                pbuf[bias + j] = vertices_orig[j];
                // if (buffer_size <= bias + j) {
                //     printf("XXXXXXXXXXXXXXXXXXX buffer overrun\n");
                // }
            }
        }
        m_pDrawData->setUpdated(true);
    }

    inline void updateData()
    {
        for (size_t i = 0; i < VERTEX_NUMS; ++i) {
            m_pDrawData->at(i).x += randomUniform() * scl;
            m_pDrawData->at(i).y += randomUniform() * scl;
            m_pDrawData->at(i).z += randomUniform() * scl;

            m_pDrawData->at(i).r =
                clamp(0, 1, randomUniform() * 0.1 + m_pDrawData->at(i).r);
            m_pDrawData->at(i).g =
                clamp(0, 1, randomUniform() * 0.1 + m_pDrawData->at(i).g);
            m_pDrawData->at(i).b =
                clamp(0, 1, randomUniform() * 0.1 + m_pDrawData->at(i).b);
            // m_pDrawData->at(i).a =
            //     clamp(0, 1, randomUniform() * 0.1 + m_pDrawData->at(i).a);
        }
        m_pDrawData->setUpdated(true);
    }

    inline void setUpdate(bool b) { m_bUpdate = b; }
    inline bool isUpdate() const { return m_bUpdate; }
};

}  // namespace qsys

#endif
