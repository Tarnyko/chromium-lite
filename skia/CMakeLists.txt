PROJECT(chromium-skia C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE, Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Skia")

# CMake modules
INCLUDE(FindThreads)
INCLUDE(FindPkgConfig)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PACKAGE(Threads)
PKG_CHECK_MODULES(freetype REQUIRED freetype2)
PKG_CHECK_MODULES(fontconfig REQUIRED fontconfig)
PKG_CHECK_MODULES(libpng12 REQUIRED libpng12)
#PKG_CHECK_MODULES(qt4 REQUIRED Qt)

CHECK_INCLUDE_FILES(gif_lib.h HAVE_LIBGIF_H)
CHECK_LIBRARY_EXISTS(gif DGifGetImageDesc "" HAVE_LIBGIF_SO)
IF(HAVE_LIBGIF_H)
  IF(HAVE_LIBGIF_SO)
    SET(HAVE_LIBGIF "1")
  ENDIF(HAVE_LIBGIF_SO)
ENDIF(HAVE_LIBGIF_H)
IF(NOT HAVE_LIBGIF)
  MESSAGE(FATAL_ERROR "\"gif_lib.h\" or \"libgif.so\" missing.
    Please install the \"giflib-devel\" or \"libgif-dev\" package !")
ENDIF(NOT HAVE_LIBGIF)

# Common definitions
ADD_DEFINITIONS(-DSK_INTERNAL=1 -DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1 -DSK_SUPPORT_GPU=1 -DSK_FORCE_DISTANCE_FIELD_TEXT=0)
ADD_DEFINITIONS(-DSK_GAMMA_SRGB -DSK_GAMMA_APPLY_TO_A8)
ADD_DEFINITIONS(-DSK_BUILD_FOR_UNIX -DSKIA_DLL -DSKIA_IMPLEMENTATION=1)

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DSK_DEVELOPER=1)
ELSE()
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

IF(CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS(-fno-exceptions -fstrict-aliasing -Wall -Wextra -Winit-self -Wpointer-arith -Wsign-compare -Wno-unused-parameter -Werror)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -fno-rtti -fno-threadsafe-statics -Wnon-virtual-dtor)
ENDIF()


# SOURCES FILE LIST, PER SECTION

