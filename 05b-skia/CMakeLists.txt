PROJECT(chromium-skia C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Skia")

# Location of Skia Chromium extensions (has an "ext" subfolder)
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../skia")

# Location of direct dependencies
SET(MOJO_SRC_DIR "${CMAKE_SOURCE_DIR}/../../mojo")

# Verify the Toolchain file has been passed
IF(NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE(FATAL_ERROR "Please pass \"-DCMAKE_TOOLCHAIN_FILE=../Toolchain-<arch>.cmake\" when running cmake !")
ENDIF(NOT CMAKE_TOOLCHAIN_FILE)
MESSAGE(STATUS "Chosen Toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# Verify a backend (GLX/EGL) has been chosen ; default to EGL
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<GLX/EGL>\" not passed; defaulting to EGL...")
  SET(BACKEND "EGL")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

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
PKG_CHECK_MODULES(libwebp REQUIRED libwebp)

# libjpeg-turbo
IF(EXISTS "/usr/include/jpeglib.h")
  SET(HAVE_LIBJPEG_TURBO_H "1")
ENDIF()
CHECK_LIBRARY_EXISTS(jpeg jpeg_start_decompress "" HAVE_LIBJPEG_TURBO_SO)
IF(HAVE_LIBJPEG_TURBO_H)
  IF(HAVE_LIBJPEG_TURBO_SO)
    SET(HAVE_LIBJPEG_TURBO "1")
  ENDIF(HAVE_LIBJPEG_TURBO_SO)
ENDIF(HAVE_LIBJPEG_TURBO_H)
IF(NOT HAVE_LIBJPEG_TURBO)
  MESSAGE(FATAL_ERROR "\"jpeglib.h\" or \"libjpeg.so\" missing.
    Please install the \"libjpeg-turbo-devel\" or \"libjpeg62-turbo-dev\" package !")
ENDIF(NOT HAVE_LIBJPEG_TURBO)

# libgif
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

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Optional dependencies (Backends)
IF(BACKEND STREQUAL "EGL")
  PKG_CHECK_MODULES(backend REQUIRED egl glesv2)
  SET(BACKEND_SOURCE_FILES src/gpu/gl/egl/GrGLCreateNativeInterface_egl.cpp)
  ADD_DEFINITIONS(-DSK_EGL=1)
ELSE()
  ADD_DEFINITIONS(-DSK_EGL=0)
ENDIF(BACKEND STREQUAL "EGL")

IF(BACKEND STREQUAL "GLX")
  PKG_CHECK_MODULES(backend REQUIRED gl glu x11)
  SET(BACKEND_SOURCE_FILES src/gpu/gl/glx/GrGLCreateNativeInterface_glx.cpp)
ENDIF(BACKEND STREQUAL "GLX")

# Common definitions
ADD_DEFINITIONS(-DSK_INTERNAL=1 -DSK_SUPPORT_GPU=1 -DSK_FORCE_DISTANCE_FIELD_TEXT=0)
ADD_DEFINITIONS(-DSK_GAMMA_SRGB -DSK_GAMMA_APPLY_TO_A8 -DSK_SAMPLES_FOR_X)
ADD_DEFINITIONS(-DSK_BUILD_FOR_UNIX -DSKIA_DLL -DSKIA_IMPLEMENTATION=1)

# Chromium-specific definitions !
ADD_DEFINITIONS(-DSK_SUPPORT_LEGACY_GETDEVICE -DSK_SUPPORT_LEGACY_GETTOPDEVICE)
# this exposes 2 private methods : "SkCanvas::getDevice()", "SkCanvas::getTopDevice()"

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  # Required for "SkToU8()", "SkToS32()"... functions
  ADD_DEFINITIONS(-DSK_DEVELOPER=1 -DSK_DEBUG)
ELSE()
  ADD_DEFINITIONS(-DNDEBUG)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)

IF(CMAKE_COMPILER_IS_GNUCC)
    ADD_DEFINITIONS(-fno-exceptions -fstrict-aliasing -Wall -Wextra -Winit-self -Wpointer-arith -Wsign-compare -Wno-unused-parameter -Werror)
ENDIF()

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -fno-threadsafe-statics -Wnon-virtual-dtor -Wno-literal-suffix -Wno-deprecated-declarations)
# former last parameter for "png.h" in libpng12 ; better solution ?
# last parameter for "ext/analysis_canvas.cc"
ENDIF()


# PRE-GENERATION .TMPL -> .ZIP (bytecode)

MESSAGE(STATUS "Generating .zip files from .tmpl...")

EXECUTE_PROCESS(COMMAND mkdir -p mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs precompile -o ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings ${CMAKE_SOURCE_DIR})


