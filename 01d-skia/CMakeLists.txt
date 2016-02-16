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

# Verify a backend (OSMESA/GLX/EGL) has been chosen ; default to EGL
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<OSMESA/GLX/EGL>\" not passed; defaulting to EGL...")
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
  SET(BACKEND_SOURCE_FILES src/gpu/gl/egl/GrGLCreateNativeInterface_egl.cpp src/gpu/gl/egl/SkCreatePlatformGLContext_egl.cpp)
  ADD_DEFINITIONS(-DSK_EGL=1)
ELSE()
  ADD_DEFINITIONS(-DSK_EGL=0)
ENDIF(BACKEND STREQUAL "EGL")

IF(BACKEND STREQUAL "GLX")
  PKG_CHECK_MODULES(backend REQUIRED gl glu x11)
  SET(BACKEND_SOURCE_FILES src/gpu/gl/glx/GrGLCreateNativeInterface_glx.cpp src/gpu/gl/glx/SkCreatePlatformGLContext_glx.cpp)
ENDIF(BACKEND STREQUAL "GLX")

IF(BACKEND STREQUAL "OSMESA")
  PKG_CHECK_MODULES(backend REQUIRED osmesa)
  SET(BACKEND_SOURCE_FILES src/gpu/gl/mesa/SkMesaGLContext.cpp src/gpu/gl/mesa/GrGLCreateMesaInterface.cpp)
ENDIF(BACKEND STREQUAL "OSMESA")

# Common definitions
ADD_DEFINITIONS(-DSK_INTERNAL=1 -DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1 -DSK_SUPPORT_GPU=1 -DSK_FORCE_DISTANCE_FIELD_TEXT=0)
ADD_DEFINITIONS(-DSK_GAMMA_SRGB -DSK_GAMMA_APPLY_TO_A8)
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


# PRE-GENERATION .MOJOM -> .CC,.H

MESSAGE(STATUS "Generating .cc,.h files from .mojom...")

