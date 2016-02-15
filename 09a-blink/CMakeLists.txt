PROJECT(chromium-blink C CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - Blink")

# Verify the Toolchain file has been passed
IF(NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE(FATAL_ERROR "Please pass \"-DCMAKE_TOOLCHAIN_FILE=../Toolchain-<arch>.cmake\" when running cmake !")
ENDIF(NOT CMAKE_TOOLCHAIN_FILE)
MESSAGE(STATUS "Chosen Toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# Location of Base and V8 source folders
SET(BASE_DIR "${CMAKE_SOURCE_DIR}/../../../base")
SET(V8_DIR "${CMAKE_SOURCE_DIR}/../../../v8")

# Location of Skia source folders
SET(SKIA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/skia")
SET(SKIA_EXT_DIR "${CMAKE_SOURCE_DIR}/../../../skia")

# Location of Chromium-specific OpenGL headers
SET(CHROMIUM_GL_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/khronos")
SET(CHROMIUM_GLES2_EXT_DIR "${CMAKE_SOURCE_DIR}/../../../gpu")

# Location of third-party dependencies
SET(QCMS_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/qcms")
SET(OTS_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/ots")
SET(ICCJPEG_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/iccjpeg")
SET(NPAPI_DIR "${CMAKE_SOURCE_DIR}/../../../third_party/npapi")

# Verify a backend (X11/OZONE) has been chosen ; default to OZONE
IF(NOT BACKEND)
  MESSAGE(STATUS "Option \"-DBACKEND=<X11/OZONE>\" not passed; defaulting to OZONE...")
  SET(BACKEND "OZONE")
ENDIF(NOT BACKEND)
MESSAGE(STATUS "Chosen Backend: ${BACKEND}")

# CMake modules
INCLUDE(FindThreads)
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
FIND_PACKAGE(Threads)
PKG_CHECK_MODULES(icu REQUIRED icu-i18n>=52.0.0 icu-uc>=52.0.0)
PKG_CHECK_MODULES(harfbuzz REQUIRED harfbuzz>=0.9.22)
PKG_CHECK_MODULES(libwebp REQUIRED libwebp>=0.4.0)

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

FIND_PROGRAM(GPERF_FOUND gperf)
IF(NOT GPERF_FOUND)
  MESSAGE(FATAL_ERROR "\"gperf\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"gperf\" found...")
ENDIF(NOT GPERF_FOUND)

# Optional definitions
IF(BACKEND STREQUAL "OZONE")
  ADD_DEFINITIONS(-DUSE_OZONE)
ENDIF(BACKEND STREQUAL "OZONE")

IF(BACKEND STREQUAL "X11")
  ADD_DEFINITIONS(-DUSE_X11)
ENDIF(BACKEND STREQUAL "X11")

# Common definitions
ADD_DEFINITIONS(-DBLINK_IMPLEMENTATION=1 -DBLINK_COMMON_IMPLEMENTATION=1 -DBLINK_PLATFORM_IMPLEMENTATION=1 -DINSIDE_BLINK -DUSE_AURA -DUSING_SYSTEM_ICU)
ADD_DEFINITIONS(-DWTF_IMPLEMENTATION=1)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11 -Wno-c++0x-compat)
ENDIF()


# PRE-GENERATION .IN,.GPERF -> .CC,.H

EXECUTE_PROCESS(COMMAND mkdir -p platform/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND mkdir -p core/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND mkdir -p modules/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

MESSAGE(STATUS "Generating .cc,.h files from .in...")

EXECUTE_PROCESS(COMMAND python core/html/parser/create-html-entity-table -o ${CMAKE_BINARY_DIR}/core/HTMLEntityTable.cpp core/html/parser/HTMLEntityNames.in WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_css_property_names.py core/css/CSSProperties.in --output_dir ${CMAKE_BINARY_DIR}/core --gperf gperf WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py platform/fonts/FontFamilyNames.in --output_dir ${CMAKE_BINARY_DIR}/platform WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py core/events/EventTargetFactory.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py core/events/EventTypeNames.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py core/html/forms/InputTypeNames.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py modules/EventTargetModulesFactory.in --output_dir ${CMAKE_BINARY_DIR}/modules WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_settings.py core/frame/Settings.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_element_factory.py core/html/HTMLTagNames.in core/html/HTMLAttributeNames.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_css_value_keywords.py core/css/CSSValueKeywords.in core/css/SVGCSSValueKeywords.in --output_dir ${CMAKE_BINARY_DIR}/core --gperf gperf WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python core/inspector/CodeGeneratorInspector.py devtools/protocol.json --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python core/inspector/CodeGeneratorInstrumentation.py core/inspector/InspectorInstrumentation.idl --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_element_type_helpers.py core/html/HTMLTagNames.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_media_feature_names.py core/css/MediaFeatureNames.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_runtime_features.py platform/RuntimeEnabledFeatures.in --output_dir ${CMAKE_BINARY_DIR}/platform WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "Generating .cpp from .gperf...")

EXECUTE_PROCESS(COMMAND gperf --key-positions=* -D -s 2 platform/ColorData.gperf --output-file=${CMAKE_BINARY_DIR}/platform/ColorData.cpp WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "Generating EventNames.h/.cpp from .idl files...")

# Generate "EventInterfaces.in", then "EventNames.cpp/.h", from *.idl files
EXECUTE_PROCESS(COMMAND sh -c "find core/ -name *.idl | grep Event.idl > ${CMAKE_BINARY_DIR}/event.lst" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/generate_event_interfaces.py --event-idl-files-list ${CMAKE_BINARY_DIR}/event.lst --event-interfaces-file ${CMAKE_BINARY_DIR}/core/EventInterfaces.in --write-file-only-if-changed 0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py ${CMAKE_BINARY_DIR}/core/EventInterfaces.in --output_dir ${CMAKE_BINARY_DIR}/core WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "Generating EventModulesNames.h/.cpp from .idl files...")

# Generate "EventModulesInterfaces.in", then "EventModulesNames.cpp/.h", from *.idl files
EXECUTE_PROCESS(COMMAND sh -c "find modules/ -name *.idl | grep Event.idl > ${CMAKE_BINARY_DIR}/event_modules.lst" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/generate_event_interfaces.py --event-idl-files-list ${CMAKE_BINARY_DIR}/event_modules.lst --event-interfaces-file ${CMAKE_BINARY_DIR}/modules/EventModulesInterfaces.in --write-file-only-if-changed 0 --suffix Modules WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python build/scripts/make_names.py ${CMAKE_BINARY_DIR}/modules/EventModulesInterfaces.in --output_dir ${CMAKE_BINARY_DIR}/modules WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

MESSAGE(STATUS "Generating .cpp,.h V8 bindings from .idl...")

# We create lists for all "core" and "modules" .idl files
EXECUTE_PROCESS(COMMAND sh -c "find core/ -name *.idl > ${CMAKE_BINARY_DIR}/core.lst" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND sh -c "find modules/ -name *.idl > ${CMAKE_BINARY_DIR}/modules.lst" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# We need to remove one "core" .idl which doesn't work
EXECUTE_PROCESS(COMMAND sed -i s,core/inspector/InspectorInstrumentation.idl,,g core.lst WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
# Remove the now empty line
EXECUTE_PROCESS(COMMAND sh -c "sed -i '/^$/d' core.lst" WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# We create a separate list with only dictionary .idl files, they will allow us
# to generate the "UnionTypesCore.*" source files and also serve as input file
# list for idl_compiler.py (so we can compile everything in one pass)
EXECUTE_PROCESS(COMMAND sh -c "find core/ -name *.idl |grep -E 'Init.idl|Bag.idl|Options.idl|Properties.idl|Descriptors.idl|InternalDictionary|CanvasContextCreationAttributes.idl' > ${CMAKE_BINARY_DIR}/dicts_core.lst" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND sh -c "find modules/ -name *.idl |grep -E 'Init.idl|Bag.idl|Options.idl|Parameters.idl|Descriptor.idl|Range.idl|Attributes.idl|Data.idl|InternalDictionary|MediaTrackConstraintSet.idl|ServicePortConnectResponse.idl|NFCMessage.idl|NFCRecord.idl|FileSystemFlags.idl' |grep -E -v 'VREyeParameters.idl|IDBKeyRange.idl|BluetoothAdvertisingData.idl|PushMessageData.idl' > ${CMAKE_BINARY_DIR}/dicts_modules.lst" WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND mkdir -p bindings/scripts/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND mkdir -p bindings/core/v8/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
EXECUTE_PROCESS(COMMAND mkdir -p bindings/modules/v8/ WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# We create intermediary .pickle files aggregating all the .idl interface names
# Create "core" .pickle files
EXECUTE_PROCESS(COMMAND python bindings/scripts/compute_interfaces_info_individual.py --cache-directory ${CMAKE_BINARY_DIR}/bindings/scripts --idl-files-list ${CMAKE_BINARY_DIR}/core.lst --interfaces-info-file ${CMAKE_BINARY_DIR}/bindings/core/InterfacesInfoCoreIndividual.pickle --component-info-file ${CMAKE_BINARY_DIR}/bindings/core/ComponentInfoCore.pickle --write-file-only-if-changed 0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# Create "modules" .pickle files
EXECUTE_PROCESS(COMMAND python bindings/scripts/compute_interfaces_info_individual.py --cache-directory ${CMAKE_BINARY_DIR}/bindings/scripts --idl-files-list ${CMAKE_BINARY_DIR}/modules.lst --interfaces-info-file ${CMAKE_BINARY_DIR}/bindings/modules/InterfacesInfoModulesIndividual.pickle --component-info-file ${CMAKE_BINARY_DIR}/bindings/modules/ComponentInfoModules.pickle --write-file-only-if-changed 0 WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# Create an aggregating ("core"+"modules") .pickle file
EXECUTE_PROCESS(COMMAND python bindings/scripts/compute_interfaces_info_overall.py --write-file-only-if-changed 0 ${CMAKE_BINARY_DIR}/bindings/core/InterfacesInfoCoreIndividual.pickle ${CMAKE_BINARY_DIR}/bindings/modules/InterfacesInfoModulesIndividual.pickle ${CMAKE_BINARY_DIR}/bindings/modules/InterfacesInfoOverall.pickle WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# We *really* compile all .idl -> .cpp,.h !
# here, to pass a source file list, we use "--impl-output-dir --generate-impl"
# with "dicts.lst" as a last argument (see its generation above)
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --impl-output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --generate-impl --write-file-only-if-changed 0 --target-component core ${CMAKE_BINARY_DIR}/dicts_core.lst WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --impl-output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --generate-impl --write-file-only-if-changed 0 --target-component modules ${CMAKE_BINARY_DIR}/dicts_modules.lst WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# THIS SHOULD BE AUTOMATED

EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/events/EventListenerOptions.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/inspector/InspectorOverlayHost.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/dom/ArrayBuffer.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/dom/ArrayBufferView.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/fileapi/Blob.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/events/EventTarget.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/inspector/DevToolsHost.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/dom/ElementRegistrationOptions.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/dom/Element.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/core/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component core core/dom/Node.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/mediastream/MediaTrackConstraintSet.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/nfc/NFCMessage.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/filesystem/DOMFileSystem.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/filesystem/DirectoryEntry.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/filesystem/Entry.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/filesystem/FileEntry.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
EXECUTE_PROCESS(COMMAND python bindings/scripts/idl_compiler.py --cache-dir ${CMAKE_BINARY_DIR}/bindings/scripts --output-dir ${CMAKE_BINARY_DIR}/bindings/modules/v8 --info-dir ${CMAKE_BINARY_DIR}/bindings --write-file-only-if-changed 0 --target-component modules modules/mediastream/MediaStreamTrack.idl WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})



# SOURCES FILE LIST

# wtf
ADD_LIBRARY(wtf OBJECT wtf/ASCIICType.cpp wtf/AddressSpaceRandomization.cpp wtf/ArrayBuffer.cpp wtf/ArrayBufferBuilder.cpp wtf/ArrayBufferContents.cpp wtf/ArrayBufferView.cpp wtf/ArrayPiece.cpp wtf/Assertions.cpp wtf/BitVector.cpp wtf/CryptographicallyRandomNumber.cpp wtf/CurrentTime.cpp wtf/DataLog.cpp wtf/DateMath.cpp wtf/DynamicAnnotations.cpp wtf/FilePrintStream.cpp wtf/HashTable.cpp wtf/InstanceCounter.cpp wtf/MainThread.cpp wtf/PageAllocator.cpp wtf/PartitionAlloc.cpp wtf/PartitionAllocator.cpp wtf/Partitions.cpp wtf/PrintStream.cpp wtf/RefCountedLeakCounter.cpp wtf/SpinLock wtf/ThreadingPthreads.cpp wtf/TypeTraits.cpp wtf/WTF.cpp wtf/WTFThreadData.cpp wtf/dtoa.cpp wtf/dtoa/bignum-dtoa.cc wtf/dtoa/bignum.cc wtf/dtoa/cached-powers.cc wtf/dtoa/diy-fp.cc wtf/dtoa/double-conversion.cc wtf/dtoa/fast-dtoa.cc wtf/dtoa/fixed-dtoa.cc wtf/dtoa/strtod.cc wtf/text/AtomicString.cpp wtf/text/AtomicStringCF.cpp wtf/text/Base64.cpp wtf/text/CString.cpp wtf/text/StringBuilder.cpp wtf/text/StringConcatenate.cpp wtf/text/StringImpl.cpp wtf/text/StringImplCF.cpp wtf/text/StringStatics.cpp wtf/text/TextCodecICU.cpp wtf/text/TextCodecLatin1.cpp wtf/text/TextCodecReplacement.cpp wtf/text/TextCodecUTF16.cpp wtf/text/TextCodecUTF8.cpp wtf/text/TextCodecUserDefined.cpp wtf/text/TextEncodingRegistry.cpp wtf/text/TextPosition.cpp wtf/text/UTF8.cpp wtf/text/WTFString.cpp wtf/text/icu/CollatorICU.cpp)

# platform_common
ADD_LIBRARY(platform_common OBJECT platform/exported/WebCString.cpp platform/exported/WebString.cpp platform/exported/WebCommon.cpp)

# platform
ADD_LIBRARY(platform OBJECT platform/Clock.cpp platform/ContentSettingCallbacks.cpp platform/ContentType.cpp platform/ContextMenu.cpp platform/ContextMenuItem.cpp platform/CrossThreadCopier.cpp platform/Crypto.cpp platform/Cursor.cpp platform/DateComponents.cpp platform/Decimal.cpp platform/DragImage.cpp platform/EventDispatchForbiddenScope.cpp platform/EventTracer.cpp platform/FileChooser.cpp platform/FileMetadata.cpp platform/JSONValues.cpp platform/KillRingNone.cpp platform/Language.cpp platform/LayoutTestSupport.cpp platform/Length.cpp platform/LengthBox.cpp platform/LengthFunctions.cpp platform/LinkHash.cpp platform/Logging.cpp platform/MIMETypeFromURL.cpp platform/MIMETypeRegistry.cpp platform/MemoryPurgeController.cpp platform/NotImplemented.cpp platform/PartitionAllocMemoryDumpProvider.cpp platform/PlatformInstrumentation.cpp platform/PlatformKeyboardEvent.cpp platform/PlatformResourceLoader.cpp platform/PluginScriptForbiddenScope.cpp platform/Prerender.cpp platform/PurgeableVector.cpp platform/ScriptForbiddenScope.cpp platform/SecureTextInput.cpp platform/SharedBuffer.cpp platform/SharedBufferChunkReader.cpp platform/Supplementable.cpp platform/TaskSynchronizer.cpp platform/Theme.cpp platform/Timer.cpp platform/TracedValue.cpp platform/UUID.cpp platform/UserGestureIndicator.cpp platform/WebScheduler.cpp platform/WebTaskRunner.cpp platform/WebThreadSupportingGC.cpp platform/WebThread.cpp platform/Widget.cpp platform/animation/CubicBezierControlPoints.cpp platform/animation/TimingFunction.cpp platform/audio/AudioBus.cpp platform/audio/AudioChannel.cpp platform/audio/AudioDSPKernel.cpp platform/audio/AudioDSPKernelProcessor.cpp platform/audio/AudioDelayDSPKernel.cpp platform/audio/AudioDestination.cpp platform/audio/AudioFIFO.cpp platform/audio/AudioProcessor.cpp platform/audio/AudioPullFIFO.cpp platform/audio/AudioResampler.cpp platform/audio/AudioResamplerKernel.cpp platform/audio/AudioUtilities.cpp platform/audio/Biquad.cpp platform/audio/Cone.cpp platform/audio/DirectConvolver.cpp platform/audio/Distance.cpp platform/audio/DownSampler.cpp platform/audio/DynamicsCompressor.cpp platform/audio/EqualPowerPanner.cpp platform/audio/FFTConvolver.cpp platform/audio/FFTFrame.cpp platform/audio/FFTFrameStub.cpp platform/audio/HRTFDatabase.cpp platform/audio/HRTFPanner.cpp platform/audio/MultiChannelResampler.cpp platform/audio/Panner.cpp platform/audio/Reverb.cpp platform/audio/ReverbAccumulationBuffer.cpp platform/audio/ReverbConvolver.cpp platform/audio/ReverbConvolverStage.cpp platform/audio/ReverbInputBuffer.cpp platform/audio/SincResampler.cpp platform/audio/Spatializer.cpp platform/audio/StereoPanner.cpp platform/audio/UpSampler.cpp platform/audio/VectorMath.cpp platform/audio/ZeroPole.cpp platform/blob/BlobData.cpp platform/blob/BlobRegistry.cpp platform/blob/BlobURL.cpp platform/clipboard/ClipboardMimeTypes.cpp platform/clipboard/ClipboardUtilities.cpp platform/clipboard/ClipboardUtilitiesPosix.cpp platform/credentialmanager/PlatformCredential.cpp platform/credentialmanager/PlatformFederatedCredential.cpp platform/credentialmanager/PlatformPasswordCredential.cpp platform/exported/Platform.cpp platform/exported/WebActiveGestureAnimation.cpp platform/exported/WebAudioBus.cpp platform/exported/WebAudioBus.cpp platform/exported/WebAudioDevice.cpp platform/exported/WebBlobData.cpp platform/exported/WebCanvasCaptureHandler.cpp platform/exported/WebCompositorAnimationPlayerClient.cpp platform/exported/WebContentDecryptionModule.cpp platform/exported/WebContentDecryptionModule.cpp platform/exported/WebContentSettingCallbacks.cpp platform/exported/WebConvertableToTraceFormat.cpp platform/exported/WebCredential.cpp platform/exported/WebCryptoAlgorithm.cpp platform/exported/WebCryptoKey.cpp platform/exported/WebCryptoKeyAlgorithm.cpp platform/exported/WebCryptoResult.cpp platform/exported/WebCryptoUtil.cpp platform/exported/WebData.cpp platform/exported/WebDataConsumerHandle.cpp platform/exported/WebDeviceMotionData.cpp platform/exported/WebDeviceOrientationData.cpp platform/exported/WebDragData.cpp platform/exported/WebEncryptedMediaClient.cpp platform/exported/WebFederatedCredential.cpp platform/exported/WebFileSystemCallbacks.cpp platform/exported/WebFilterKeyframe.cpp platform/exported/WebHTTPBody.cpp platform/exported/WebHTTPLoadInfo.cpp platform/exported/WebImageGenerator.cpp platform/exported/WebImage.cpp platform/exported/WebMediaConstraints.cpp platform/exported/WebMediaDeviceInfo.cpp platform/exported/WebMediaPlayerClient.cpp platform/exported/WebMediaPlayerClient.cpp platform/exported/WebMediaPlayerEncryptedMediaClient.cpp platform/exported/WebMediaRecorderHandler.cpp platform/exported/WebMediaStream.cpp platform/exported/WebMediaStreamSource.cpp platform/exported/WebMediaStreamTrack.cpp platform/exported/WebMemoryAllocatorDump.cpp platform/exported/WebMemoryDumpProvider.cpp platform/exported/WebMessagePortChannelClient.cpp platform/exported/WebPasswordCredential.cpp platform/exported/WebPrerender.cpp platform/exported/WebPrerenderingSupport.cpp platform/exported/WebPresentationAvailabilityObserver.cpp platform/exported/WebPresentationController.cpp platform/exported/WebProcessMemoryDump.cpp platform/exported/WebRTCConfiguration.cpp platform/exported/WebRTCICECandidate.cpp platform/exported/WebRTCOfferOptions.cpp platform/exported/WebRTCSessionDescription.cpp platform/exported/WebRTCStatsRequest.cpp platform/exported/WebRTCStatsResponse.cpp platform/exported/WebRTCVoidRequest.cpp platform/exported/WebScrollbarImpl.cpp platform/exported/WebScrollbarThemeClientImpl.cpp platform/exported/WebScrollbarThemeGeometryNative.cpp platform/exported/WebScrollbarThemePainter.cpp platform/exported/WebSecurityOrigin.cpp platform/exported/WebServicePortProviderClient.cpp platform/exported/WebServiceWorkerProviderClient.cpp platform/exported/WebServiceWorkerProxy.cpp platform/exported/WebServiceWorkerRequest.cpp platform/exported/WebServiceWorkerResponse.cpp platform/exported/WebSocketHandshakeRequestInfo.cpp platform/exported/WebSocketHandshakeRequestInfo.cpp platform/exported/WebSourceInfo.cpp platform/exported/WebSkImage.cpp platform/exported/WebSpeechSynthesisUtterance.cpp platform/exported/WebSpeechSynthesisVoice.cpp platform/exported/WebSpeechSynthesizerClientImpl.cpp platform/exported/WebStorageQuotaCallbacks.cpp platform/exported/WebThreadSafeData.cpp platform/exported/WebThreadedDataReceiver.cpp platform/exported/WebTraceLocation.cpp platform/exported/WebTransformKeyframe.cpp platform/exported/WebURL.cpp platform/exported/WebURLError.cpp platform/exported/WebURLLoaderClient.cpp platform/exported/WebURLLoaderClient.cpp platform/exported/WebURLLoaderTestDelegate.cpp platform/exported/WebURLLoadTiming.cpp platform/exported/WebURLRequest.cpp platform/exported/WebURLResponse.cpp platform/exported/linux/WebFontInfo.cpp platform/exported/linux/WebFontRenderStyle.cpp platform/fonts/Character.cpp platform/fonts/Font.cpp platform/fonts/FontCache.cpp platform/fonts/FontCacheMemoryDumpProvider.cpp platform/fonts/FontCustomPlatformData.cpp platform/fonts/FontData.cpp platform/fonts/FontDataCache.cpp platform/fonts/FontDescription.cpp platform/fonts/FontFallbackIterator.cpp platform/fonts/FontFallbackList.cpp platform/fonts/FontFamily.cpp platform/fonts/FontFeatureSettings.cpp platform/fonts/FontPlatformData.cpp platform/fonts/GenericFontFamilySettings.cpp platform/fonts/GlyphPageTreeNode.cpp platform/fonts/OrientationIterator.cpp platform/fonts/ScriptRunIterator.cpp platform/fonts/SegmentedFontData.cpp platform/fonts/SimpleFontData.cpp platform/fonts/SmallCapsIterator.cpp platform/fonts/UTF16TextIterator.cpp platform/fonts/VDMXParser.cpp platform/fonts/opentype/OpenTypeSanitizer.cpp platform/fonts/opentype/OpenTypeVerticalData.cpp platform/fonts/linux/FontCacheLinux.cpp platform/fonts/linux/FontPlatformDataLinux.cpp platform/fonts/shaping/CachingWordShaper.cpp platform/fonts/shaping/HarfBuzzFace.cpp platform/fonts/shaping/HarfBuzzShaper.cpp platform/fonts/shaping/RunSegmenter.cpp platform/fonts/shaping/ShapeResult.cpp platform/fonts/shaping/ShapeResultTestInfo.cpp platform/fonts/shaping/Shaper.cpp platform/fonts/shaping/SimpleShaper.cpp platform/fonts/skia/FontCacheSkia.cpp platform/geometry/DoublePoint.cpp platform/geometry/DoubleRect.cpp platform/geometry/DoubleSize.cpp platform/geometry/FloatPoint.cpp platform/geometry/FloatPoint3D.cpp platform/geometry/FloatPolygon.cpp platform/geometry/FloatQuad.cpp platform/geometry/FloatRect.cpp platform/geometry/FloatRoundedRect.cpp platform/geometry/FloatSize.cpp platform/geometry/IntRect.cpp platform/geometry/LayoutRectOutsets.cpp platform/geometry/LayoutRect.cpp platform/geometry/Region.cpp platform/geometry/TransformState.cpp platform/graphics/BitmapImage.cpp platform/graphics/CanvasMetrics.cpp platform/graphics/Color.cpp platform/graphics/ColorSpace.cpp platform/graphics/CompositingReasons.cpp platform/graphics/ContentLayerDelegate.cpp platform/graphics/ContiguousContainer.cpp platform/graphics/CrossfadeGeneratedImage.cpp platform/graphics/DecodingImageGenerator.cpp platform/graphics/DeferredImageDecoder.cpp platform/graphics/PicturePattern.cpp platform/graphics/PictureSnapshot.cpp platform/graphics/DrawLooperBuilder.cpp platform/graphics/FirstPaintInvalidationTracking.cpp platform/graphics/FrameData.cpp platform/graphics/GeneratedImage.cpp platform/graphics/Gradient.cpp platform/graphics/GradientGeneratedImage.cpp platform/graphics/GraphicsContext.cpp platform/graphics/GraphicsContextState.cpp platform/graphics/GraphicsLayer.cpp platform/graphics/GraphicsLayerDebugInfo.cpp platform/graphics/GraphicsTypes.cpp platform/graphics/ImageBuffer.cpp platform/graphics/Image.cpp platform/graphics/ImageBuffer.cpp platform/graphics/ImageBufferSurface.cpp platform/graphics/ImageDecodingStore.cpp platform/graphics/ImageFrameGenerator.cpp platform/graphics/ImageObserver.cpp platform/graphics/ImageOrientation.cpp platform/graphics/ImagePattern.cpp platform/graphics/ImageSource.cpp platform/graphics/InterceptingCanvas.cpp platform/graphics/LoggingCanvas.cpp platform/graphics/PaintInvalidationReason.cpp platform/graphics/Path.cpp platform/graphics/PathTraversalState.cpp platform/graphics/Pattern.cpp platform/graphics/ProfilingCanvas.cpp platform/graphics/RecordingImageBufferSurface.cpp platform/graphics/ReplayingCanvas.cpp platform/graphics/StaticBitmapImage.cpp platform/graphics/StrokeData.cpp platform/graphics/ThreadSafeDataTransport.cpp platform/graphics/UnacceleratedImageBufferSurface.cpp platform/graphics/compositing/PaintArtifactCompositor.cpp platform/graphics/filters/DistantLightSource.cpp platform/graphics/filters/FEBlend.cpp platform/graphics/filters/FEColorMatrix.cpp platform/graphics/filters/FEComponentTransfer.cpp platform/graphics/filters/FEComposite.cpp platform/graphics/filters/FEConvolveMatrix.cpp platform/graphics/filters/FEDiffuseLighting.cpp platform/graphics/filters/FEDisplacementMap.cpp platform/graphics/filters/FEDropShadow.cpp platform/graphics/filters/FEFlood.cpp platform/graphics/filters/FEGaussianBlur.cpp platform/graphics/filters/FEMerge.cpp platform/graphics/filters/FEMorphology.cpp platform/graphics/filters/FEOffset.cpp platform/graphics/filters/FESpecularLighting.cpp platform/graphics/filters/FETile.cpp platform/graphics/filters/FETurbulence.cpp platform/graphics/filters/Filter.cpp platform/graphics/filters/FilterEffect.cpp platform/graphics/filters/FilterOperation.cpp platform/graphics/filters/FilterOperations.cpp platform/graphics/filters/LightSource.cpp platform/graphics/filters/PointLightSource.cpp platform/graphics/filters/SkiaImageFilterBuilder.cpp platform/graphics/filters/SourceAlpha.cpp platform/graphics/filters/SourceGraphic.cpp platform/graphics/filters/SpotLightSource.cpp platform/graphics/gpu/AcceleratedImageBufferSurface.cpp platform/graphics/gpu/SharedContextRateLimiter.cpp platform/graphics/gpu/DrawingBuffer.cpp platform/graphics/gpu/Extensions3DUtil.cpp platform/graphics/gpu/WebGLImageConversion.cpp platform/graphics/paint/ClipDisplayItem.cpp platform/graphics/paint/ClipPathDisplayItem.cpp platform/graphics/paint/ClipPathRecorder.cpp platform/graphics/paint/ClipRecorder.cpp platform/graphics/paint/CullRect.cpp platform/graphics/paint/DisplayItem.cpp platform/graphics/paint/DrawingDisplayItem.cpp platform/graphics/paint/DrawingRecorder.cpp platform/graphics/paint/FilterDisplayItem.cpp platform/graphics/paint/FixedPositionContainerDisplayItem.cpp platform/graphics/paint/FixedPositionDisplayItem.cpp platform/graphics/paint/FloatClipDisplayItem.cpp platform/graphics/paint/PaintArtifact.cpp platform/graphics/paint/PaintArtifactToSkCanvas.cpp platform/graphics/paint/PaintChunker.cpp platform/graphics/paint/PaintController.cpp platform/graphics/paint/ScrollDisplayItem.cpp platform/graphics/paint/SubsequenceRecorder.cpp platform/graphics/paint/Transform3DDisplayItem.cpp platform/graphics/paint/CompositingDisplayItem.cpp platform/graphics/skia/ImagePixelLocker.cpp platform/graphics/skia/SkiaUtils.cpp platform/image-decoders/FastSharedBufferReader.cpp platform/image-decoders/ImageDecoder.cpp platform/image-decoders/ImageFrame.cpp platform/image-decoders/bmp/BMPImageDecoder.cpp platform/image-decoders/bmp/BMPImageReader.cpp platform/image-decoders/gif/GIFImageDecoder.cpp platform/image-decoders/gif/GIFImageReader.cpp platform/image-decoders/ico/ICOImageDecoder.cpp platform/image-decoders/jpeg/JPEGImageDecoder.cpp platform/image-decoders/png/PNGImageDecoder.cpp platform/image-decoders/webp/WEBPImageDecoder.cpp platform/image-encoders/skia/JPEGImageEncoder.cpp platform/image-encoders/skia/PNGImageEncoder.cpp platform/image-encoders/skia/WEBPImageEncoder.cpp platform/mediastream/MediaStreamCenter.cpp platform/mediastream/MediaStreamComponent.cpp platform/mediastream/MediaStreamDescriptor.cpp platform/mediastream/MediaStreamSource.cpp platform/mediastream/MediaStreamWebAudioSource.cpp platform/mhtml/ArchiveResource.cpp platform/mhtml/ArchiveResourceCollection.cpp platform/mhtml/MHTMLArchive.cpp platform/mhtml/MHTMLParser.cpp platform/network/ContentSecurityPolicyParsers.cpp platform/network/ContentSecurityPolicyResponseHeaders.cpp platform/network/EncodedFormData.cpp platform/network/FormDataEncoder.cpp platform/network/HTTPHeaderMap.cpp platform/network/HTTPParsers.cpp platform/network/NetworkHints.cpp platform/network/ParsedContentType.cpp platform/network/ResourceError.cpp platform/network/ResourceLoadTiming.cpp platform/network/ResourceRequest.cpp platform/network/ResourceResponse.cpp platform/network/ResourceTimingInfo.cpp platform/network/WebSocketHandshakeRequest.cpp platform/network/WebSocketHandshakeResponse.cpp platform/plugins/PluginData.cpp platform/plugins/PluginListBuilder.cpp platform/scheduler/CancellableTaskFactory.cpp platform/scroll/ProgrammaticScrollAnimator.cpp platform/scroll/ScrollAnimatorBase.cpp platform/scroll/ScrollAnimator.cpp platform/scroll/ScrollableArea.cpp platform/scroll/Scrollbar.cpp platform/scroll/ScrollbarTheme.cpp platform/scroll/ScrollbarThemeAura.cpp platform/scroll/ScrollbarThemeOverlay.cpp platform/speech/PlatformSpeechSynthesisUtterance.cpp platform/speech/PlatformSpeechSynthesisVoice.cpp platform/speech/PlatformSpeechSynthesizer.cpp platform/text/BidiCharacterRun.cpp platform/text/BidiContext.cpp platform/text/BidiTextRun.cpp platform/text/DateTimeFormat.cpp platform/text/LineEnding.cpp platform/text/LocaleICU.cpp platform/text/LocaleToScriptMapping.cpp platform/text/PlatformLocale.cpp platform/text/QuotedPrintable.cpp platform/text/SegmentedString.cpp platform/text/StringTruncator.cpp platform/text/TextBoundaries.cpp platform/text/TextBreakIterator.cpp platform/text/TextBreakIteratorICU.cpp platform/text/TextBreakIteratorInternalICU.cpp platform/text/TextCheckerClient.cpp platform/text/TextEncodingDetector.cpp platform/text/TextRun.cpp platform/text/TextStream.cpp platform/text/UnicodeRange.cpp platform/text/UnicodeRange.cpp platform/text/UnicodeUtilities.cpp platform/transforms/AffineTransform.cpp platform/transforms/InterpolatedTransformOperation.cpp platform/transforms/Matrix3DTransformOperation.cpp platform/transforms/MatrixTransformOperation.cpp platform/transforms/PerspectiveTransformOperation.cpp platform/transforms/RotateTransformOperation.cpp platform/transforms/ScaleTransformOperation.cpp platform/transforms/SkewTransformOperation.cpp platform/transforms/TransformOperations.cpp platform/transforms/TransformationMatrix.cpp platform/transforms/TranslateTransformOperation.cpp platform/weborigin/DatabaseIdentifier.cpp platform/weborigin/KURL.cpp platform/weborigin/KnownPorts.cpp platform/weborigin/OriginAccessEntry.cpp platform/weborigin/SchemeRegistry.cpp platform/weborigin/SecurityOrigin.cpp platform/weborigin/SecurityPolicy.cpp)

# platform_heap
ADD_LIBRARY(platform_heap OBJECT platform/heap/BlinkGCInterruptor.cpp platform/heap/BlinkGCMemoryDumpProvider.cpp platform/heap/CallbackStack.cpp platform/heap/GCInfo.cpp platform/heap/HeapAllocator.cpp platform/heap/HeapPage.cpp platform/heap/PagePool.cpp platform/heap/PersistentNode.cpp platform/heap/StackFrameDepth.cpp platform/heap/ThreadState.cpp)

# web
ADD_LIBRARY(web OBJECT web/AssertMatchingEnums.cpp web/AssociatedURLLoader.cpp web/AudioOutputDeviceClientImpl.cpp web/ChromeClientImpl.cpp web/ColorChooserPopupUIController.cpp web/ColorChooserUIController.cpp web/CompositionUnderlineVectorBuilder.cpp web/ContextFeaturesClientImpl.cpp web/ContextMenuAllowedScope.cpp web/ContextMenuClientImpl.cpp web/DatabaseClientImpl.cpp web/DateTimeChooserImpl.cpp web/DevToolsEmulator.cpp web/DragClientImpl.cpp web/EditorClientImpl.cpp web/ExternalDateTimeChooser.cpp web/ExternalPopupMenu.cpp web/FindInPageCoordinates.cpp web/FrameLoaderClientImpl.cpp web/FullscreenController.cpp web/GeolocationClientProxy.cpp web/GraphicsLayerFactoryChromium.cpp web/IndexedDBClientImpl.cpp web/InspectorEmulationAgent.cpp web/InspectorOverlay.cpp web/InspectorRenderingAgent.cpp web/LinkHighlightImpl.cpp web/LocalFileSystemClient.cpp web/MIDIClientProxy.cpp web/MediaKeysClientImpl.cpp web/NavigatorContentUtilsClientImpl.cpp web/NotificationPermissionClientImpl.cpp web/OpenedFrameTracker.cpp web/PageOverlay.cpp web/PageWidgetDelegate.cpp web/PopupMenuImpl.cpp web/PrerendererClientImpl.cpp web/RemoteBridgeFrameOwner.cpp web/RemoteFrameClientImpl.cpp web/ResizeViewportAnchor.cpp web/ServiceWorkerGlobalScopeClientImpl.cpp web/ServiceWorkerGlobalScopeProxy.cpp web/SharedWorkerRepositoryClientImpl.cpp web/SpeechRecognitionClientProxy.cpp web/SpellCheckerClientImpl.cpp web/StorageQuotaClientImpl.cpp web/SuspendableScriptExecutor.cpp web/TextFinder.cpp web/UserMediaClientImpl.cpp web/ValidationMessageClientImpl.cpp web/WebAXObject.cpp web/WebArrayBuffer.cpp web/WebArrayBufferConverter.cpp web/WebArrayBufferView.cpp web/WebBindings.cpp web/WebBlob.cpp web/WebCache.cpp web/WebColorSuggestion.cpp web/WebCryptoNormalize.cpp web/WebCustomElement.cpp web/WebCSSParser.cpp web/WebDOMActivityLogger.cpp web/WebDOMEvent.cpp web/WebDOMFileSystem.cpp web/WebDOMMediaStreamTrack.cpp web/WebDOMMessageEvent.cpp web/WebDataSourceImpl.cpp web/WebDatabase.cpp web/WebDateTimeSuggestion.cpp web/WebDevToolsAgentImpl.cpp web/WebDevToolsFrontendImpl.cpp web/WebDocument.cpp web/WebElement.cpp web/WebElementCollection.cpp web/WebEmbeddedWorkerImpl.cpp web/WebEntities.cpp web/WebFileChooserCompletionImpl.cpp web/WebFontDescription.cpp web/WebFontImpl.cpp web/WebFormControlElement.cpp web/WebFormElement.cpp web/WebFrameImplBase.cpp web/WebFrame.cpp web/WebFrameWidgetImpl.cpp web/WebGeolocationController.cpp web/WebGeolocationError.cpp web/WebGeolocationPermissionRequest.cpp web/WebGeolocationPermissionRequestManager.cpp web/WebGeolocationPosition.cpp web/WebHeap.cpp web/WebHelperPluginImpl.cpp web/WebHistoryItem.cpp web/WebHitTestResult.cpp web/WebIDBKey.cpp web/WebIDBKeyRange.cpp web/WebImageCache.cpp web/WebImageDecoder.cpp web/WebInputEventConversion.cpp web/WebLabelElement.cpp web/WebLeakDetector.cpp web/WebLocalFrameImpl.cpp web/WebMIDIPermissionRequest.cpp web/WebMediaDevicesRequest.cpp web/WebMediaStreamRegistry.cpp web/WebMetaElement.cpp web/WebMemoryPressureListener.cpp web/WebNetworkStateNotifier.cpp web/WebNode.cpp web/WebOptionElement.cpp web/WebPageImportanceSignals.cpp web/WebPagePopupImpl.cpp web/WebPageSerializer.cpp web/WebPageSerializerImpl.cpp web/WebPerformance.cpp web/WebPluginContainerImpl.cpp web/WebPluginDocument.cpp web/WebPluginScriptForbiddenScope.cpp web/WebRange.cpp web/WebRemoteFrameImpl.cpp web/WebRuntimeFeatures.cpp web/WebScopedMicrotaskSuppression.cpp web/WebScopedUserGesture.cpp web/WebScopedWindowFocusAllowedIndicator.cpp web/WebScriptBindings.cpp web/WebScriptController.cpp web/WebScriptSource.cpp web/WebSearchableFormData.cpp web/WebSecurityPolicy.cpp web/WebSelectElement.cpp web/WebSelection.cpp web/WebSelector.cpp web/WebSerializedScriptValue.cpp web/WebSettingsImpl.cpp web/WebSharedWorkerImpl.cpp web/WebSocket.cpp web/WebSocketImpl.cpp web/WebSpeechGrammar.cpp web/WebSpeechRecognitionHandle.cpp web/WebStorageEventDispatcherImpl.cpp web/WebSurroundingText.cpp web/WebTextCheckingCompletionImpl.cpp web/WebTextCheckingResult.cpp web/WebTextInputInfo.cpp web/WebTextRun.cpp web/WebUserGestureIndicator.cpp web/WebUserGestureToken.cpp web/WebUserMediaRequest.cpp web/WebViewFrameWidget.cpp web/WebViewImpl.cpp web/WorkerContentSettingsClient.cpp web/WorkerGlobalScopeProxyProviderImpl.cpp web/default/WebRenderTheme.cpp web/linux/WebFontRendering.cpp web/WebKit.cpp)

# qcms
ADD_LIBRARY(qcms OBJECT ${QCMS_DIR}/src/chain.c ${QCMS_DIR}/src/iccread.c ${QCMS_DIR}/src/matrix.c ${QCMS_DIR}/src/transform.c ${QCMS_DIR}/src/transform_util.c)

# ots
ADD_LIBRARY(ots OBJECT ${OTS_DIR}/src/cff.cc ${OTS_DIR}/src/cff_type2_charstring.cc ${OTS_DIR}/src/cmap.cc ${OTS_DIR}/src/cvt.cc ${OTS_DIR}/src/fpgm.cc ${OTS_DIR}/src/gasp.cc ${OTS_DIR}/src/gdef.cc ${OTS_DIR}/src/glyf.cc ${OTS_DIR}/src/gpos.cc ${OTS_DIR}/src/gsub.cc ${OTS_DIR}/src/hdmx.cc ${OTS_DIR}/src/head.cc ${OTS_DIR}/src/hhea.cc ${OTS_DIR}/src/hmtx.cc ${OTS_DIR}/src/kern.cc ${OTS_DIR}/src/layout.cc ${OTS_DIR}/src/loca.cc ${OTS_DIR}/src/ltsh.cc ${OTS_DIR}/src/maxp.cc ${OTS_DIR}/src/math.cc ${OTS_DIR}/src/metrics.cc ${OTS_DIR}/src/name.cc ${OTS_DIR}/src/os2.cc ${OTS_DIR}/src/post.cc ${OTS_DIR}/src/prep.cc ${OTS_DIR}/src/vdmx.cc ${OTS_DIR}/src/vhea.cc ${OTS_DIR}/src/vmtx.cc ${OTS_DIR}/src/vorg.cc ${OTS_DIR}/src/ots.cc)

# iccjpeg
ADD_LIBRARY(iccjpeg OBJECT ${ICCJPEG_DIR}/iccjpeg.c)

# generated (these are generated !)
ADD_LIBRARY(generated OBJECT ${CMAKE_BINARY_DIR}/core/HTMLEntityTable.cpp ${CMAKE_BINARY_DIR}/core/CSSPropertyNames.cpp ${CMAKE_BINARY_DIR}/platform/FontFamilyNames.cpp ${CMAKE_BINARY_DIR}/platform/RuntimeEnabledFeatures.cpp ${CMAKE_BINARY_DIR}/platform/ColorData.cpp)


# all files refer to "ui/...", so we include "../.." here
# we include CMAKE_BINARY_DIR because generated .cc/.h files are inside
SET(include_dirs ${INCLUDE_DIRS} ${icu_INCLUDE_DIRS} ${harfbuzz_INCLUDE_DIRS} ${libwebp_INCLUDE_DIRS} ${BASE_DIR}/.. ${V8_DIR}/include ${SKIA_EXT_DIR}/config ${SKIA_SOURCE_DIR}/include/core ${SKIA_SOURCE_DIR}/include/utils ${SKIA_SOURCE_DIR}/include/ports ${SKIA_SOURCE_DIR}/include/gpu ${SKIA_SOURCE_DIR}/include/effects ${CHROMIUM_GL_DIR} ${CHROMIUM_GLES2_EXT_DIR} ${QCMS_DIR}/src ${OTS_DIR}/include ${ICCJPEG_DIR} ${NPAPI_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/bindings/core/v8 ${CMAKE_BINARY_DIR}/bindings/modules/v8 ${CMAKE_SOURCE_DIR}/wtf ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${icu_LIBRARIES} ${harfbuzz_LIBRARIES} ${libwebp_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} -ldl)

ADD_LIBRARY(blink SHARED $<TARGET_OBJECTS:wtf> $<TARGET_OBJECTS:platform_common> $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:platform_heap> $<TARGET_OBJECTS:web> $<TARGET_OBJECTS:qcms> $<TARGET_OBJECTS:ots> $<TARGET_OBJECTS:iccjpeg> $<TARGET_OBJECTS:generated> ${TOOLCHAIN_OPTS_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(blink ${link_libraries})

INSTALL(TARGETS blink
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/chromium)