# core
ADD_LIBRARY(core OBJECT src/c/sk_paint.cpp src/c/sk_surface.cpp src/core/SkAAClip.cpp src/core/SkAnnotation.cpp src/core/SkAdvancedTypefaceMetrics.cpp src/core/SkAlphaRuns.cpp src/core/SkBBHFactory.cpp src/core/SkBigPicture.cpp src/core/SkBitmap.cpp src/core/SkBitmapCache.cpp src/core/SkBitmapController.cpp src/core/SkBitmapDevice.cpp src/core/SkBitmapFilter.cpp src/core/SkBitmapHeap.cpp src/core/SkBitmapProcShader.cpp src/core/SkBitmapProcState.cpp src/core/SkBitmapProcState_matrixProcs.cpp src/core/SkBitmapProvider.cpp src/core/SkBitmapScaler.cpp src/core/SkBlitMask_D32.cpp src/core/SkBlitRow_D16.cpp src/core/SkBlitRow_D32.cpp src/core/SkBlitter.cpp src/core/SkBlitter_A8.cpp src/core/SkBlitter_ARGB32.cpp src/core/SkBlitter_RGB16.cpp src/core/SkBlitter_Sprite.cpp src/core/SkBuffer.cpp src/core/SkCachedData.cpp src/core/SkCanvas.cpp src/core/SkChecksum.cpp src/core/SkChunkAlloc.cpp src/core/SkClipStack.cpp src/core/SkColor.cpp src/core/SkColorFilter.cpp src/core/SkColorTable.cpp src/core/SkComposeShader.cpp src/core/SkConfig8888.cpp src/core/SkConvolver.cpp src/core/SkCubicClipper.cpp src/core/SkData.cpp src/core/SkDataTable.cpp src/core/SkDebug.cpp src/core/SkDeque.cpp src/core/SkDevice.cpp src/core/SkDeviceLooper.cpp src/core/SkDeviceProfile.cpp src/lazy/SkDiscardableMemoryPool.cpp src/lazy/SkDiscardablePixelRef.cpp src/core/SkDistanceFieldGen.cpp src/core/SkDither.cpp src/core/SkDraw.cpp src/core/SkDrawable.cpp src/core/SkDrawLooper.cpp src/core/SkEdgeBuilder.cpp src/core/SkEdgeClipper.cpp src/core/SkEdge.cpp src/core/SkError.cpp src/core/SkFilterProc.cpp src/core/SkFilterShader.cpp src/core/SkFlattenable.cpp src/core/SkFlattenableSerialization.cpp src/core/SkFloatBits.cpp src/core/SkFont.cpp src/core/SkFontHost.cpp src/core/SkFontMgr.cpp src/core/SkFontStyle.cpp src/core/SkFontDescriptor.cpp src/core/SkFontStream.cpp src/core/SkGeometry.cpp src/core/SkGlyphCache.cpp src/core/SkGraphics.cpp src/core/SkHalf.cpp src/core/SkImageFilter.cpp src/core/SkImageInfo.cpp src/core/SkImageCacherator.cpp src/core/SkImageGenerator.cpp src/core/SkLightingShader.cpp src/core/SkLineClipper.cpp src/core/SkLocalMatrixImageFilter.cpp src/core/SkLocalMatrixShader.cpp src/core/SkMask.cpp src/core/SkMaskCache.cpp src/core/SkMaskCache.cpp src/core/SkMaskFilter.cpp src/core/SkMaskGamma.cpp src/core/SkMath.cpp src/core/SkMatrix.cpp src/core/SkMatrixImageFilter.cpp src/core/SkMetaData.cpp src/core/SkMipMap.cpp src/core/SkMiniRecorder.cpp src/core/SkMultiPictureDraw.cpp src/core/SkNinePatchIter.cpp src/core/SkOpts.cpp src/core/SkPackBits.cpp src/core/SkPaint.cpp src/core/SkPaintPriv.cpp src/core/SkPath.cpp src/core/SkPathEffect.cpp src/core/SkPathMeasure.cpp src/core/SkPathRef.cpp src/core/SkPicture.cpp src/core/SkPictureContentInfo.cpp src/core/SkPictureData.cpp src/core/SkPictureFlat.cpp src/core/SkPictureImageGenerator.cpp src/core/SkPicturePlayback.cpp src/core/SkPictureRecord.cpp src/core/SkPictureRecorder.cpp src/core/SkPictureShader.cpp src/core/SkPixelRef.cpp src/core/SkPixmap.cpp src/core/SkPoint.cpp src/core/SkPoint3.cpp src/core/SkPtrRecorder.cpp src/core/SkQuadClipper.cpp src/core/SkRasterClip.cpp src/core/SkRasterizer.cpp src/core/SkReadBuffer.cpp src/core/SkRecord.cpp src/core/SkRecords.cpp src/core/SkRecordDraw.cpp src/core/SkRecordOpts.cpp src/core/SkRecorder.cpp src/core/SkRect.cpp src/core/SkRefDict.cpp src/core/SkRegion.cpp src/core/SkRegion_path.cpp src/core/SkRemote.cpp src/core/SkResourceCache.cpp src/core/SkRRect.cpp src/core/SkRTree.cpp src/core/SkRWBuffer.cpp src/core/SkScalar.cpp src/core/SkScalerContext.cpp src/core/SkScan.cpp src/core/SkScan_AntiPath.cpp src/core/SkScan_Antihair.cpp src/core/SkScan_Hairline.cpp src/core/SkScan_Path.cpp src/core/SkSemaphore.cpp src/core/SkShader.cpp src/core/SkSharedMutex.cpp src/core/SkSpinlock.cpp src/core/SkSpriteBlitter_ARGB32.cpp src/core/SkSpriteBlitter_RGB16.cpp src/core/SkStream.cpp src/core/SkString.cpp src/core/SkStringUtils.cpp src/core/SkStroke.cpp src/core/SkStrokeRec.cpp src/core/SkStrokerPriv.cpp src/core/SkTaskGroup.cpp src/core/SkTextBlob.cpp src/core/SkTime.cpp src/core/SkThreadID.cpp src/core/SkTLS.cpp src/core/SkTSearch.cpp src/core/SkTypeface.cpp src/core/SkTypefaceCache.cpp src/core/SkUnPreMultiply.cpp src/core/SkUtils.cpp src/core/SkValidatingReadBuffer.cpp src/core/SkVarAlloc.cpp src/core/SkVertState.cpp src/core/SkWriteBuffer.cpp src/core/SkWriter32.cpp src/core/SkXfermode.cpp src/core/SkXfermodeInterpretation.cpp src/core/SkYUVPlanesCache.cpp)

