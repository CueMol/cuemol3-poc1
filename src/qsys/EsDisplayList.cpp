#include "EsDisplayList.hpp"

#include "View.hpp"

namespace qsys {

EsDisplayList::EsDisplayList()
    : m_pLineArray(nullptr),
      m_pTrigArray(nullptr),
      m_pTrigMesh(nullptr),
      m_fValid(false),
      m_nDrawMode(DRAWMODE_NONE),
      m_pColor(gfx::SolidColor::createRGB(0.5, 0.5, 0.5)),
      m_fPrevPosValid(false),
      m_prevCol(0)
{
    m_matstack.clear();
    pushMatrix();
    loadIdent();
}

EsDisplayList::~EsDisplayList()
{
    if (m_pLineArray) delete m_pLineArray;
    if (m_pTrigArray) delete m_pTrigArray;
}

qlib::uid_t EsDisplayList::getSceneID() const
{
    return getTargetView()->getSceneID();
}

void EsDisplayList::vertex(const Vector4D &aV)
{
    // printf("vert (%f,%f,%f)\n", aV.x(), aV.y(), aV.z());
    Vector4D v(aV);
    xform_vec(v);
    // printf("vert (%f,%f,%f)\n", v.x(), v.y(), v.z());

#ifdef MB_DEBUG
    if (!qlib::isFinite(v.x()) || !qlib::isFinite(v.y()) || !qlib::isFinite(v.z())) {
        LOG_DPRINTLN("ERROR: invalid vertex");
    }
#endif

    // printf("draw mode %d\n", m_nDrawMode);
    auto color_value = m_pColor->getDevCode(getSceneID());
    switch (m_nDrawMode) {
        default:
        case DRAWMODE_NONE:
            MB_DPRINTLN("vertex command ignored.");
            break;

        case DRAWMODE_LINES:
            if (!m_fPrevPosValid) {
                m_prevPos = v;
                m_prevCol = color_value;
                m_fPrevPosValid = true;
            } else {
                drawLine(v, color_value, m_prevPos, m_prevCol);
                m_fPrevPosValid = false;
            }
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_LINESTRIP:
            if (!m_fPrevPosValid) {
                m_prevPos = v;
                m_prevCol = color_value;
                m_fPrevPosValid = true;
                break;
            } else {
                drawLine(v, color_value, m_prevPos, m_prevCol);
                m_prevPos = v;
            }
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_TRIGS:
            addTrigVert(v, m_norm, color_value);
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_TRIGSTRIP:
            m_mesh.addVertex(v, m_norm, color_value);
            break;

            //////////////////////////////////////////////////////
        case DRAWMODE_TRIGFAN:
            m_mesh.addVertex(v, m_norm, color_value);
            // m_pIntData->meshVertex(v, m_norm, m_pColor);
            break;
    }
}

void EsDisplayList::normal(const Vector4D &av)
{
    Vector4D v(av);
    xform_norm(v);

#ifdef MB_DEBUG
    if (!qlib::isFinite(v.x()) || !qlib::isFinite(v.y()) || !qlib::isFinite(v.z())) {
        LOG_DPRINTLN("FileDC> ERROR: invalid mesh norm");
    }
#endif

    const double len = v.length();
    if (len < F_EPS4) {
        LOG_DPRINTLN("FileDisp> Normal vector <%f,%f,%f> is too small.", v.x(), v.y(),
                     v.z());
        m_norm = Vector4D(1.0, 0.0, 0.0);
        return;
    }
    m_norm = v.scale(1.0 / len);
}

void EsDisplayList::color(const gfx::ColorPtr &c)
{
    m_pColor = c;
}

void EsDisplayList::pushMatrix()
{
    if (m_matstack.size() <= 0) {
        Matrix4D m;
        m_matstack.push_front(m);
        return;
    }
    const Matrix4D &top = m_matstack.front();
    m_matstack.push_front(top);
}
void EsDisplayList::popMatrix()
{
    if (m_matstack.size() <= 1) {
        LString msg("POVWriter> FATAL ERROR: cannot popMatrix()!!");
        LOG_DPRINTLN(msg);
        MB_THROW(qlib::RuntimeException, msg);
        return;
    }
    m_matstack.pop_front();
}
void EsDisplayList::multMatrix(const qlib::Matrix4D &mat)
{
    Matrix4D top = m_matstack.front();
    top.matprod(mat);
    m_matstack.front() = top;

    // check unitarity
    // checkUnitary();
}
void EsDisplayList::loadMatrix(const qlib::Matrix4D &mat)
{
    m_matstack.front() = mat;

    // check unitarity
    // checkUnitary();
}

void EsDisplayList::setPolygonMode(int id)
{
    LOG_DPRINTLN("setPolygonMode is not supported (ignored.)");
}

void EsDisplayList::startPoints()
{
    m_nDrawMode = DRAWMODE_POINTS;
}

void EsDisplayList::startPolygon()
{
    LOG_DPRINTLN("polygon is not supported (vertex command ignored.)");
}

void EsDisplayList::startLines()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        printf("EsDisplayList::startLines ERR: %d\n", m_nDrawMode);
        MB_THROW(qlib::RuntimeException, "EsDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_LINES;
    // printf("EsDisplayList::startLines OK\n");
}

void EsDisplayList::startLineStrip()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "EsDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_LINESTRIP;
}