EXECUTE_PROCESS(COMMAND python ${MOJO_SRC_DIR}/public/tools/bindings/mojom_bindings_generator.py ${SKIA_EXT_DIR}/public/interfaces/bitmap.mojom --use_bundled_pylibs -d .. -I. -o ${CMAKE_BINARY_DIR}/skia WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST, PER SECTION

# core
ADD_LIBRARY(core OBJECT src/c/sk_paint.cpp src/c/sk_surface.cpp src/core/SkAAClip.cpp src/core/SkAnnotation.cpp src/core/SkAdvancedTypefaceMetrics.cpp src/core/SkAlphaRuns.cpp src/core/SkBBHFactory.cpp src/core/SkBigPicture.cpp src/core/SkBitmap.cpp src/core/SkBitmapCache.cpp src/core/SkBitmapController.cpp src/core/SkBitmapDevice.cpp src/core/SkBitmapFilter.cpp src/core/SkBitmapHeap.cpp src/core/SkBitmapProcShader.cpp src/core/SkBitmapProcState.cpp src/core/SkBitmapProcState_matrixProcs.cpp src/core/SkBitmapProvider.cpp src/core/SkBitmapScaler.cpp src/core/SkBlitMask_D32.cpp src/core/SkBlitRow_D16.cpp src/core/SkBlitRow_D32.cpp src/core/SkBlitter.cpp src/core/SkBlitter_A8.cpp src/core/SkBlitter_ARGB32.cpp src/core/SkBlitter_RGB16.cpp src/core/SkBlitter_Sprite.cpp src/core/SkBuffer.cpp src/core/SkCachedData.cpp src/core/SkCanvas.cpp src/core/SkChecksum.cpp src/core/SkChunkAlloc.cpp src/core/SkClipStack.cpp src/core/SkColor.cpp src/core/SkColorFilter.cpp src/core/SkColorTable.cpp src/core/SkComposeShader.cpp src/core/SkConfig8888.cpp src/core/SkConvolver.cpp src/core/SkCubicClipper.cpp src/core/SkData.cpp src/core/SkDataTable.cpp src/core/SkDebug.cpp src/core/SkDeque.cpp src/core/SkDevice.cpp src/core/SkDeviceLooper.cpp src/core/SkDeviceProfile.cpp src/lazy/SkDiscardableMemoryPool.cpp src/lazy/SkDiscardablePixelRef.cpp src/core/SkDistanceFieldGen.cpp src/core/SkDither.cpp src/core/SkDraw.cpp src/core/SkDrawable.cpp src/core/SkDrawLooper.cpp src/core/SkEdgeBuilder.cpp src/core/SkEdgeClipper.cpp src/core/SkEdge.cpp src/core/SkError.cpp src/core/SkFilterProc.cpp src/core/SkFilterShader.cpp src/core/SkFlattenable.cpp src/core/SkFlattenableSerialization.cpp src/core/SkFloatBits.cpp src/core/SkFont.cpp src/core/SkFontHost.cpp src/core/SkFontMgr.cpp src/core/SkFontStyle.cpp src/core/SkFontDescriptor.cpp src/core/SkFontStream.cpp src/core/SkGeometry.cpp src/core/SkGlyphCache.cpp src/core/SkGraphics.cpp src/core/SkHalf.cpp src/core/SkImageFilter.cpp src/core/SkImageInfo.cpp src/core/SkImageCacherator.cpp src/core/SkImageGenerator.cpp src/core/SkLightingShader.cpp src/core/SkLineClipper.cpp src/core/SkLocalMatrixImageFilter.cpp src/core/SkLocalMatrixShader.cpp src/core/SkMallocPixelRef.cpp src/core/SkMask.cpp src/core/SkMaskCache.cpp src/core/SkMaskCache.cpp src/core/SkMaskFilter.cpp src/core/SkMaskGamma.cpp src/core/SkMath.cpp src/core/SkMatrix.cpp src/core/SkMatrixImageFilter.cpp src/core/SkMetaData.cpp src/core/SkMipMap.cpp src/core/SkMiniRecorder.cpp src/core/SkMultiPictureDraw.cpp src/core/SkNinePatchIter.cpp src/core/SkOpts.cpp src/core/SkPackBits.cpp src/core/SkPaint.cpp src/core/SkPaintPriv.cpp src/core/SkPath.cpp src/core/SkPathEffect.cpp src/core/SkPathMeasure.cpp src/core/SkPathRef.cpp src/core/SkPicture.cpp src/core/SkPictureContentInfo.cpp src/core/SkPictureData.cpp src/core/SkPictureFlat.cpp src/core/SkPictureImageGenerator.cpp src/core/SkPicturePlayback.cpp src/core/SkPictureRecord.cpp src/core/SkPictureRecorder.cpp src/core/SkPictureShader.cpp src/core/SkPixelRef.cpp src/core/SkPixmap.cpp src/core/SkPoint.cpp src/core/SkPoint3.cpp src/core/SkPtrRecorder.cpp src/core/SkQuadClipper.cpp src/core/SkRasterClip.cpp src/core/SkRasterizer.cpp src/core/SkReadBuffer.cpp src/core/SkRecord.cpp src/core/SkRecords.cpp src/core/SkRecordDraw.cpp src/core/SkRecordOpts.cpp src/core/SkRecorder.cpp src/core/SkRect.cpp src/core/SkRefDict.cpp src/core/SkRegion.cpp src/core/SkRegion_path.cpp src/core/SkRemote.cpp src/core/SkResourceCache.cpp src/core/SkRRect.cpp src/core/SkRTree.cpp src/core/SkRWBuffer.cpp src/core/SkScalar.cpp src/core/SkScalerContext.cpp src/core/SkScan.cpp src/core/SkScan_AntiPath.cpp src/core/SkScan_Antihair.cpp src/core/SkScan_Hairline.cpp src/core/SkScan_Path.cpp src/core/SkSemaphore.cpp src/core/SkShader.cpp src/core/SkSharedMutex.cpp src/core/SkSpinlock.cpp src/core/SkSpriteBlitter_ARGB32.cpp src/core/SkSpriteBlitter_RGB16.cpp src/core/SkStream.cpp src/core/SkString.cpp src/core/SkStringUtils.cpp src/core/SkStroke.cpp src/core/SkStrokeRec.cpp src/core/SkStrokerPriv.cpp src/core/SkTaskGroup.cpp src/core/SkTextBlob.cpp src/core/SkTime.cpp src/core/SkThreadID.cpp src/core/SkTLS.cpp src/core/SkTSearch.cpp src/core/SkTypeface.cpp src/core/SkTypefaceCache.cpp src/core/SkUnPreMultiply.cpp src/core/SkUtils.cpp src/core/SkValidatingReadBuffer.cpp src/core/SkVarAlloc.cpp src/core/SkVertState.cpp src/core/SkWriteBuffer.cpp src/core/SkWriter32.cpp src/core/SkXfermode.cpp src/core/SkXfermodeInterpretation.cpp src/core/SkYUVPlanesCache.cpp)

# core-ext
ADD_LIBRARY(core-ext OBJECT src/doc/SkDocument.cpp src/image/SkImage.cpp src/image/SkImage_Generator.cpp src/image/SkImage_Raster.cpp src/image/SkImageShader.cpp src/image/SkSurface.cpp src/image/SkSurface_Raster.cpp src/pipe/SkGPipeRead.cpp src/pipe/SkGPipeWrite.cpp src/pathops/SkAddIntersections.cpp src/pathops/SkDConicLineIntersection.cpp src/pathops/SkDCubicLineIntersection.cpp src/pathops/SkDLineIntersection.cpp src/pathops/SkDQuadLineIntersection.cpp src/pathops/SkIntersections.cpp src/pathops/SkOpAngle.cpp src/pathops/SkOpBuilder.cpp src/pathops/SkOpCoincidence.cpp src/pathops/SkOpContour.cpp src/pathops/SkOpCubicHull.cpp src/pathops/SkOpEdgeBuilder.cpp src/pathops/SkOpSegment.cpp src/pathops/SkOpSpan.cpp src/pathops/SkPathOpsCommon.cpp src/pathops/SkPathOpsConic.cpp src/pathops/SkPathOpsCubic.cpp src/pathops/SkPathOpsCurve.cpp src/pathops/SkPathOpsDebug.cpp src/pathops/SkPathOpsLine.cpp src/pathops/SkPathOpsOp.cpp src/pathops/SkPathOpsPoint.cpp src/pathops/SkPathOpsQuad.cpp src/pathops/SkPathOpsRect.cpp src/pathops/SkPathOpsSimplify.cpp src/pathops/SkPathOpsTSect.cpp src/pathops/SkPathOpsTightBounds.cpp src/pathops/SkPathOpsTypes.cpp src/pathops/SkPathOpsWinding.cpp src/pathops/SkPathWriter.cpp src/pathops/SkReduceOrder.cpp)

# etc1
ADD_LIBRARY(etc1 OBJECT third_party/etc1/etc1.cpp)

# utils
ADD_LIBRARY(utils OBJECT src/utils/SkBase64.cpp src/utils/SkBitmapHasher.cpp src/utils/SkBitmapSourceDeserializer.cpp src/utils/SkBitSet.cpp src/utils/SkBoundaryPatch.cpp src/utils/SkFrontBufferedStream.cpp src/utils/SkCamera.cpp src/utils/SkCanvasStack.cpp src/utils/SkCanvasStateUtils.cpp src/utils/SkCubicInterval.cpp src/utils/SkCullPoints.cpp src/utils/SkDashPath.cpp src/utils/SkDumpCanvas.cpp src/utils/SkEventTracer.cpp src/utils/SkImageGeneratorUtils.cpp src/utils/SkLayer.cpp src/utils/SkMatrix22.cpp src/utils/SkMatrix44.cpp src/utils/SkMD5.cpp src/utils/SkMeshUtils.cpp src/utils/SkNinePatch.cpp src/utils/SkNWayCanvas.cpp src/utils/SkNullCanvas.cpp src/utils/SkOSFile.cpp src/utils/SkPaintFilterCanvas.cpp src/utils/SkParse.cpp src/utils/SkParseColor.cpp src/utils/SkParsePath.cpp src/utils/SkPatchGrid.cpp src/utils/SkPatchUtils.cpp src/utils/SkSHA1.cpp src/utils/SkRTConf.cpp src/utils/SkTextBox.cpp src/utils/SkTextureCompressor.cpp src/utils/SkTextureCompressor_ASTC.cpp src/utils/SkTextureCompressor_R11EAC.cpp src/utils/SkTextureCompressor_LATC.cpp src/utils/SkThreadUtils_pthread.cpp src/utils/SkWhitelistTypefaces.cpp)

# ktx
ADD_LIBRARY(ktx OBJECT third_party/ktx/ktx.cpp)

# images
ADD_LIBRARY(images OBJECT src/images/bmpdecoderhelper.cpp src/images/SkDecodingImageGenerator.cpp src/images/SkForceLinking.cpp src/images/SkImageDecoder.cpp src/images/SkImageDecoder_FactoryDefault.cpp src/images/SkImageDecoder_FactoryRegistrar.cpp src/images/SkImageDecoder_wbmp.cpp src/images/SkImageDecoder_pkm.cpp src/images/SkImageDecoder_ktx.cpp src/images/SkImageDecoder_astc.cpp src/images/SkImageDecoder_libbmp.cpp src/images/SkImageDecoder_libgif.cpp src/images/SkImageDecoder_libico.cpp src/images/SkImageDecoder_libwebp.cpp src/images/SkImageDecoder_libjpeg.cpp src/images/SkImageDecoder_libpng.cpp src/images/SkImageEncoder.cpp src/images/SkImageEncoder_Factory.cpp src/images/SkImageEncoder_argb.cpp src/images/SkJpegUtility.cpp src/images/SkMovie.cpp src/images/SkMovie_gif.cpp src/images/SkPageFlipper.cpp src/images/SkScaledBitmapSampler.cpp)

# effects
ADD_LIBRARY(effects OBJECT src/effects/GrCircleBlurFragmentProcessor.cpp src/effects/Sk1DPathEffect.cpp src/effects/Sk2DPathEffect.cpp src/effects/SkAlphaThresholdFilter.cpp src/effects/SkArcToPathEffect.cpp src/effects/SkArithmeticMode.cpp src/effects/SkArithmeticMode_gpu.cpp src/effects/SkBlurDrawLooper.cpp src/effects/SkBlurMask.cpp src/effects/SkBlurImageFilter.cpp src/effects/SkBlurMaskFilter.cpp src/effects/SkColorCubeFilter.cpp src/effects/SkColorFilters.cpp src/effects/SkColorFilterImageFilter.cpp src/effects/SkColorMatrix.cpp src/effects/SkColorMatrixFilter.cpp src/effects/SkComposeImageFilter.cpp src/effects/SkCornerPathEffect.cpp src/effects/SkDashPathEffect.cpp src/effects/SkDiscretePathEffect.cpp src/effects/SkDisplacementMapEffect.cpp src/effects/SkDropShadowImageFilter.cpp src/effects/SkEmbossMask.cpp src/effects/SkEmbossMaskFilter.cpp src/effects/SkImageSource.cpp src/effects/SkGpuBlurUtils.cpp src/effects/SkLayerDrawLooper.cpp src/effects/SkLayerRasterizer.cpp src/effects/SkLerpXfermode.cpp src/effects/SkLightingImageFilter.cpp src/effects/SkLumaColorFilter.cpp src/effects/SkMagnifierImageFilter.cpp src/effects/SkMatrixConvolutionImageFilter.cpp src/effects/SkMergeImageFilter.cpp src/effects/SkMorphologyImageFilter.cpp src/effects/SkOffsetImageFilter.cpp src/effects/SkPaintFlagsDrawFilter.cpp src/effects/SkPerlinNoiseShader.cpp src/effects/SkPictureImageFilter.cpp src/effects/SkPixelXorXfermode.cpp src/effects/SkRectShaderImageFilter.cpp src/effects/SkTableColorFilter.cpp src/effects/SkTableMaskFilter.cpp src/effects/SkTestImageFilters.cpp src/effects/SkTileImageFilter.cpp src/effects/SkXfermodeImageFilter.cpp src/effects/gradients/SkClampRange.cpp src/effects/gradients/SkGradientBitmapCache.cpp src/effects/gradients/SkGradientShader.cpp src/effects/gradients/SkLinearGradient.cpp src/effects/gradients/SkRadialGradient.cpp src/effects/gradients/SkTwoPointConicalGradient.cpp src/effects/gradients/SkTwoPointConicalGradient_gpu.cpp src/effects/gradients/SkSweepGradient.cpp)

# gpu
ADD_LIBRARY(gpu OBJECT src/gpu/SkGpuDevice.cpp src/gpu/SkGpuDevice_drawTexture.cpp src/gpu/SkGr.cpp src/gpu/SkGrPixelRef.cpp src/gpu/SkGrTexturePixelRef.cpp src/image/SkImage_Gpu.cpp src/image/SkSurface_Gpu.cpp  src/gpu/GrBatchAtlas.cpp src/gpu/GrBatchFlushState.cpp src/gpu/GrBatchTest.cpp src/gpu/GrBlend.cpp src/gpu/GrBlurUtils.cpp src/gpu/GrBufferAllocPool.cpp src/gpu/GrCaps.cpp src/gpu/GrClip.cpp src/gpu/GrClipMaskManager.cpp src/gpu/GrContext.cpp src/gpu/GrCoordTransform.cpp src/gpu/GrDefaultGeoProcFactory.cpp src/gpu/GrDrawContext src/gpu/GrDrawingManager.cpp src/gpu/GrDrawTarget.cpp src/gpu/GrFragmentProcessor.cpp src/gpu/GrGpu.cpp src/gpu/GrGpuResource.cpp src/gpu/GrGpuFactory.cpp src/gpu/GrImageIDTextureAdjuster.cpp src/gpu/GrInvariantOutput.cpp src/gpu/GrLayerAtlas.cpp src/gpu/GrLayerCache.cpp src/gpu/GrLayerHoister.cpp src/gpu/GrMemoryPool.cpp src/gpu/GrOvalRenderer.cpp src/gpu/GrPaint.cpp src/gpu/GrPath.cpp src/gpu/GrPathProcessor.cpp src/gpu/GrPathRange.cpp src/gpu/GrPathRendererChain.cpp src/gpu/GrPathRenderer.cpp src/gpu/GrPathRendering.cpp src/gpu/GrPathUtils.cpp src/gpu/GrPipeline.cpp src/gpu/GrPipelineBuilder.cpp src/gpu/GrPrimitiveProcessor.cpp src/gpu/GrProgramElement.cpp src/gpu/GrProcessor.cpp src/gpu/GrProcessorUnitTest.cpp src/gpu/GrProcOptInfo.cpp src/gpu/GrGpuResourceRef.cpp src/gpu/GrRecordReplaceDraw.cpp src/gpu/GrRectanizer_pow2.cpp src/gpu/GrRectanizer_skyline.cpp src/gpu/GrRenderTarget.cpp src/gpu/GrReducedClip.cpp src/gpu/GrResourceCache.cpp src/gpu/GrResourceProvider.cpp src/gpu/GrStencil.cpp src/gpu/GrStencilAttachment.cpp src/gpu/GrStrokeInfo.cpp src/gpu/GrTestUtils.cpp src/gpu/GrSWMaskHelper.cpp src/gpu/GrSoftwarePathRenderer.cpp src/gpu/GrSurface.cpp src/gpu/GrTexture.cpp src/gpu/GrTextureParamsAdjuster.cpp src/gpu/GrTextureProvider.cpp src/gpu/GrTextureAccess.cpp src/gpu/GrXferProcessor.cpp src/gpu/GrYUVProvider.cpp src/gpu/batches/GrAAHairLinePathRenderer.cpp src/gpu/batches/GrAALinearizingConvexPathRenderer.cpp src/gpu/batches/GrAAConvexTessellator.cpp src/gpu/batches/GrAADistanceFieldPathRenderer.cpp src/gpu/batches/GrAAConvexPathRenderer.cpp src/gpu/batches/GrAAFillRectBatch.cpp src/gpu/batches/GrAAStrokeRectBatch.cpp src/gpu/batches/GrAtlasTextBatch.cpp src/gpu/batches/GrBatch.cpp src/gpu/batches/GrCopySurfaceBatch.cpp src/gpu/batches/GrDashLinePathRenderer.cpp src/gpu/batches/GrDefaultPathRenderer.cpp src/gpu/batches/GrDrawBatch.cpp src/gpu/batches/GrDrawAtlasBatch.cpp src/gpu/batches/GrDrawPathBatch.cpp src/gpu/batches/GrDrawVerticesBatch.cpp src/gpu/batches/GrNonAAFillRectBatch.cpp src/gpu/batches/GrNonAAStrokeRectBatch.cpp src/gpu/batches/GrNinePatch.cpp src/gpu/batches/GrRectBatchFactory.cpp src/gpu/batches/GrStencilAndCoverPathRenderer.cpp src/gpu/batches/GrTessellatingPathRenderer.cpp src/gpu/batches/GrVertexBatch.cpp src/gpu/effects/GrConfigConversionEffect.cpp src/gpu/effects/GrConstColorProcessor.cpp src/gpu/effects/GrCoverageSetOpXP.cpp src/gpu/effects/GrCustomXfermode.cpp src/gpu/effects/GrBezierEffect.cpp src/gpu/effects/GrConvolutionEffect.cpp src/gpu/effects/GrConvexPolyEffect.cpp src/gpu/effects/GrBicubicEffect.cpp src/gpu/effects/GrBitmapTextGeoProc.cpp src/gpu/effects/GrDashingEffect.cpp src/gpu/effects/GrDisableColorXP.cpp src/gpu/effects/GrDistanceFieldGeoProc.cpp src/gpu/effects/GrDitherEffect.cpp src/gpu/effects/GrMatrixConvolutionEffect.cpp src/gpu/effects/GrOvalEffect.cpp src/gpu/effects/GrPorterDuffXferProcessor.cpp src/gpu/effects/GrRRectEffect.cpp src/gpu/effects/GrSimpleTextureEffect.cpp src/gpu/effects/GrSingleTextureEffect.cpp src/gpu/effects/GrTextureDomain.cpp src/gpu/effects/GrTextureStripAtlas.cpp src/gpu/effects/GrXfermodeFragmentProcessor.cpp src/gpu/effects/GrYUVtoRGBEffect.cpp src/gpu/text/GrAtlasTextBlob.cpp src/gpu/text/GrAtlasTextContext.cpp src/gpu/text/GrBatchFontCache.cpp src/gpu/text/GrDistanceFieldAdjustTable.cpp src/gpu/text/GrFontScaler.cpp src/gpu/text/GrStencilAndCoverTextContext.cpp src/gpu/text/GrTextBlobCache.cpp src/gpu/text/GrTextContext.cpp src/gpu/text/GrTextUtils.cpp)

# gpu-gl
ADD_LIBRARY(gpu-gl OBJECT src/gpu/gl/SkGLContext.cpp src/gpu/gl/GrGLAssembleInterface.cpp src/gpu/gl/GrGLBufferImpl.cpp src/gpu/gl/GrGLCaps.cpp src/gpu/gl/GrGLContext.cpp src/gpu/gl/GrGLGLSL.cpp src/gpu/gl/GrGLGpu.cpp src/gpu/gl/GrGLGpuProgramCache.cpp src/gpu/gl/GrGLExtensions.cpp src/gpu/gl/GrGLIndexBuffer.cpp src/gpu/gl/GrGLInterface.cpp src/gpu/gl/GrGLNameAllocator.cpp src/gpu/gl/GrGLNoOpInterface.cpp src/gpu/gl/GrGLPath.cpp src/gpu/gl/GrGLPathRange.cpp src/gpu/gl/GrGLPathRendering.cpp src/gpu/gl/GrGLProgram.cpp src/gpu/gl/GrGLProgramDesc.cpp src/gpu/gl/GrGLProgramDataManager.cpp src/gpu/gl/GrGLRenderTarget.cpp src/gpu/gl/GrGLStencilAttachment.cpp src/gpu/gl/GrGLTexture.cpp src/gpu/gl/GrGLTextureRenderTarget.cpp src/gpu/gl/GrGLTransferBuffer.cpp src/gpu/gl/GrGLUtil.cpp src/gpu/gl/GrGLUniformHandler.cpp src/gpu/gl/GrGLVaryingHandler.cpp src/gpu/gl/GrGLVertexArray.cpp src/gpu/gl/GrGLVertexBuffer.cpp src/gpu/gl/builders/GrGLProgramBuilder.cpp src/gpu/gl/builders/GrGLShaderStringBuilder.cpp src/gpu/gl/builders/GrGLSLPrettyPrint.cpp src/gpu/glsl/GrGLSL.cpp src/gpu/glsl/GrGLSLBlend.cpp src/gpu/glsl/GrGLSLCaps.cpp src/gpu/glsl/GrGLSLFragmentProcessor.cpp src/gpu/glsl/GrGLSLFragmentShaderBuilder.cpp src/gpu/glsl/GrGLSLFragmentShaderBuilder.cpp src/gpu/glsl/GrGLSLGeometryProcessor.cpp src/gpu/glsl/GrGLSLGeometryShaderBuilder.cpp src/gpu/glsl/GrGLSLPrimitiveProcessor.cpp src/gpu/glsl/GrGLSLProgramBuilder.cpp src/gpu/glsl/GrGLSLShaderBuilder.cpp src/gpu/glsl/GrGLSLUtil.cpp src/gpu/glsl/GrGLSLVarying.cpp src/gpu/glsl/GrGLSLVertexShaderBuilder.cpp src/gpu/glsl/GrGLSLXferProcessor.cpp)

# gpu-gl-backend
ADD_LIBRARY(gpu-gl-backend OBJECT src/gpu/gl/GrGLDefaultInterface_native.cpp ${BACKEND_SOURCE_FILES})

# ports
ADD_LIBRARY(ports OBJECT src/ports/SkDebug_stdio.cpp src/fonts/SkFontMgr_indirect.cpp src/fonts/SkRemotableFontMgr.cpp src/ports/SkFontMgr_fontconfig.cpp src/ports/SkFontMgr_fontconfig_factory.cpp src/ports/SkGlobalInitialization_default.cpp src/ports/SkMemory_malloc.cpp src/ports/SkOSFile_posix.cpp src/ports/SkOSFile_stdio.cpp src/ports/SkOSLibrary_posix.cpp src/ports/SkDiscardableMemory_none.cpp src/ports/SkTime_Unix.cpp src/ports/SkTLS_pthread.cpp src/ports/SkFontHost_FreeType.cpp src/ports/SkFontHost_FreeType_common.cpp src/ports/SkFontMgr_custom.cpp src/ports/SkImageGenerator_skia.cpp)

# sfnt
ADD_LIBRARY(sfnt OBJECT src/sfnt/SkOTTable_name.cpp src/sfnt/SkOTUtils.cpp)

# opts
ADD_LIBRARY(opts OBJECT ${TOOLCHAIN_OPTS_SOURCE_FILES})

# ext (This is Chromium-specific !)
ADD_LIBRARY(ext OBJECT ${SKIA_EXT_DIR}/ext/analysis_canvas.cc ${SKIA_EXT_DIR}/ext/opacity_filter_canvas.cc ${SKIA_EXT_DIR}/ext/platform_canvas.cc ${SKIA_EXT_DIR}/ext/platform_device.cc ${SKIA_EXT_DIR}/ext/platform_device_linux.cc ${SKIA_EXT_DIR}/ext/bitmap_platform_device_skia.cc ${SKIA_EXT_DIR}/ext/convolver.cc ${SKIA_EXT_DIR}/ext/image_operations.cc ${SKIA_EXT_DIR}/ext/google_logging.cc)

# generated (These are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/skia/public/interfaces/bitmap.mojom.cc)


SET(include_dirs ${INCLUDE_DIRS} ${freetype_INCLUDE_DIRS} ${fontconfig_INCLUDE_DIRS} ${libpng12_INCLUDE_DIRS} ${libwebp_INCLUDE_DIRS} ${backend_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/include/config ${CMAKE_SOURCE_DIR}/include/private ${CMAKE_SOURCE_DIR}/include/c ${CMAKE_SOURCE_DIR}/src/core ${CMAKE_SOURCE_DIR}/include/core ${CMAKE_SOURCE_DIR}/src/utils ${CMAKE_SOURCE_DIR}/include/utils ${CMAKE_SOURCE_DIR}/src/gpu ${CMAKE_SOURCE_DIR}/include/gpu ${CMAKE_SOURCE_DIR}/src/image ${CMAKE_SOURCE_DIR}/include/images ${CMAKE_SOURCE_DIR}/include/pathops ${CMAKE_SOURCE_DIR}/include/ports ${CMAKE_SOURCE_DIR}/src/opts ${CMAKE_SOURCE_DIR}/src/sfnt ${CMAKE_SOURCE_DIR}/include/pipe ${CMAKE_SOURCE_DIR}/include/effects ${CMAKE_SOURCE_DIR}/src/lazy ${CMAKE_SOURCE_DIR}/third_party/etc1 ${CMAKE_SOURCE_DIR}/third_party/ktx ${SKIA_EXT_DIR}/.. ${MOJO_SRC_DIR}/build ${MOJO_SRC_DIR}/build/mojo ${CMAKE_BINARY_DIR}/skia)
# TODO : "${MOJO_SRC_DIR}/build" is a HACK ! Fix this once we use .pc files
SET(link_libraries ${freetype_LIBRARIES} ${fontconfig_LIBRARIES} ${libpng12_LIBRARIES} ${libwebp_LIBRARIES} ${backend_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ljpeg -lgif)

ADD_LIBRARY(skia SHARED $<TARGET_OBJECTS:core> $<TARGET_OBJECTS:core-ext> $<TARGET_OBJECTS:etc1> $<TARGET_OBJECTS:utils> $<TARGET_OBJECTS:ktx> $<TARGET_OBJECTS:images> $<TARGET_OBJECTS:effects> $<TARGET_OBJECTS:gpu> $<TARGET_OBJECTS:gpu-gl> $<TARGET_OBJECTS:gpu-gl-backend> $<TARGET_OBJECTS:ports> $<TARGET_OBJECTS:sfnt> $<TARGET_OBJECTS:opts> $<TARGET_OBJECTS:ext> $<TARGET_OBJECTS:generated>)
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(skia ${link_libraries})

INSTALL(TARGETS skia
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})
