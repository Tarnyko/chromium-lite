PROJECT(chromium-v8 CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.8)
SET(CMAKE_BUILD_TYPE Debug)
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

SET(PROJECT_NAME "Chromium - V8")

# Verify the Toolchain file has been passed
IF(NOT CMAKE_TOOLCHAIN_FILE)
  MESSAGE(FATAL_ERROR "Please pass \"-DCMAKE_TOOLCHAIN_FILE=../Toolchain-<arch>.cmake\" when running cmake !")
ENDIF(NOT CMAKE_TOOLCHAIN_FILE)
MESSAGE(STATUS "Chosen Toolchain: ${CMAKE_TOOLCHAIN_FILE}")

# CMake modules
INCLUDE(FindPkgConfig)
INCLUDE(GNUInstallDirs)

# Mandatory dependencies
PKG_CHECK_MODULES(icui18n REQUIRED icu-i18n)

FIND_PROGRAM(PYTHON_FOUND python)
IF(NOT PYTHON_FOUND)
  MESSAGE(FATAL_ERROR "\"python\" is required, please install it !")
ELSE()
  MESSAGE(STATUS "\"python\" found...")
ENDIF(NOT PYTHON_FOUND)

# Common definitions
ADD_DEFINITIONS(-DV8_SHARED -DBUILDING_V8_SHARED -DUSING_V8_SHARED)
ADD_DEFINITIONS(-DV8_IMMINENT_DEPRECATION_WARNINGS -DV8_LIBRT_NOT_AVAILABLE=1)

IF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-std=c++11)
ENDIF()


# PRE-GENERATION .JS -> .CC

MESSAGE(STATUS "Generating .cc files from .js...")