ADD_LIBRARY(core-ext OBJECT src/doc/SkDocument.cpp src/image/SkImage.cpp src/image/SkImage_Generator.cpp src/image/SkImage_Raster.cpp src/image/SkImageShader.cpp src/image/SkSurface.cpp src/image/SkSurface_Raster.cpp src/pipe/SkGPipeRead.cpp src/pipe/SkGPipeWrite.cpp src/pathops/SkAddIntersections.cpp src/pathops/SkDConicLineIntersection.cpp src/pathops/SkDCubicLineIntersection.cpp src/pathops/SkDLineIntersection.cpp src/pathops/SkDQuadLineIntersection.cpp src/pathops/SkIntersections.cpp src/pathops/SkOpAngle.cpp src/pathops/SkOpBuilder.cpp src/pathops/SkOpCoincidence.cpp src/pathops/SkOpContour.cpp src/pathops/SkOpCubicHull.cpp src/pathops/SkOpEdgeBuilder.cpp src/pathops/SkOpSegment.cpp src/pathops/SkOpSpan.cpp src/pathops/SkPathOpsCommon.cpp src/pathops/SkPathOpsConic.cpp src/pathops/SkPathOpsCubic.cpp src/pathops/SkPathOpsCurve.cpp src/pathops/SkPathOpsDebug.cpp src/pathops/SkPathOpsLine.cpp src/pathops/SkPathOpsOp.cpp src/pathops/SkPathOpsPoint.cpp src/pathops/SkPathOpsQuad.cpp src/pathops/SkPathOpsRect.cpp src/pathops/SkPathOpsSimplify.cpp src/pathops/SkPathOpsTSect.cpp src/pathops/SkPathOpsTightBounds.cpp src/pathops/SkPathOpsTypes.cpp src/pathops/SkPathOpsWinding.cpp src/pathops/SkPathWriter.cpp src/pathops/SkReduceOrder.cpp)


SET(include_dirs ${INCLUDE_DIRS} ${freetype_INCLUDE_DIRS} ${fontconfig_INCLUDE_DIRS} ${libpng12_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include/config ${CMAKE_SOURCE_DIR}/include/private ${CMAKE_SOURCE_DIR}/include/c ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/include/core ${CMAKE_SOURCE_DIR}/src/utils ${CMAKE_SOURCE_DIR}/include/utils ${CMAKE_SOURCE_DIR}/src/gpu ${CMAKE_SOURCE_DIR}/include/gpu ${CMAKE_SOURCE_DIR}/src/image ${CMAKE_SOURCE_DIR}/include/pathops ${CMAKE_SOURCE_DIR}/include/ports ${CMAKE_SOURCE_DIR}/src/opts ${CMAKE_SOURCE_DIR}/src/sfnt ${CMAKE_SOURCE_DIR}/include/pipe)
SET(link_libraries ${freetype_LIBRARIES} ${fontconfig_LIBRARIES} ${libpng12_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -lgif)

ADD_LIBRARY(skia SHARED $<TARGET_OBJECTS:core> $<TARGET_OBJECTS:core-ext>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(skia ${link_libraries})

INSTALL(TARGETS skia
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