void EsDisplayList::startTriangles()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "EsDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_TRIGS;
    // if (m_nPolyMode == POLY_FILL || m_nPolyMode == POLY_FILL_NORGLN ||
    //     m_nPolyMode == POLY_FILL_XX)
    //     m_pIntData->meshStart(m_nDrawMode);
    // else if (m_nPolyMode == POLY_LINE) {
    //     m_nVertCnt = 0;
    //     m_vectmp.resize(3);
    // }
}

void EsDisplayList::startTriangleStrip()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "EsDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_TRIGSTRIP;
    m_mesh.start(gfx::GrowMesh::GM_TRIGSTRIP);
    // m_pIntData->meshStart(m_nDrawMode);
}

void EsDisplayList::startTriangleFan()
{
    if (m_nDrawMode != DRAWMODE_NONE) {
        MB_THROW(qlib::RuntimeException, "EsDisplayList: Unexpected condition");
        return;
    }
    m_nDrawMode = DRAWMODE_TRIGFAN;
    m_mesh.start(gfx::GrowMesh::GM_TRIGFAN);
    // m_pIntData->meshStart(m_nDrawMode);
}

void EsDisplayList::end()
{
    switch (m_nDrawMode) {
        case DRAWMODE_LINES:
        case DRAWMODE_LINESTRIP:
            m_fPrevPosValid = false;
            break;

        case DRAWMODE_TRIGS:
            // m_pIntData->meshEndTrigs();
            break;

        case DRAWMODE_TRIGFAN:
            m_mesh.end();
            // m_pIntData->meshEndFan();
            break;

        case DRAWMODE_TRIGSTRIP:
            m_mesh.end();
            // m_pIntData->meshEndTrigStrip();
            break;
    }
    m_nDrawMode = DRAWMODE_NONE;
}

void EsDisplayList::drawLine(const Vector4D &v1, qlib::quint32 c1, const Vector4D &v2,
                             qlib::quint32 c2)
{
    m_lineBuf.push_back(LineDrawAttr{float(v1.x()), float(v1.y()), float(v1.z()),
                                     float(v1.w()), float(gfx::getFR(c1)),
                                     float(gfx::getFG(c1)), float(gfx::getFB(c1)),
                                     float(gfx::getFA(c1))});

    m_lineBuf.push_back(LineDrawAttr{float(v2.x()), float(v2.y()), float(v2.z()),
                                     float(v2.w()), float(gfx::getFR(c2)),
                                     float(gfx::getFG(c2)), float(gfx::getFB(c2)),
                                     float(gfx::getFA(c2))});
}

void EsDisplayList::addTrigVert(const Vector4D &v1, const Vector4D &n1,
                                qlib::quint32 c1)
{
    m_trigBuf.push_back(TrigVertAttr{float(v1.x()), float(v1.y()), float(v1.z()),
                                     float(v1.w()), float(gfx::getFR(c1)),
                                     float(gfx::getFG(c1)), float(gfx::getFB(c1)),
                                     float(gfx::getFA(c1))});
}

bool EsDisplayList::recordStart()
{
    printf("EsDisplayList::recordStart called %p\n", m_pLineArray);
    if (m_pLineArray) {
        printf("delete %p\n", m_pLineArray);
        delete m_pLineArray;
        m_pLineArray = nullptr;
    }

    if (m_pTrigArray) {
        printf("delete %p\n", m_pTrigArray);
        delete m_pTrigArray;
        m_pTrigArray = nullptr;
    }

    m_fValid = false;

    m_matstack.clear();
    pushMatrix();
    loadIdent();

    return true;
}

