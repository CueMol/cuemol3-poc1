// -*-Mode: C++;-*-
//
//  CPK molecular renderer class (version 2)
//

#include <common.h>
#include "molvis.hpp"
#include <gfx/SphereSet.hpp>
#include <qsys/View.hpp>
#include <qsys/Scene.hpp>
#include <modules/molstr/AtomIterator.hpp>

#include "CPK2Renderer.hpp"

#include <gfx/DrawAttrArray.hpp>

#ifdef USE_OPENGL
#include <sysdep/OglDisplayContext.hpp>
#include <sysdep/OglProgramObject.hpp>
#include "GLSLSphereHelper.hpp"
#endif

using namespace molvis;
using namespace molstr;

CPK2Renderer::CPK2Renderer()
{
  m_pDrawElem = NULL;
  m_bUseShader = false;
  m_bCheckShaderOK = false;
  m_nGlRendMode = REND_DEFAULT;

#ifdef USE_OPENGL
  m_pSlSph = MB_NEW GLSLSphereHelper();
#endif
}

CPK2Renderer::~CPK2Renderer()
{
#ifdef USE_OPENGL
  delete m_pSlSph;
#endif
}

const char *CPK2Renderer::getTypeName() const
{
  return "cpk";
}

/////////

void CPK2Renderer::display(DisplayContext *pdc)
{
#ifndef USE_OPENGL
  super_t::display(pdc);
#else
  if (pdc->isFile()) {
    // case of the file (non-ogl) rendering
    // always use the old version.
    super_t::display(pdc);
    return;
  }

  if (!m_bCheckShaderOK) {
    if (m_pSlSph->initShader(this)) {
      MB_DPRINTLN("CPK2 sphere shader OK");
      m_bUseShader = true;
    }
    else {
      m_bUseShader = false;
    }
    m_bCheckShaderOK = true;
  }

  if (m_bUseShader &&
      (m_nGlRendMode==REND_DEFAULT ||
       m_nGlRendMode==REND_SHADER)) {
    // shader rendering mode
    if (m_pSlSph->getDrawElem()==NULL) {
      renderShaderImpl();
      if (m_pSlSph->getDrawElem()==NULL)
        return; // Error, Cannot draw anything (ignore)
    }
    
    preRender(pdc);
    m_pSlSph->draw(pdc);
    postRender(pdc);
  }
  else if (pdc->isDrawElemSupported() &&
           (m_nGlRendMode==REND_DEFAULT ||
            m_nGlRendMode==REND_VBO)) {
    // VBO rendering mode
    if (m_pDrawElem==NULL) {
      renderVBOImpl();
      if (m_pDrawElem==NULL)
	return; // Error, Cannot draw anything (ignore)
    }
    
    preRender(pdc);
    pdc->drawElem(*m_pDrawElem);
    postRender(pdc);
  }
  else {
    // old version (uses DisplayContext::sphere)
    super_t::display(pdc);
  }
#endif
}

void CPK2Renderer::invalidateDisplayCache()
{
  super_t::invalidateDisplayCache();
  
#ifdef USE_OPENGL
  if (m_pDrawElem!=NULL) {
    delete m_pDrawElem;
    m_pDrawElem = NULL;
  }
  if (m_bUseShader) {
    m_pSlSph->invalidate();
  }
#endif
}

void CPK2Renderer::unloading()
{
#ifdef USE_OPENGL
  if (m_pDrawElem!=NULL) {
    delete m_pDrawElem;
    m_pDrawElem = NULL;
  }
  if (m_bUseShader) {
    m_pSlSph->invalidate();
  }
#endif

  super_t::unloading();
}

double CPK2Renderer::getVdWRadius(MolAtomPtr pAtom)
{

  switch (pAtom->getElement()) {
  case ElemSym::H:
    return m_vdwr_H;

  case ElemSym::C:
    return m_vdwr_C;

  case ElemSym::N:
    return m_vdwr_N;
    
  case ElemSym::O:
    return m_vdwr_O;
    
  case ElemSym::S:
    return m_vdwr_S;
    
  case ElemSym::P:
    return m_vdwr_P;
    
  default:
    return m_vdwr_X;
  }
}

void CPK2Renderer::propChanged(qlib::LPropEvent &ev)
{
  if (ev.getName().equals("detail")) {
    invalidateDisplayCache();
  }
  else if (ev.getName().startsWith("vdwr_")) {
    invalidateDisplayCache();
  }
  else if (ev.getParentName().equals("coloring")||
      ev.getParentName().startsWith("coloring.")) {
    invalidateDisplayCache();
  }

  MolAtomRenderer::propChanged(ev);
}