EXECUTE_PROCESS(COMMAND ./tools/js2c.py ${CMAKE_BINARY_DIR}/libraries.cc CORE src/messages.h src/js/macros.py src/js/prologue.js src/js/runtime.js src/js/v8natives.js src/js/array.js src/js/string.js src/js/string.js src/js/arraybuffer.js src/js/typedarray.js src/js/collection.js src/js/weak-collection.js src/js/collection-iterator.js src/js/promise.js src/js/messages.js src/js/templates.js src/js/spread.js src/debug/mirrors.js src/debug/debug.js src/debug/liveedit.js WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
# we let "src/js/i18n.js" out for now, it causes bugs...

EXECUTE_PROCESS(COMMAND ./tools/js2c.py ${CMAKE_BINARY_DIR}/experimental-libraries.cc EXPERIMENTAL src/messages.h src/js/macros.py src/js/proxy.js src/js/harmony-atomics.js src/js/harmony-simd.js WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ./tools/js2c.py ${CMAKE_BINARY_DIR}/extras-libraries.cc EXTRAS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

EXECUTE_PROCESS(COMMAND ./tools/js2c.py ${CMAKE_BINARY_DIR}/experimental-extras-libraries.cc EXPERIMENTAL_EXTRAS WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})


# SOURCES FILE LIST, PER SECTION

# core
ADD_LIBRARY(core OBJECT src/accessors.cc src/address-map.cc src/allocation.cc src/allocation-site-scopes.cc src/api-arguments.cc src/api-experimental.cc src/api.cc src/api-natives.cc src/arguments.cc src/assembler.cc src/assert-scope.cc src/background-parsing-task.cc src/bailout-reason.cc src/basic-block-profiler.cc src/bignum-dtoa.cc src/bignum.cc src/bit-vector.cc src/bootstrapper.cc src/cached-powers.cc src/cancelable-task.cc src/char-predicates.cc src/code-factory.cc src/code-stub-assembler.cc src/code-stubs.cc src/code-stubs-hydrogen.cc src/codegen.cc src/compilation-cache.cc src/compilation-dependencies.cc src/compilation-info.cc src/compilation-statistics.cc src/context-measure.cc src/contexts.cc src/conversions.cc src/counters.cc src/date.cc src/dateparser.cc src/deoptimize-reason.cc src/deoptimizer.cc src/disassembler.cc src/diy-fp.cc src/dtoa.cc src/eh-frame.cc src/elements-kind.cc src/elements.cc src/execution.cc src/external-reference-table.cc src/factory.cc src/fast-accessor-assembler.cc src/fast-dtoa.cc src/ffi/ffi-compiler.cc src/field-type.cc src/fixed-dtoa.cc src/flags.cc src/frames.cc src/full-codegen/full-codegen.cc src/futex-emulation.cc src/gdb-jit.cc src/global-handles.cc src/handles.cc src/identity-map.cc src/interface-descriptors.cc src/isolate.cc src/keys.cc src/layout-descriptor.cc src/log-utils.cc src/log.cc src/lookup.cc src/lookup-cache.cc src/machine-type.cc src/map-updater.cc src/messages.cc src/objects-debug.cc src/objects-printer.cc src/objects.cc src/objects/literal-objects.cc src/objects/scope-info.cc src/ostreams.cc src/pending-compilation-error-handler.cc src/perf-jit.cc src/property-descriptor.cc src/property.cc src/register-configuration.cc src/runtime-profiler.cc src/safepoint-table.cc src/snapshot/natives-common.cc src/snapshot/snapshot-common.cc src/snapshot/snapshot-source-sink.cc src/snapshot/deserializer.cc src/snapshot/code-serializer.cc src/snapshot/partial-serializer.cc src/snapshot/startup-serializer.cc src/snapshot/serializer-common.cc src/snapshot/serializer.cc src/source-position.cc src/source-position-table.cc src/startup-data-util.cc src/string-builder.cc src/string-case.cc src/string-stream.cc src/strtod.cc src/transitions.cc src/type-feedback-vector.cc src/type-hints.cc src/type-info.cc src/unicode.cc src/unicode-decoder.cc src/utils.cc src/version.cc src/i18n.cc src/icu_util.cc src/json-parser.cc src/json-stringifier.cc src/uri.cc src/value-serializer.cc src/v8threads.cc src/v8.cc)

# base
ADD_LIBRARY(base OBJECT src/base/bits.cc src/base/cpu.cc src/base/division-by-constant.cc src/base/functional.cc src/base/debug/stack_trace.cc src/base/debug/stack_trace_posix.cc src/base/ieee754.cc src/base/logging.cc src/base/once.cc src/base/platform/time.cc src/base/platform/condition-variable.cc src/base/platform/mutex.cc src/base/platform/semaphore.cc src/base/sys-info.cc src/base/utils/random-number-generator.cc src/base/platform/platform-linux.cc src/base/platform/platform-posix.cc)

# builtins
ADD_LIBRARY(builtins OBJECT src/builtins/builtins-api.cc src/builtins/builtins-array.cc src/builtins/builtins-arraybuffer.cc src/builtins/builtins-async.cc src/builtins/builtins-async-function.cc src/builtins/builtins-boolean.cc src/builtins/builtins-call.cc src/builtins/builtins-callsite.cc src/builtins/builtins-constructor.cc src/builtins/builtins-conversion.cc src/builtins/builtins-dataview.cc src/builtins/builtins-date.cc src/builtins/builtins-debug.cc src/builtins/builtins-error.cc src/builtins/builtins-function.cc src/builtins/builtins-generator.cc src/builtins/builtins-global.cc src/builtins/builtins-handler.cc src/builtins/builtins-ic.cc src/builtins/builtins-internal.cc src/builtins/builtins-interpreter.cc src/builtins/builtins-json.cc src/builtins/builtins-math.cc src/builtins/builtins-number.cc src/builtins/builtins-object.cc src/builtins/builtins-promise.cc src/builtins/builtins-proxy.cc src/builtins/builtins-reflect.cc src/builtins/builtins-regexp.cc src/builtins/builtins-sharedarraybuffer.cc src/builtins/builtins-string.cc src/builtins/builtins-symbol.cc src/builtins/builtins-typedarray.cc src/builtins/builtins.cc)

# platform
ADD_LIBRARY(platform OBJECT src/libplatform/default-platform.cc src/libplatform/task-queue.cc src/libplatform/tracing/trace-config.cc src/libplatform/tracing/trace-object.cc src/libplatform/tracing/tracing-controller.cc src/libplatform/worker-thread.cc)

# asmjs
ADD_LIBRARY(asmjs OBJECT src/asmjs/switch-logic.cc src/asmjs/asm-typer.cc src/asmjs/asm-types.cc src/asmjs/asm-wasm-builder.cc src/asmjs/asm-js.cc)

# ast
ADD_LIBRARY(ast OBJECT src/ast/ast.cc src/ast/ast-expression-rewriter.cc src/ast/ast-function-literal-id-reindexer.cc src/ast/ast-numbering.cc src/ast/ast-types.cc src/ast/ast-value-factory.cc src/ast/compile-time-value.cc src/ast/context-slot-cache.cc src/ast/modules.cc src/ast/prettyprinter.cc src/ast/scopes.cc src/ast/variables.cc)

# compiler
ADD_LIBRARY(compiler OBJECT src/compiler/access-builder.cc src/compiler/access-info.cc src/compiler/all-nodes.cc src/compiler/ast-graph-builder.cc src/compiler/ast-loop-assignment-analyzer.cc src/compiler/basic-block-instrumentor.cc src/compiler/branch-elimination.cc src/compiler/bytecode-analysis.cc src/compiler/bytecode-graph-builder.cc src/compiler/bytecode-liveness-map.cc src/compiler/c-linkage.cc src/compiler/checkpoint-elimination.cc src/compiler/code-assembler.cc src/compiler/code-generator.cc src/compiler/common-node-cache.cc src/compiler/common-operator-reducer.cc src/compiler/common-operator.cc src/compiler/compiler-source-position-table.cc src/compiler/control-builders.cc src/compiler/control-equivalence.cc src/compiler/control-flow-optimizer.cc src/compiler/dead-code-elimination.cc src/compiler/effect-control-linearizer.cc src/compiler/escape-analysis.cc src/compiler/escape-analysis-reducer.cc src/compiler/frame.cc src/compiler/frame-elider.cc src/compiler/frame-states.cc src/compiler/gap-resolver.cc src/compiler/graph-assembler.cc src/compiler/graph-reducer.cc src/compiler/graph-replay.cc src/compiler/graph-trimmer.cc src/compiler/graph-visualizer.cc src/compiler/graph.cc src/compiler/instruction-selector.cc src/compiler/instruction-scheduler.cc src/compiler/instruction.cc src/compiler/int64-lowering.cc src/compiler/js-builtin-reducer.cc src/compiler/js-call-reducer.cc src/compiler/js-context-specialization.cc src/compiler/js-create-lowering.cc src/compiler/js-frame-specialization.cc src/compiler/js-generic-lowering.cc src/compiler/js-graph.cc src/compiler/js-inlining.cc src/compiler/js-inlining-heuristic.cc src/compiler/js-intrinsic-lowering.cc src/compiler/js-native-context-specialization.cc src/compiler/js-operator.cc src/compiler/js-typed-lowering.cc src/compiler/jump-threading.cc src/compiler/linkage.cc src/compiler/liveness-analyzer.cc src/compiler/live-range-separator.cc src/compiler/load-elimination.cc src/compiler/loop-analysis.cc src/compiler/loop-peeling.cc src/compiler/loop-variable-optimizer.cc src/compiler/machine-graph-verifier.cc src/compiler/machine-operator-reducer.cc src/compiler/machine-operator.cc src/compiler/memory-optimizer.cc src/compiler/move-optimizer.cc src/compiler/node-cache.cc src/compiler/node-marker.cc src/compiler/node-matchers.cc src/compiler/node-properties.cc src/compiler/node.cc src/compiler/opcodes.cc src/compiler/operation-typer.cc src/compiler/operator-properties.cc src/compiler/operator.cc src/compiler/osr.cc src/compiler/pipeline.cc src/compiler/pipeline-statistics.cc src/compiler/raw-machine-assembler.cc src/compiler/redundancy-elimination.cc src/compiler/register-allocator.cc src/compiler/register-allocator-verifier.cc src/compiler/representation-change.cc src/compiler/schedule.cc src/compiler/scheduler.cc src/compiler/select-lowering.cc src/compiler/simd-scalar-lowering.cc src/compiler/simplified-lowering.cc src/compiler/simplified-operator-reducer.cc src/compiler/simplified-operator.cc src/compiler/state-values-utils.cc src/compiler/store-store-elimination.cc src/compiler/tail-call-optimization.cc src/compiler/type-cache.cc src/compiler/typed-optimization.cc src/compiler/typer.cc src/compiler/types.cc src/compiler/value-numbering-reducer.cc src/compiler/verifier.cc src/compiler/zone-stats.cc src/compiler/wasm-linkage.cc src/compiler/wasm-compiler.cc src/compiler.cc)

# compiler-dispatcher
ADD_LIBRARY(compilerdispatcher OBJECT src/compiler-dispatcher/optimizing-compile-dispatcher.cc src/compiler-dispatcher/compiler-dispatcher-job.cc src/compiler-dispatcher/compiler-dispatcher-tracer.cc src/compiler-dispatcher/compiler-dispatcher.cc)

# crankshaft
ADD_LIBRARY(crankshaft OBJECT src/crankshaft/hydrogen-bce.cc src/crankshaft/hydrogen-canonicalize.cc src/crankshaft/hydrogen-check-elimination.cc src/crankshaft/hydrogen-dce.cc src/crankshaft/hydrogen-dehoist.cc src/crankshaft/hydrogen-environment-liveness.cc src/crankshaft/hydrogen-escape-analysis.cc src/crankshaft/hydrogen-gvn.cc src/crankshaft/hydrogen-infer-representation.cc src/crankshaft/hydrogen-infer-types.cc src/crankshaft/hydrogen-instructions.cc src/crankshaft/hydrogen-load-elimination.cc src/crankshaft/hydrogen-mark-unreachable.cc src/crankshaft/hydrogen-osr.cc src/crankshaft/hydrogen-range-analysis.cc src/crankshaft/hydrogen-redundant-phi.cc src/crankshaft/hydrogen-removable-simulates.cc src/crankshaft/hydrogen-representation-changes.cc src/crankshaft/hydrogen-sce.cc src/crankshaft/hydrogen-store-elimination.cc src/crankshaft/hydrogen-types.cc src/crankshaft/hydrogen-uint32-analysis.cc src/crankshaft/hydrogen.cc src/crankshaft/lithium-allocator.cc src/crankshaft/lithium-codegen.cc src/crankshaft/lithium.cc src/crankshaft/typing.cc src/crankshaft/compilation-phase.cc)

# debug
ADD_LIBRARY(debug OBJECT src/debug/debug-evaluate.cc src/debug/debug-frames.cc src/debug/debug-scopes.cc src/debug/liveedit.cc src/debug/debug.cc)

# extensions
ADD_LIBRARY(extensions OBJECT src/extensions/externalize-string-extension.cc src/extensions/free-buffer-extension.cc src/extensions/gc-extension.cc src/extensions/statistics-extension.cc src/extensions/ignition-statistics-extension.cc src/extensions/trigger-failure-extension.cc)

# heap
ADD_LIBRARY(heap OBJECT src/heap/array-buffer-tracker.cc src/heap/code-stats.cc src/heap/embedder-tracing.cc src/heap/memory-reducer.cc src/heap/gc-idle-time-handler.cc src/heap/gc-tracer.cc src/heap/incremental-marking-job.cc src/heap/incremental-marking.cc src/heap/mark-compact.cc src/heap/object-stats.cc src/heap/objects-visiting.cc src/heap/scavenge-job.cc src/heap/scavenger.cc src/heap/spaces.cc src/heap/store-buffer.cc src/heap/heap.cc)

# ic
ADD_LIBRARY(ic OBJECT src/ic/access-compiler.cc src/ic/accessor-assembler.cc src/ic/call-optimization.cc src/ic/ic-compiler.cc src/ic/handler-compiler.cc src/ic/keyed-store-generic.cc src/ic/ic-state.cc src/ic/ic-stats.cc src/ic/stub-cache.cc src/ic/ic.cc)

# interpreter
ADD_LIBRARY(interpreter OBJECT src/interpreter/bytecodes.cc src/interpreter/bytecode-array-accessor.cc src/interpreter/bytecode-array-builder.cc src/interpreter/bytecode-array-iterator.cc src/interpreter/bytecode-array-random-iterator.cc src/interpreter/bytecode-array-writer.cc src/interpreter/bytecode-dead-code-optimizer.cc src/interpreter/bytecode-decoder.cc src/interpreter/bytecode-flags.cc src/interpreter/bytecode-generator.cc src/interpreter/bytecode-label.cc src/interpreter/bytecode-register.cc src/interpreter/bytecode-register-optimizer.cc src/interpreter/constant-array-builder.cc src/interpreter/control-flow-builders.cc src/interpreter/handler-table-builder.cc src/interpreter/interpreter-assembler.cc src/interpreter/interpreter-intrinsics.cc src/interpreter/interpreter.cc)

# parsing
ADD_LIBRARY(parsing OBJECT src/parsing/duplicate-finder.cc src/parsing/func-name-inferrer.cc src/parsing/parameter-initializer-rewriter.cc src/parsing/parser.cc src/parsing/parse-info.cc src/parsing/pattern-rewriter.cc src/parsing/preparse-data.cc src/parsing/preparsed-scope-data.cc src/parsing/preparser.cc src/parsing/rewriter.cc src/parsing/scanner-character-streams.cc src/parsing/scanner.cc src/parsing/token.cc src/parsing/parsing.cc)

# profiler
ADD_LIBRARY(profiler OBJECT src/profiler/allocation-tracker.cc src/profiler/cpu-profiler.cc src/profiler/heap-profiler.cc src/profiler/sampling-heap-profiler.cc src/profiler/heap-snapshot-generator.cc src/profiler/profile-generator.cc src/profiler/profiler-listener.cc src/profiler/strings-storage.cc src/profiler/tick-sample.cc src/libsampler/sampler.cc)

# regexp
ADD_LIBRARY(regexp OBJECT src/regexp/interpreter-irregexp.cc src/regexp/jsregexp.cc src/regexp/regexp-ast.cc src/regexp/regexp-macro-assembler-irregexp.cc src/regexp/regexp-macro-assembler-tracer.cc src/regexp/regexp-macro-assembler.cc src/regexp/regexp-parser.cc src/regexp/regexp-stack.cc src/regexp/regexp-utils.cc)

# runtime
ADD_LIBRARY(runtime OBJECT src/runtime/runtime-array.cc src/runtime/runtime-atomics.cc src/runtime/runtime-classes.cc src/runtime/runtime-collections.cc src/runtime/runtime-compiler.cc src/runtime/runtime-date.cc src/runtime/runtime-debug.cc src/runtime/runtime-error.cc src/runtime/runtime-forin.cc src/runtime/runtime-function.cc src/runtime/runtime-futex.cc src/runtime/runtime-generator.cc src/runtime/runtime-i18n.cc src/runtime/runtime-internal.cc src/runtime/runtime-interpreter.cc src/runtime/runtime-literals.cc src/runtime/runtime-liveedit.cc src/runtime/runtime-maths.cc src/runtime/runtime-module.cc src/runtime/runtime-numbers.cc src/runtime/runtime-object.cc src/runtime/runtime-operators.cc src/runtime/runtime-promise.cc src/runtime/runtime-proxy.cc src/runtime/runtime-regexp.cc src/runtime/runtime-scopes.cc src/runtime/runtime-simd.cc src/runtime/runtime-strings.cc src/runtime/runtime-symbol.cc src/runtime/runtime-test.cc src/runtime/runtime-typedarray.cc src/runtime/runtime-wasm.cc src/runtime/runtime.cc)

# tracing
ADD_LIBRARY(tracing OBJECT src/tracing/trace-event.cc src/tracing/traced-value.cc src/tracing/tracing-category-observer.cc)

# wasm
ADD_LIBRARY(wasm OBJECT src/wasm/function-body-decoder.cc src/wasm/module-decoder.cc src/wasm/signature-map.cc src/wasm/wasm-debug.cc src/wasm/wasm-external-refs.cc src/wasm/wasm-interpreter.cc src/wasm/wasm-js.cc src/wasm/wasm-module.cc src/wasm/wasm-module-builder.cc src/wasm/wasm-objects.cc src/wasm/wasm-opcodes.cc src/wasm/wasm-result.cc src/wasm/wasm-text.cc)

# zone
ADD_LIBRARY(zone OBJECT src/zone/accounting-allocator.cc src/zone/zone-segment.cc src/zone/zone.cc)

# js_libraries (these were generated !)
ADD_LIBRARY(js_libraries OBJECT ${CMAKE_BINARY_DIR}/libraries.cc ${CMAKE_BINARY_DIR}/experimental-libraries.cc ${CMAKE_BINARY_DIR}/extras-libraries.cc ${CMAKE_BINARY_DIR}/experimental-extras-libraries.cc src/snapshot/snapshot-empty.cc)


SET(include_dirs ${INCLUDE_DIRS} ${icui18n_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/..)
SET(link_libraries ${icui18n_LIBRARIES} -ldl -lrt)

ADD_LIBRARY(v8 SHARED $<TARGET_OBJECTS:core> $<TARGET_OBJECTS:base> $<TARGET_OBJECTS:builtins> $<TARGET_OBJECTS:platform> $<TARGET_OBJECTS:asmjs> $<TARGET_OBJECTS:ast> $<TARGET_OBJECTS:compiler> $<TARGET_OBJECTS:compilerdispatcher> $<TARGET_OBJECTS:crankshaft> $<TARGET_OBJECTS:debug> $<TARGET_OBJECTS:extensions> $<TARGET_OBJECTS:heap> $<TARGET_OBJECTS:ic> $<TARGET_OBJECTS:interpreter> $<TARGET_OBJECTS:parsing> $<TARGET_OBJECTS:profiler> $<TARGET_OBJECTS:regexp> $<TARGET_OBJECTS:runtime> $<TARGET_OBJECTS:tracing> $<TARGET_OBJECTS:wasm> $<TARGET_OBJECTS:zone> $<TARGET_OBJECTS:js_libraries> ${TOOLCHAIN_OPTS_SOURCE_FILES})
INCLUDE_DIRECTORIES(${include_dirs})
TARGET_LINK_LIBRARIES(v8 ${link_libraries})

INSTALL(TARGETS v8
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/chromium)