void EsDisplayList::recordEnd()
{
    printf("EsDisplayList::recordEnd called %p\n", m_pLineArray);
    MB_ASSERT(m_pLineArray == nullptr);

    // Mark as valid
    m_fValid = true;

    // Create Line attr array
    const size_t nelems_line = m_lineBuf.size();
    printf("EsDisplayList::recordEnd nelems_line %zu\n", nelems_line);
    if (nelems_line > 0) {
        m_pLineArray = new LineDrawArray();
        m_pLineArray->setDrawMode(gfx::AbstDrawElem::DRAW_LINES);
        m_pLineArray->setAttrSize(2);
        m_pLineArray->setAttrInfo(0, DSLOC_VERT_POS, 4, qlib::type_consts::QTC_FLOAT32,
                                  offsetof(LineDrawAttr, x));
        m_pLineArray->setAttrInfo(1, DSLOC_VERT_COLOR, 4,
                                  qlib::type_consts::QTC_FLOAT32,
                                  offsetof(LineDrawAttr, r));
        m_pLineArray->alloc(nelems_line);

        size_t i = 0;
        for (const auto &elem : m_lineBuf) {
            MB_ASSERT(i < nelems);
            m_pLineArray->at(i) = elem;
            ++i;
        }
        m_pLineArray->setUpdated(true);
        m_lineBuf.clear();
    }

    // Create Trig attr array
    MB_ASSERT(m_pTrigArray == nullptr);
    const size_t nelems_trig = m_trigBuf.size();
    printf("EsDisplayList::recordEnd nelems_trig %zu\n", nelems_trig);
    if (nelems_trig > 0) {
        m_pTrigArray = new TrigVertArray();
        m_pTrigArray->setDrawMode(gfx::AbstDrawElem::DRAW_TRIANGLES);
        m_pTrigArray->setAttrSize(2);
        m_pTrigArray->setAttrInfo(0, DSLOC_VERT_POS, 4, qlib::type_consts::QTC_FLOAT32,
                                  offsetof(TrigVertAttr, x));
        m_pTrigArray->setAttrInfo(1, DSLOC_VERT_COLOR, 4,
                                  qlib::type_consts::QTC_FLOAT32,
                                  offsetof(TrigVertAttr, r));
        m_pTrigArray->alloc(nelems_trig);

        size_t i = 0;
        for (const auto &elem : m_trigBuf) {
            MB_ASSERT(i < nelems);
            m_pTrigArray->at(i) = elem;
            ++i;
        }
        m_pTrigArray->setUpdated(true);
        m_trigBuf.clear();
    }

    // Create Trig attr indexed array
    MB_ASSERT(m_pTrigMesh == nullptr);
    const size_t nMeshVerts = m_mesh.getVertexSize();
    const size_t nMeshFaces = m_mesh.getFaceSize();
    if (nMeshFaces > 0) {
        m_pTrigMesh = new TrigMesh();
        m_pTrigMesh->setDrawMode(gfx::AbstDrawElem::DRAW_TRIANGLES);
        m_pTrigMesh->setAttrSize(2);
        m_pTrigMesh->setAttrInfo(0, DSLOC_VERT_POS, 4, qlib::type_consts::QTC_FLOAT32,
                                 offsetof(TrigVertAttr, x));
        m_pTrigMesh->setAttrInfo(1, DSLOC_VERT_COLOR, 4, qlib::type_consts::QTC_FLOAT32,
                                 offsetof(TrigVertAttr, r));
        m_pTrigMesh->alloc(nMeshVerts);
        m_pTrigMesh->allocInd(nMeshFaces * 3);
        size_t i = 0;
        for (const auto *pelem : m_mesh.getVertexData()) {
            const auto &c1 = pelem->c;
            const auto &v1 = pelem->v;
            m_pTrigMesh->at(i) =
                TrigVertAttr{float(v1.x()),         float(v1.y()),
                             float(v1.z()),         1.0f,
                             float(gfx::getFR(c1)), float(gfx::getFG(c1)),
                             float(gfx::getFB(c1)), float(gfx::getFA(c1))};
            i++;
        }
        i = 0;
        for (const auto &elem : m_mesh.getFaceData()) {
            m_pTrigMesh->atind(i) = elem.iv1;
            m_pTrigMesh->atind(i + 1) = elem.iv2;
            m_pTrigMesh->atind(i + 2) = elem.iv3;
            i += 3;
        }
        m_pTrigMesh->setUpdated(true);
        m_mesh.clear();
    }
}

gfx::DisplayContext *EsDisplayList::createDisplayList()
{
    return nullptr;
}

bool EsDisplayList::canCreateDL() const
{
    return false;
}

bool EsDisplayList::isDisplayList() const
{
    return true;
}

}  // namespace qsys
