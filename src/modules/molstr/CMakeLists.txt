include_directories(
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/src
  ${Boost_INCLUDE_DIRS}
  )

# Normal C++ source files
SET(MOLSTR_SRCS
molstr.cpp
TraceRenderer.cpp
MainChainRenderer.cpp
ColoringScheme.cpp
CPKColoring.cpp
BfacColoring.cpp
SimpleRenderer.cpp
MolAtomRenderer.cpp
SelCommand.cpp
SelNodes.cpp
SelCacheMgr.cpp
SelAroundImpl.cpp
# SelAroundImpl2.cpp
SelCompiler.cpp
BondIterator.cpp
TopoBuilder.cpp
ResidIterator.cpp
ChainIterator.cpp
ResiPatch.cpp
ResiToppar.cpp
ParamDB.cpp
TopoDB.cpp
CnsParFile.cpp
XMLTopparFile.cpp
TopparManager.cpp
ElemSym.cpp
MolAtom.cpp
MolBond.cpp
MolResidue.cpp
MolChain.cpp
MolCoord.cpp
MolCoordGeomImpl.cpp
AtomIterator.cpp
PDBFileReader.cpp
PDBFileWriter.cpp
NuclBasePair.cpp
MolRenderer.cpp
NameLabelRenderer.cpp
SelectionRenderer.cpp
Prot2ndry.cpp
AtomPosMap.cpp
# AtomPosMap2.cpp
MolArrayMap.cpp
MolXformEditInfo.cpp
RectSelDrawObj.cpp
QdfPdbReader.cpp
ResidRangeSet.cpp
QdfMolReader.cpp
QdfMolWriter.cpp
Evaluator.cpp
  )

##########
# CueMol interface definitions for MCWRAPGEN

SET(MOLSTR_MCWG_QIFS
TraceRenderer.qif
MainChainRenderer.qif
CPKColoring.qif
BfacColoring.qif
ColoringScheme.qif
SolidColoring.qif
SimpleRenderer.qif
SelCommand.qif
MolSelection.qif
ResidIterator.qif
ChainIterator.qif
AtomIterator.qif
MolAtom.qif
MolResidue.qif
MolChain.qif
MolCoord.qif
MolRenderer.qif
NameLabelRenderer.qif
SelectionRenderer.qif
PDBFileReader.qif
PDBFileWriter.qif
RectSelDrawObj.qif
QdfPdbReader.qif
ResidRangeSet.qif
  )

##########
# molselector parser

# parser_sel.yxx  \
BISON_TARGET(molstr_sel_parser
  parser_sel.yxx
  ${CMAKE_CURRENT_BINARY_DIR}/parser_sel.cpp
  #COMPILE_FLAGS "-p col"
  )
FLEX_TARGET(molstr_sel_scanner
  scanner_sel.lxx
  ${CMAKE_CURRENT_BINARY_DIR}/scanner_sel.cpp
#  COMPILE_FLAGS "-P col"
  )
ADD_FLEX_BISON_DEPENDENCY(molstr_sel_scanner molstr_sel_parser)

# message(STATUS "BISON_molstr_sel_parser_OUTPUTS: ${BISON_molstr_sel_parser_OUTPUTS}")
# message(STATUS "FLEX_molstr_sel_scanner_OUTPUTS: ${FLEX_molstr_sel_scanner_OUTPUTS}")
list(APPEND MOLSTR_SRCS ${BISON_molstr_sel_parser_OUTPUTS})
list(APPEND MOLSTR_SRCS ${FLEX_molstr_sel_scanner_OUTPUTS})
# message("MOLSTR_SRCS ${MOLSTR_SRCS}")

##########
# module loader generated by MCWG

SET(MOLSTR_MODLDR_SRC molstr.moddef)

MCWRAPGEN_CLASS(MOLSTR_SRCS ${MOLSTR_MCWG_QIFS})
MCWRAPGEN_MODULE(MOLSTR_SRCS ${MOLSTR_MODLDR_SRC} ${MOLSTR_MCWG_QIFS})

if (WIN32)
  add_definitions(-DMOLSTR_EXPORTS=1)
endif()

add_library(molstr SHARED ${MOLSTR_SRCS} ${MCWG_HEADERS})
# target_link_libraries(molstr PRIVATE qlib gfx qsys Boost::filesystem Boost::thread CGAL::CGAL)
target_link_libraries(molstr PRIVATE qlib gfx qsys)

MCWRAPGEN_PYWRAPPERS(molstr)
MCWRAPGEN_JS_WRAPPERS(molstr)

install(TARGETS molstr
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