# PRE-GENERATION .MOJOM -> .CC,.H (bindings)

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND mkdir -p skia WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate ${SKIA_EXT_DIR}/public/interfaces/bitmap.mojom -d .. -I. -o ${CMAKE_BINARY_DIR}/skia --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py --use_bundled_pylibs generate ${SKIA_EXT_DIR}/public/interfaces/bitmap.mojom -d .. -I. -o ${CMAKE_BINARY_DIR}/skia --bytecode_path ${CMAKE_BINARY_DIR}/mojo/public/tools/bindings --generate_non_variant_code -g c++  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST, PER SECTION

# core
ADD_LIBRARY(core OBJECT src/c/sk_paint.cpp src/c/sk_surface.cpp src/core/SkGlobalInitialization_core.cpp src/core/SkAAClip.cpp src/core/SkAnalyticEdge.cpp src/core/SkAnnotation.cpp src/core/SkAlphaRuns.cpp src/core/SkArenaAlloc.cpp src/core/SkAutoPixmapStorage.cpp src/core/SkBBHFactory.cpp src/core/SkBigPicture.cpp src/core/SkBitmap.cpp src/core/SkBitmapCache.cpp src/core/SkBitmapController.cpp src/core/SkBitmapDevice.cpp src/core/SkBitmapProcShader.cpp src/core/SkBitmapProcState.cpp src/core/SkBitmapProcState_matrixProcs.cpp src/core/SkBitmapProvider.cpp src/core/SkBitmapScaler.cpp src/core/SkLinearBitmapPipeline.cpp src/core/SkBlitMask_D32.cpp src/core/SkBlitRow_D16.cpp src/core/SkBlitRow_D32.cpp src/core/SkBlitter.cpp src/core/SkBlitter_A8.cpp src/core/SkBlitter_ARGB32.cpp src/core/SkBlitter_RGB16.cpp src/core/SkBlitter_PM4f.cpp src/core/SkBlitter_Sprite.cpp src/core/SkSpriteBlitter4f.cpp src/core/SkBlurImageFilter.cpp src/core/SkBuffer.cpp src/core/SkCachedData.cpp src/core/SkCanvas.cpp src/core/SkChunkAlloc.cpp src/core/SkClipStack.cpp src/core/SkColor.cpp src/core/SkColorFilter.cpp src/core/SkColorFilterShader.cpp src/core/SkColorShader.cpp src/core/SkColorSpace.cpp src/core/SkColorSpace_A2B.cpp src/core/SkColorSpace_ICC.cpp src/core/SkColorSpace_XYZ.cpp src/core/SkColorSpaceXform.cpp src/core/SkColorSpaceXform_A2B.cpp src/core/SkColorTable.cpp src/core/SkColorLookUpTable.cpp src/core/SkColorMatrixFilterRowMajor255.cpp src/core/SkComposeShader.cpp src/core/SkConfig8888.cpp src/core/SkConvolver.cpp src/core/SkCubicClipper.cpp src/core/SkCpu.cpp src/core/SkData.cpp src/core/SkDataTable.cpp src/core/SkDebug.cpp src/core/SkDeque.cpp src/core/SkDevice.cpp src/core/SkDeviceLooper.cpp src/core/SkDeviceProfile.cpp src/lazy/SkDiscardableMemoryPool.cpp src/core/SkDistanceFieldGen.cpp src/core/SkDither.cpp src/core/SkDocument.cpp src/core/SkDraw.cpp src/core/SkDrawable.cpp src/core/SkDrawLooper.cpp src/core/SkEdgeBuilder.cpp src/core/SkEdgeClipper.cpp src/core/SkEdge.cpp src/core/SkFilterProc.cpp src/core/SkFlattenable.cpp src/core/SkFlattenableSerialization.cpp src/core/SkNormalSource.cpp src/core/SkNormalBevelSource.cpp src/core/SkNormalFlatSource.cpp src/core/SkNormalMapSource.cpp src/core/SkFont.cpp src/core/SkFontLCDConfig.cpp src/core/SkFontMgr.cpp src/core/SkFontStyle.cpp src/core/SkFontDescriptor.cpp src/core/SkFontStream.cpp src/core/SkGeometry.cpp src/core/SkGlyphCache.cpp src/core/SkGpuBlurUtils.cpp src/core/SkGraphics.cpp src/core/SkHalf.cpp src/core/SkICC.cpp src/core/SkImageFilter.cpp src/core/SkImageFilterCache.cpp src/core/SkImageInfo.cpp src/core/SkImageCacherator.cpp src/core/SkImageGenerator.cpp src/core/SkSpecialImage.cpp src/core/SkSpecialSurface.cpp src/core/SkLatticeIter.cpp src/core/SkLights.cpp src/core/SkLightingShader.cpp src/core/SkLineClipper.cpp src/core/SkLocalMatrixImageFilter.cpp src/core/SkLocalMatrixShader.cpp src/core/SkMallocPixelRef.cpp src/core/SkMask.cpp src/core/SkMaskCache.cpp src/core/SkMaskCache.cpp src/core/SkMaskFilter.cpp src/core/SkMaskGamma.cpp src/core/SkMath.cpp src/core/SkMatrix.cpp src/core/SkMatrixImageFilter.cpp src/core/SkMatrix44.cpp src/core/SkMetaData.cpp src/core/SkMipMap.cpp src/core/SkMiniRecorder.cpp src/core/SkModeColorFilter.cpp src/core/SkMultiPictureDraw.cpp src/core/SkOpts.cpp src/core/SkPaint.cpp src/core/SkPaintPriv.cpp src/core/SkPath.cpp src/core/SkPathEffect.cpp src/core/SkPathMeasure.cpp src/core/SkPathRef.cpp src/core/SkPicture.cpp src/core/SkPictureContentInfo.cpp src/core/SkPictureData.cpp src/core/SkPictureFlat.cpp src/core/SkPictureImageGenerator.cpp src/core/SkPicturePlayback.cpp src/core/SkPictureRecord.cpp src/core/SkPictureRecorder.cpp src/core/SkPictureShader.cpp src/core/SkPixelRef.cpp src/core/SkPixmap.cpp src/core/SkPoint.cpp src/core/SkPoint3.cpp src/core/SkPtrRecorder.cpp src/core/SkQuadClipper.cpp src/core/SkRasterClip.cpp src/core/SkRasterPipeline.cpp src/core/SkRasterPipelineBlitter.cpp src/core/SkRasterizer.cpp src/core/SkReadBuffer.cpp src/core/SkRecord.cpp src/core/SkRecords.cpp src/core/SkRecordDraw.cpp src/core/SkRecordOpts.cpp src/core/SkRecordedDrawable.cpp src/core/SkRecorder.cpp src/core/SkRect.cpp src/core/SkRefDict.cpp src/core/SkRegion.cpp src/core/SkRegion_path.cpp src/core/SkResourceCache.cpp src/core/SkRRect.cpp src/core/SkSRGB.cpp src/core/SkRTree.cpp src/core/SkRWBuffer.cpp src/core/SkScalar.cpp src/core/SkScalerContext.cpp src/core/SkScan.cpp src/core/SkScan_AAAPath.cpp src/core/SkScan_AntiPath.cpp src/core/SkScan_Antihair.cpp src/core/SkScan_Hairline.cpp src/core/SkScan_Path.cpp src/core/SkSemaphore.cpp src/core/SkShader.cpp src/core/SkSharedMutex.cpp src/core/SkSpanProcs.cpp src/core/SkSpinlock.cpp src/core/SkSpriteBlitter_ARGB32.cpp src/core/SkSpriteBlitter_RGB16.cpp src/core/SkStream.cpp src/core/SkString.cpp src/core/SkStringUtils.cpp src/core/SkStroke.cpp src/core/SkStrokeRec.cpp src/core/SkStrokerPriv.cpp src/core/SkTaskGroup.cpp src/core/SkTextBlob.cpp src/core/SkTime.cpp src/core/SkThreadID.cpp src/core/SkTLS.cpp src/core/SkTSearch.cpp src/core/SkTypeface.cpp src/core/SkTypefaceCache.cpp src/core/SkUnPreMultiply.cpp src/core/SkUtils.cpp src/core/SkValidatingReadBuffer.cpp src/core/SkVarAlloc.cpp src/core/SkVertState.cpp src/core/SkWriteBuffer.cpp src/core/SkWriter32.cpp src/core/SkXfermode.cpp src/core/SkXfermode4f.cpp src/core/SkXfermodeF16.cpp src/core/SkXfermodeInterpretation.cpp src/core/SkYUVPlanesCache.cpp)