/////////

bool CPK2Renderer::isRendBond() const
{
  return false;
}

void CPK2Renderer::rendBond(DisplayContext *pdl, MolAtomPtr pAtom1, MolAtomPtr pAtom2, MolBond *pMB)
{
}

void CPK2Renderer::beginRend(DisplayContext *pdl)
{
  m_nDetailOld = pdl->getDetail();
  setupDetail(pdl, m_nDetail);
}

void CPK2Renderer::endRend(DisplayContext *pdl)
{
  pdl->setDetail(m_nDetailOld);
}

void CPK2Renderer::rendAtom(DisplayContext *pdl, MolAtomPtr pAtom, bool)
{
  pdl->color(ColSchmHolder::getColor(pAtom));
  pdl->sphere(getVdWRadius(pAtom), pAtom->getPos());
}

/////////////////////
// VBO implementation

void CPK2Renderer::renderVBOImpl()
{
  MolCoordPtr pMol = getClientMol();
  if (pMol.isnull()) {
    MB_DPRINTLN("CPK2Renderer::render> Client mol is null");
    return;
  }

  // estimate the size of drawing elements
  int nsphs=0;
  {
    AtomIterator iter(pMol, getSelection());
    for (iter.first(); iter.hasMore(); iter.next()) {
      int aid = iter.getID();
      MolAtomPtr pAtom = pMol->getAtom(aid);
      if (pAtom.isnull()) continue; // ignore errors
      ++nsphs;
    }
  }
  
  if (nsphs==0)
    return; // nothing to draw
  
  // initialize the coloring scheme
  getColSchm()->start(pMol, this);
  pMol->getColSchm()->start(pMol, this);


  gfx::SphereSetTmpl<gfx::VBOSphereSet_traits> sphs2;

  //gfx::SphereSet sphs;
  //sphs.create(nsphs, m_nDetail);
  sphs2.getdata().create(nsphs, m_nDetail);
  if (!useShaderAlpha()) {
    //sphs.setAlpha(getDefaultAlpha());
    sphs2.getdata().setAlpha(getDefaultAlpha());
  }

  // build meshes / DrawElemVNCI
  {
    AtomIterator iter(pMol, getSelection());
    int i=0;
    for (iter.first(); iter.hasMore(); iter.next()) {
      int aid = iter.getID();
      MolAtomPtr pAtom = pMol->getAtom(aid);
      if (pAtom.isnull()) continue; // ignore errors

      //sphs.sphere(i, pAtom->getPos(),
      //getVdWRadius(pAtom),
      //ColSchmHolder::getColor(pAtom));

      sphs2.getdata().sphere(i, pAtom->getPos(),
                             getVdWRadius(pAtom),
                             ColSchmHolder::getColor(pAtom));

      ++i;
    }
  }

  //m_pDrawElem = sphs.buildDrawElem();
  m_pDrawElem = sphs2.getdata().buildDrawElem(&sphs2);

  // finalize the coloring scheme
  getColSchm()->end();
  pMol->getColSchm()->end();

}

//////////////////////
// GLSL implementation

#ifdef USE_OPENGL
void CPK2Renderer::renderShaderImpl()
{
  MolCoordPtr pMol = getClientMol();
  if (pMol.isnull()) {
    MB_DPRINTLN("CPK2Renderer::render> Client mol is null");
    return;
  }

  // estimate the size of drawing elements
  int nsphs=0;
  {
    AtomIterator iter(pMol, getSelection());
    for (iter.first(); iter.hasMore(); iter.next()) {
      int aid = iter.getID();
      MolAtomPtr pAtom = pMol->getAtom(aid);
      if (pAtom.isnull()) continue; // ignore errors
      ++nsphs;
    }
  }
  
  if (nsphs==0)
    return; // nothing to draw
  
  // initialize the coloring scheme
  getColSchm()->start(pMol, this);
  pMol->getColSchm()->start(pMol, this);

  m_pSlSph->alloc(nsphs);

  {
    AtomIterator iter(pMol, getSelection());
    int i=0; //, j, ifc=0;
    Vector4D pos;
    for (iter.first(); iter.hasMore(); iter.next()) {
      int aid = iter.getID();
      MolAtomPtr pAtom = pMol->getAtom(aid);
      if (pAtom.isnull()) continue; // ignore errors

      m_pSlSph->setData(i, pAtom->getPos(), getVdWRadius(pAtom), ColSchmHolder::getColor(pAtom), getSceneID());
      ++i;
    }
  }

  // finalize the coloring scheme
  getColSchm()->end();
  pMol->getColSchm()->end();

}
#endif

