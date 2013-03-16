
LOCAL_PATH:= $(call my-dir)

#
# The Marsyas lib, which will be built statically
#
include $(CLEAR_VARS)
LOCAL_MODULE    := libmarsyas
LOCAL_C_INCLUDES := ~/src/marsyas/src/marsyas
LOCAL_CFLAGS    := -fexceptions -Ijni/marsyas/ -Ijni/marsyas/core/ -Ijni/marsyas/marsystems/
#LOCAL_SRC_FILES := marsyas/RealvecSource.cpp marsyas/Series.cpp marsyas/MarControlValue.cpp marsyas/core/realvec.cpp marsyas/MarControl.cpp marsyas/marsystems/Gain.cpp marsyas/MarSystem.cpp 
LOCAL_SRC_FILES := marsyas/RealvecSource.cpp marsyas/Series.cpp marsyas/SineSource.cpp marsyas/vmblock.cpp marsyas/lu.cpp marsyas/ExVal.cpp marsyas/ExScanner.cpp marsyas/ExParser.cpp marsyas/ExSymTbl.cpp marsyas/ExNode.cpp marsyas/ExCommon.cpp marsyas/TmRealTime.cpp marsyas/Expr.cpp marsyas/EvExpr.cpp marsyas/MarControlValue.cpp marsyas/TmTime.cpp marsyas/TmTimerManager.cpp marsyas/TmParam.cpp marsyas/TmVirtualTime.cpp marsyas/Repeat.cpp marsyas/EvValUpd.cpp marsyas/core/realvec.cpp marsyas/Scheduler.cpp marsyas/EvEvent.cpp marsyas/Conversions.cpp marsyas/TmControlValue.cpp marsyas/NumericLib.cpp marsyas/TmTimer.cpp marsyas/MrsLog.cpp marsyas/MarControl.cpp marsyas/marsystems/Gain.cpp marsyas/MarControlManager.cpp marsyas/MarSystem.cpp 
include $(BUILD_STATIC_LIBRARY)

#
# The second, small driver to call Marsyas
#
include $(CLEAR_VARS)
LOCAL_MODULE    := hellomarsyas
#LOCAL_C_INCLUDES := ~/src/marsyas/src/marsyas
#LOCAL_CFLAGS    := -Werror -Ijni/marsyas/ -Ijni/marsyas/core/ -Iaaaaaaaaaaa/
LOCAL_CFLAGS    := -fexceptions -Ijni/marsyas/ -Ijni/marsyas/core/ -Ijni/marsyas/marsystems/
#LOCAL_CFLAGS    := -Werror
LOCAL_SRC_FILES := hellomarsyas.cpp
LOCAL_LDLIBS    := -llog
LOCAL_STATIC_LIBRARIES := libmarsyas
include $(BUILD_SHARED_LIBRARY)