# core-ext
ADD_LIBRARY(core-ext OBJECT src/image/SkImage.cpp src/image/SkImage_Generator.cpp src/image/SkImage_Raster.cpp src/image/SkImageShader.cpp src/image/SkSurface.cpp src/image/SkSurface_Raster.cpp src/pathops/SkAddIntersections.cpp src/pathops/SkDConicLineIntersection.cpp src/pathops/SkDCubicLineIntersection.cpp src/pathops/SkDLineIntersection.cpp src/pathops/SkDQuadLineIntersection.cpp src/pathops/SkIntersections.cpp src/pathops/SkOpAngle.cpp src/pathops/SkOpBuilder.cpp src/pathops/SkOpCoincidence.cpp src/pathops/SkOpContour.cpp src/pathops/SkOpCubicHull.cpp src/pathops/SkOpEdgeBuilder.cpp src/pathops/SkOpSegment.cpp src/pathops/SkOpSpan.cpp src/pathops/SkPathOpsCommon.cpp src/pathops/SkPathOpsConic.cpp src/pathops/SkPathOpsCubic.cpp src/pathops/SkPathOpsCurve.cpp src/pathops/SkPathOpsDebug.cpp src/pathops/SkPathOpsLine.cpp src/pathops/SkPathOpsOp.cpp src/pathops/SkPathOpsPoint.cpp src/pathops/SkPathOpsQuad.cpp src/pathops/SkPathOpsRect.cpp src/pathops/SkPathOpsSimplify.cpp src/pathops/SkPathOpsTSect.cpp src/pathops/SkPathOpsTightBounds.cpp src/pathops/SkPathOpsTypes.cpp src/pathops/SkPathOpsWinding.cpp src/pathops/SkPathWriter.cpp src/pathops/SkReduceOrder.cpp)

