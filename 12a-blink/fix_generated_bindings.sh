#!/bin/sh

# add the required definition for "RuntimeEnabledFeatures"

sed -i '11i #include "platform/RuntimeEnabledFeatures.h"' bindings/core/v8/V8EventListenerOptions.cpp
sed -i '11i #include "platform/RuntimeEnabledFeatures.h"' bindings/core/v8/V8ShadowRootInit.cpp
sed -i '16i #include "platform/RuntimeEnabledFeatures.h"' bindings/modules/v8/V8MediaTrackConstraintSet.cpp

# remove all references to "USBConnectionEvent", because it does not compile

sed -i '/USB/d' modules/EventModules.cpp
