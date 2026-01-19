# Install script for directory: F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/Assimp")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.5-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/lib/Debug/assimp-vc143-mtd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/lib/Release/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/lib/MinSizeRel/assimp-vc143-mt.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/lib/RelWithDebInfo/assimp-vc143-mt.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libassimp5.2.5" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/bin/Debug/assimp-vc143-mtd.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/bin/Release/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/bin/MinSizeRel/assimp-vc143-mt.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/bin/RelWithDebInfo/assimp-vc143-mt.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/anim.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/aabb.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ai_assert.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/camera.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/color4.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/color4.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/config.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ColladaMetaData.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/commonMetaData.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/defs.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/cfileio.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/light.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/material.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/material.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/matrix3x3.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/matrix3x3.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/matrix4x4.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/matrix4x4.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/mesh.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ObjMaterial.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/pbrmaterial.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/GltfMaterial.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/postprocess.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/quaternion.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/quaternion.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/scene.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/metadata.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/texture.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/types.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/vector2.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/vector2.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/vector3.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/vector3.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/version.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/cimport.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/AssertHandler.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/importerdesc.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Importer.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/DefaultLogger.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ProgressHandler.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/IOStream.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/IOSystem.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Logger.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/LogStream.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/NullLogger.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/cexport.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Exporter.hpp"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/DefaultIOStream.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/DefaultIOSystem.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SceneCombiner.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/fast_atof.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/qnan.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/BaseImporter.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Hash.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ParsingUtils.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/StreamReader.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/StreamWriter.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/StringComparison.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/StringUtils.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SGSpatialSort.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/GenericProperty.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SpatialSort.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SmallVector.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SmoothingGroups.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/SmoothingGroups.inl"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/StandardShapes.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/RemoveComments.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Subdivision.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Vertex.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/LineSplitter.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/TinyFormatter.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Profiler.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/LogAux.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Bitmap.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/XMLTools.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/IOStreamBuffer.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/CreateAnimMesh.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/XmlParser.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/BlobIOSystem.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/MathFunctions.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Exceptional.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/ByteSwapper.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Base64.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "assimp-dev" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Compiler/poppack1.h"
    "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/Debug/assimp-vc143-mtd.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/RelWithDebInfo/assimp-vc143-mt.pdb")
  endif()
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "F:/Hazel3D/Hazel3D/Hazel/vendor/assimp/code/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