# etc1
ADD_LIBRARY(etc1 OBJECT third_party/etc1/etc1.cpp)

# utils
ADD_LIBRARY(utils OBJECT src/utils/SkBase64.cpp src/utils/SkBitmapSourceDeserializer.cpp src/utils/SkBoundaryPatch.cpp src/utils/SkFrontBufferedStream.cpp src/utils/SkCamera.cpp src/utils/SkCanvasStack.cpp src/utils/SkCanvasStateUtils.cpp src/utils/SkDashPath.cpp src/utils/SkDumpCanvas.cpp src/utils/SkEventTracer.cpp src/utils/SkOSPath.cpp src/utils/SkMatrix22.cpp src/utils/SkMeshUtils.cpp src/utils/SkNWayCanvas.cpp src/utils/SkNullCanvas.cpp src/utils/SkPaintFilterCanvas.cpp src/utils/SkParse.cpp src/utils/SkParseColor.cpp src/utils/SkParsePath.cpp src/utils/SkPatchGrid.cpp src/utils/SkPatchUtils.cpp src/utils/SkRGBAToYUV.cpp src/utils/SkTextBox.cpp src/utils/SkTextureCompressor.cpp src/utils/SkTextureCompressor_ASTC.cpp src/utils/SkTextureCompressor_R11EAC.cpp src/utils/SkTextureCompressor_LATC.cpp src/utils/SkThreadUtils_pthread.cpp src/utils/SkWhitelistTypefaces.cpp)

# ktx
ADD_LIBRARY(ktx OBJECT third_party/ktx/ktx.cpp)

# images
ADD_LIBRARY(images OBJECT src/images/SkImageEncoder.cpp)

# effects
ADD_LIBRARY(effects OBJECT src/effects/GrAlphaThresholdFragmentProcessor.cpp src/effects/GrCircleBlurFragmentProcessor.cpp src/effects/Sk1DPathEffect.cpp src/effects/Sk2DPathEffect.cpp src/effects/SkAlphaThresholdFilter.cpp src/effects/SkArcToPathEffect.cpp src/effects/SkArithmeticImageFilter.cpp src/effects/SkArithmeticMode.cpp src/effects/SkBlurDrawLooper.cpp src/effects/SkBlurMask.cpp src/effects/SkBlurMaskFilter.cpp src/effects/SkColorFilterImageFilter.cpp src/effects/SkColorMatrix.cpp src/effects/SkColorMatrixFilter.cpp src/effects/SkComposeImageFilter.cpp src/effects/SkCornerPathEffect.cpp src/effects/SkDashPathEffect.cpp src/effects/SkDiscretePathEffect.cpp src/effects/SkDisplacementMapEffect.cpp src/effects/SkDropShadowImageFilter.cpp src/effects/SkEmbossMask.cpp src/effects/SkEmbossMaskFilter.cpp src/effects/SkImageSource.cpp src/effects/SkLayerDrawLooper.cpp src/effects/SkLayerRasterizer.cpp src/effects/SkLumaColorFilter.cpp src/effects/SkOverdrawColorFilter.cpp src/effects/SkLightingImageFilter.cpp src/effects/SkMagnifierImageFilter.cpp src/effects/SkMatrixConvolutionImageFilter.cpp src/effects/SkMergeImageFilter.cpp src/effects/SkMorphologyImageFilter.cpp src/effects/SkOffsetImageFilter.cpp src/effects/SkPackBits.cpp src/effects/SkPaintFlagsDrawFilter.cpp src/effects/SkPaintImageFilter.cpp src/effects/SkPerlinNoiseShader.cpp src/effects/SkPictureImageFilter.cpp src/effects/SkTableColorFilter.cpp src/effects/SkTableMaskFilter.cpp src/effects/SkTileImageFilter.cpp src/effects/SkXfermodeImageFilter.cpp src/effects/SkRRectsGaussianEdgeMaskFilter.cpp src/effects/gradients/Sk4fGradientBase.cpp src/effects/gradients/SkClampRange.cpp src/effects/gradients/SkGradientBitmapCache.cpp src/effects/gradients/SkGradientShader.cpp src/effects/SkGaussianEdgeShader.cpp src/effects/gradients/SkLinearGradient.cpp src/effects/gradients/Sk4fLinearGradient.cpp src/effects/gradients/SkRadialGradient.cpp src/effects/gradients/SkTwoPointConicalGradient.cpp src/effects/gradients/SkTwoPointConicalGradient_gpu.cpp src/effects/gradients/SkSweepGradient.cpp)

# gpu
ADD_LIBRARY(gpu OBJECT src/gpu/SkGpuDevice.cpp src/gpu/SkGpuDevice_drawTexture.cpp src/gpu/SkGr.cpp src/image/SkImage_Gpu.cpp src/image/SkSurface_Gpu.cpp src/gpu/GrAuditTrail.cpp src/gpu/GrBlend.cpp src/gpu/GrBlurUtils.cpp src/gpu/GrBuffer.cpp src/gpu/GrBufferAllocPool.cpp src/gpu/GrCaps.cpp src/gpu/GrContext.cpp src/gpu/GrCoordTransform.cpp src/gpu/GrDefaultGeoProcFactory.cpp src/gpu/GrDrawingManager.cpp src/gpu/GrDrawOpAtlas.cpp src/gpu/GrDrawOpTest.cpp src/gpu/GrFixedClip.cpp src/gpu/GrFragmentProcessor.cpp src/gpu/GrGpu.cpp src/gpu/GrGpuCommandBuffer.cpp src/gpu/GrGpuResource.cpp src/gpu/GrGpuFactory.cpp src/gpu/GrBitmapTextureMaker.cpp src/gpu/GrImageTextureMaker.cpp src/gpu/GrColorSpaceXform.cpp src/gpu/GrMemoryPool.cpp src/gpu/GrOpFlushState.cpp src/gpu/GrOpList.cpp src/gpu/GrPaint.cpp src/gpu/GrPath.cpp src/gpu/GrPathProcessor.cpp src/gpu/GrPathRange.cpp src/gpu/GrPathRendererChain.cpp src/gpu/GrPathRenderer.cpp src/gpu/GrPathRendering.cpp src/gpu/GrPathRenderingRenderTargetContext.cpp src/gpu/GrPathUtils.cpp src/gpu/GrPipeline.cpp src/gpu/GrPipelineBuilder.cpp src/gpu/GrPrimitiveProcessor.cpp src/gpu/GrProcessor.cpp src/gpu/GrProcessorSet.cpp src/gpu/GrProcessorUnitTest.cpp src/gpu/GrProcOptInfo.cpp src/gpu/GrProgramDesc.cpp src/gpu/GrGpuResourceRef.cpp src/gpu/GrRectanizer_pow2.cpp src/gpu/GrRectanizer_skyline.cpp src/gpu/GrRenderTarget.cpp src/gpu/GrRenderTargetContext.cpp src/gpu/GrRenderTargetOpList.cpp src/gpu/GrRenderTargetProxy.cpp src/gpu/GrReducedClip.cpp src/gpu/GrResourceCache.cpp src/gpu/GrResourceProvider.cpp src/gpu/GrClipStackClip.cpp src/gpu/GrShape.cpp src/gpu/GrShaderCaps.cpp src/gpu/GrShaderVar.cpp src/gpu/GrStencilAttachment.cpp src/gpu/GrStencilSettings.cpp src/gpu/GrStyle.cpp src/gpu/GrTessellator.cpp src/gpu/GrSWMaskHelper.cpp src/gpu/GrSoftwarePathRenderer.cpp src/gpu/GrSurface.cpp src/gpu/GrSurfaceContext.cpp src/gpu/GrSurfaceProxy.cpp src/gpu/GrTestUtils.cpp src/gpu/GrTexture.cpp src/gpu/GrTextureAdjuster.cpp src/gpu/GrTextureContext.cpp src/gpu/GrTextureMaker.cpp src/gpu/GrTextureOpList.cpp src/gpu/GrTextureProducer.cpp src/gpu/GrTextureProvider.cpp src/gpu/GrTextureProxy.cpp src/gpu/GrTextureRenderTargetProxy.cpp src/gpu/GrTextureToYUVPlanes.cpp src/gpu/GrXferProcessor.cpp src/gpu/GrYUVProvider.cpp src/gpu/instanced/InstanceProcessor.cpp src/gpu/instanced/InstancedRendering.cpp src/gpu/instanced/GLInstancedRendering.cpp src/gpu/ops/GrAAHairLinePathRenderer.cpp src/gpu/ops/GrAALinearizingConvexPathRenderer.cpp src/gpu/ops/GrAAConvexTessellator.cpp src/gpu/ops/GrAADistanceFieldPathRenderer.cpp src/gpu/ops/GrAAConvexPathRenderer.cpp src/gpu/ops/GrAAFillRectOp.cpp src/gpu/ops/GrAAStrokeRectOp.cpp src/gpu/ops/GrAnalyticRectOp.cpp src/gpu/ops/GrAtlasTextOp.cpp src/gpu/ops/GrOp.cpp src/gpu/ops/GrCopySurfaceOp.cpp src/gpu/ops/GrDashOp.cpp src/gpu/ops/GrDashLinePathRenderer.cpp src/gpu/ops/GrDefaultPathRenderer.cpp src/gpu/ops/GrDrawOp.cpp src/gpu/ops/GrDrawAtlasOp.cpp src/gpu/ops/GrDrawPathOp.cpp src/gpu/ops/GrDrawVerticesOp.cpp src/gpu/ops/GrNonAAFillRectOp.cpp src/gpu/ops/GrNonAAFillRectPerspectiveOp.cpp src/gpu/ops/GrNonAAStrokeRectOp.cpp src/gpu/ops/GrOvalOpFactory.cpp src/gpu/ops/GrRectOpFactory.cpp src/gpu/ops/GrShadowRRectOp.cpp src/gpu/ops/GrMSAAPathRenderer.cpp src/gpu/ops/GrPLSPathRenderer.cpp src/gpu/ops/GrStencilAndCoverPathRenderer.cpp src/gpu/ops/GrTessellatingPathRenderer.cpp src/gpu/ops/GrLatticeOp.cpp src/gpu/ops/GrMeshDrawOp.cpp src/gpu/ops/GrRegionOp.cpp src/gpu/effects/GrConfigConversionEffect.cpp src/gpu/effects/GrConstColorProcessor.cpp src/gpu/effects/GrPorterDuffXferProcessor.cpp src/gpu/effects/GrCoverageSetOpXP.cpp src/gpu/effects/GrCustomXfermode.cpp src/gpu/effects/GrBezierEffect.cpp src/gpu/effects/GrConvexPolyEffect.cpp src/gpu/effects/GrBicubicEffect.cpp src/gpu/effects/GrBitmapTextGeoProc.cpp src/gpu/effects/GrDisableColorXP.cpp src/gpu/effects/GrDistanceFieldGeoProc.cpp src/gpu/effects/GrShadowGeoProc.cpp src/gpu/effects/GrDitherEffect.cpp src/gpu/effects/GrMatrixConvolutionEffect.cpp src/gpu/effects/GrOvalEffect.cpp src/gpu/effects/GrRRectEffect.cpp src/gpu/effects/GrSimpleTextureEffect.cpp src/gpu/effects/GrSingleTextureEffect.cpp src/gpu/effects/GrTextureDomain.cpp src/gpu/effects/GrTextureStripAtlas.cpp src/gpu/effects/GrBlurredEdgeFragmentProcessor.cpp src/gpu/effects/GrGaussianConvolutionFragmentProcessor.cpp src/gpu/effects/GrXfermodeFragmentProcessor.cpp src/gpu/effects/GrSRGBEffect.cpp src/gpu/effects/GrYUVEffect.cpp src/gpu/text/GrAtlasGlyphCache.cpp src/gpu/text/GrAtlasTextBlob.cpp src/gpu/text/GrAtlasTextBlob_regenInOp.cpp src/gpu/text/GrAtlasTextContext.cpp src/gpu/text/GrDistanceFieldAdjustTable.cpp src/gpu/text/GrStencilAndCoverTextContext.cpp src/gpu/text/GrTextBlobCache.cpp src/gpu/text/GrTextUtils.cpp)

# gpu-gl
ADD_LIBRARY(gpu-gl OBJECT src/gpu/gl/GrGLAssembleInterface.cpp src/gpu/gl/GrGLBuffer.cpp src/gpu/gl/GrGLCaps.cpp src/gpu/gl/GrGLContext.cpp src/gpu/gl/GrGLGLSL.cpp src/gpu/gl/GrGLGpu.cpp src/gpu/gl/GrGLGpuProgramCache.cpp src/gpu/gl/GrGLExtensions.cpp src/gpu/gl/GrGLInterface.cpp src/gpu/gl/GrGLPath.cpp src/gpu/gl/GrGLPathRange.cpp src/gpu/gl/GrGLPathRendering.cpp src/gpu/gl/GrGLProgram.cpp src/gpu/gl/GrGLProgramDataManager.cpp src/gpu/gl/GrGLRenderTarget.cpp src/gpu/gl/GrGLStencilAttachment.cpp src/gpu/gl/GrGLTexture.cpp src/gpu/gl/GrGLTextureRenderTarget.cpp src/gpu/gl/GrGLUtil.cpp src/gpu/gl/GrGLUniformHandler.cpp src/gpu/gl/GrGLVaryingHandler.cpp src/gpu/gl/GrGLVertexArray.cpp src/gpu/gl/builders/GrGLProgramBuilder.cpp src/gpu/gl/builders/GrGLShaderStringBuilder.cpp src/gpu/glsl/GrGLSL.cpp src/gpu/glsl/GrGLSLBlend.cpp src/gpu/glsl/GrGLSLFragmentProcessor.cpp src/gpu/glsl/GrGLSLFragmentShaderBuilder.cpp src/gpu/glsl/GrGLSLFragmentShaderBuilder.cpp src/gpu/glsl/GrGLSLGeometryProcessor.cpp src/gpu/glsl/GrGLSLGeometryShaderBuilder.cpp src/gpu/gl/builders/GrGLSLPrettyPrint.cpp src/gpu/glsl/GrGLSLPrimitiveProcessor.cpp src/gpu/glsl/GrGLSLProgramBuilder.cpp src/gpu/glsl/GrGLSLProgramDataManager.cpp src/gpu/glsl/GrGLSLShaderBuilder.cpp src/gpu/glsl/GrGLSLUtil.cpp src/gpu/glsl/GrGLSLVarying.cpp src/gpu/glsl/GrGLSLVertexShaderBuilder.cpp src/gpu/glsl/GrGLSLXferProcessor.cpp)

# gpu-gl-backend
ADD_LIBRARY(gpu-gl-backend OBJECT src/gpu/gl/GrGLDefaultInterface_native.cpp tools/gpu/gl/egl/CreatePlatformGLTestContext_egl.cpp tools/gpu/gl/GLTestContext.cpp tools/gpu/TestContext.cpp ${BACKEND_SOURCE_FILES})

# sksl
ADD_LIBRARY(sksl OBJECT src/sksl/SkSLCompiler.cpp src/sksl/SkSLParser.cpp src/sksl/SkSLUtil.cpp src/sksl/SkSLCFGGenerator.cpp src/sksl/SkSLIRGenerator.cpp src/sksl/ir/SkSLType.cpp src/sksl/ir/SkSLSymbolTable.cpp src/sksl/SkSLGLSLCodeGenerator.cpp src/sksl/SkSLSPIRVCodeGenerator.cpp)

# pdf
ADD_LIBRARY(pdf OBJECT src/pdf/SkDocument_PDF_None.cpp)   # remove it when adding below sources
# These other sources actually implement the feature :
# src/pdf/SkPDFDocument.cpp src/xps/SkDocument_XPS.cpp src/xps/SkDocument_XPS_None.cpp src/xps/SkXPSDevice.cpp

# ports
ADD_LIBRARY(ports OBJECT src/fonts/SkFontMgr_indirect.cpp src/fonts/SkRemotableFontMgr.cpp src/ports/SkFontMgr_fontconfig.cpp src/ports/SkFontMgr_fontconfig_factory.cpp src/ports/SkFontConfigInterface_direct.cpp src/ports/SkFontConfigInterface_direct_factory.cpp src/ports/SkGlobalInitialization_default.cpp src/ports/SkMemory_malloc.cpp src/ports/SkOSFile_posix.cpp src/ports/SkOSFile_stdio.cpp src/ports/SkOSLibrary_posix.cpp src/ports/SkTLS_pthread.cpp src/ports/SkFontHost_FreeType.cpp src/ports/SkFontHost_FreeType_common.cpp src/ports/SkFontMgr_custom.cpp src/ports/SkImageGenerator_skia.cpp)
# src/ports/SkDebug_stdio.cpp conflicts with ${SKIA_EXT_DIR}/config/SkUserConfig.h

# codec
ADD_LIBRARY(codec OBJECT src/codec/SkCodec.cpp src/codec/SkGifCodec.cpp src/codec/SkWbmpCodec.cpp src/codec/SkBmpCodec.cpp src/codec/SkBmpMaskCodec.cpp src/codec/SkBmpRLECodec.cpp src/codec/SkBmpStandardCodec.cpp src/codec/SkCodecImageGenerator.cpp src/codec/SkMasks.cpp src/codec/SkMaskSwizzler.cpp src/codec/SkSampler.cpp src/codec/SkStreamBuffer.cpp src/codec/SkSwizzler.cpp third_party/gif/SkGifImageReader.cpp)

# sfnt
ADD_LIBRARY(sfnt OBJECT src/sfnt/SkOTTable_name.cpp src/sfnt/SkOTUtils.cpp)

# opts
ADD_LIBRARY(opts OBJECT ${TOOLCHAIN_OPTS_SOURCE_FILES} src/opts/SkOpts_hsw.cpp)

# ext (This is Chromium-specific !)
ADD_LIBRARY(ext OBJECT ${SKIA_EXT_DIR}/ext/analysis_canvas.cc ${SKIA_EXT_DIR}/ext/opacity_filter_canvas.cc ${SKIA_EXT_DIR}/ext/platform_canvas.cc ${SKIA_EXT_DIR}/ext/convolver.cc ${SKIA_EXT_DIR}/ext/event_tracer_impl.cc ${SKIA_EXT_DIR}/ext/image_operations.cc ${SKIA_EXT_DIR}/ext/skia_utils_base.cc ${SKIA_EXT_DIR}/ext/skia_memory_dump_provider.cc ${SKIA_EXT_DIR}/ext/skia_trace_memory_dump_impl.cc ${SKIA_EXT_DIR}/ext/google_logging.cc ${SKIA_EXT_DIR}/ext/SkDiscardableMemory_chrome.cc)

# generated (These are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/skia/public/interfaces/bitmap.mojom.cc)


# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
# we include ${SKIA_EXT_DIR}/config first to prioritize its SkUserConfig.h header
SET(include_dirs ${INCLUDE_DIRS} ${freetype_INCLUDE_DIRS} ${fontconfig_INCLUDE_DIRS} ${libpng12_INCLUDE_DIRS} ${libwebp_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${SKIA_EXT_DIR}/config ${CMAKE_SOURCE_DIR}/include/config ${CMAKE_SOURCE_DIR}/include/private ${CMAKE_SOURCE_DIR}/include/c ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/include/core ${CMAKE_SOURCE_DIR}/src/utils ${CMAKE_SOURCE_DIR}/include/utils ${CMAKE_SOURCE_DIR}/src/gpu ${CMAKE_SOURCE_DIR}/include/gpu ${CMAKE_SOURCE_DIR}/src/image ${CMAKE_SOURCE_DIR}/include/images ${CMAKE_SOURCE_DIR}/include/pathops ${CMAKE_SOURCE_DIR}/include/ports ${CMAKE_SOURCE_DIR}/src/opts ${CMAKE_SOURCE_DIR}/include/pdf ${CMAKE_SOURCE_DIR}/src/sfnt ${CMAKE_SOURCE_DIR}/include/pipe ${CMAKE_SOURCE_DIR}/include/effects ${CMAKE_SOURCE_DIR}/src/lazy ${CMAKE_SOURCE_DIR}/third_party/etc1 ${CMAKE_SOURCE_DIR}/third_party/ktx ${CMAKE_SOURCE_DIR}/src/sksl ${CMAKE_SOURCE_DIR}/src/codec ${CMAKE_SOURCE_DIR}/include/codec ${CMAKE_SOURCE_DIR}/src/codec ${CMAKE_SOURCE_DIR}/third_party/gif ${CMAKE_SOURCE_DIR}/tools/gpu ${SKIA_EXT_DIR}/.. ${MOJO_SRC_DIR}/Build ${MOJO_SRC_DIR}/Build/mojo ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/skia)
# TODO : "${MOJO_SRC_DIR}/Build" is a HACK ! Fix this once we use .pc files
SET(link_libraries ${LINK_LIBRARIES} ${freetype_LIBRARIES} ${fontconfig_LIBRARIES} ${libpng12_LIBRARIES} ${libwebp_LIBRARIES} ${backend_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ljpeg -lgif)

ADD_LIBRARY(skia SHARED $<TARGET_OBJECTS:core> $<TARGET_OBJECTS:core-ext> $<TARGET_OBJECTS:etc1> $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:ktx> $<TARGET_OBJECTS:images> $<TARGET_OBJECTS:effects> $<TARGET_OBJECTS:gpu> $<TARGET_OBJECTS:gpu-gl> $<TARGET_OBJECTS:gpu-gl-backend> $<TARGET_OBJECTS:sksl> $<TARGET_OBJECTS:pdf> $<TARGET_OBJECTS:ports> $<TARGET_OBJECTS:codec> $<TARGET_OBJECTS:sfnt> $<TARGET_OBJECTS:opts> $<TARGET_OBJECTS:ext> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(skia ${link_libraries})

INSTALL(TARGETS skia
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
