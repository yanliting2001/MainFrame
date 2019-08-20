################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Src/TreasureBox/subdir.mk
-include Src/SQL/subdir.mk
-include Src/Record/subdir.mk
-include Src/PlayerApi/subdir.mk
-include Src/OSDControl/subdir.mk
-include Src/InputMethodWnd/subdir.mk
-include Src/Handwrite/data/subdir.mk
-include Src/Effect/subdir.mk
-include Src/DLNA/src/subdir.mk
-include Src/ComMsg/subdir.mk
-include Src/SoundCmd/subdir.mk
-include Src/CommControl/subdir.mk
-include Src/CommApi/JsonParser/subdir.mk
-include Src/CommApi/DeleteLasterSong/subdir.mk
-include Src/CommApi/subdir.mk
-include Src/AdvanceSettingsWnd/Input/subdir.mk
-include Src/AdvanceSettingsWnd/subdir.mk
-include Src/Sharing/subdir.mk
-include Src/Settings/subdir.mk
-include Src/Login/subdir.mk
-include Src/SerialCtrl/subdir.mk
-include Src/UDisk/subdir.mk
-include Src/Upgrade/subdir.mk
-include Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs
OPTS =-march=armv7-a -mcpu=cortex-a9 -D_GNU_SOURCE -Wall -O2 -g  -ffunction-sections -fdata-sections -Wl,--gc-sections
OPTS += -DCHIP_TYPE_hi3719cv100 -DCFG_HI_SDK_VERSION="HiSTBLinuxV100R002C01SPC020"  -DHI_DAC_CVBS=3 -DHI_DAC_YPBPR_Y=1
OPTS += -DHI_DAC_YPBPR_PB=2 -DHI_DAC_YPBPR_PR=0 -DHI_SND_MUTECTL_GPIO=0x39 -DHI_SCI_CLK_MODE=0 -DHI_SCI_VCCEN_MODE=1
OPTS += -DHI_SCI_RESET_MODE=0 -DHI_SCI_VCCEN_LEVEL=0 -DHI_SCI_DETECT_LEVEL=1 -DHI_TSI_port_NUMBER=0 -DHI_TSO_PORT_NUMBER=2
OPTS += -DHI_IF0_TYPE=2 -DHI_IF0_BIT_SELECTOR=0 -DHI_TSI0_TYPE=2 -DHI_TSI0_BIT_SELECTOR=0 -DHI_TSI1_TYPE=2
OPTS += -DHI_TSI1_BIT_SELECTOR=0 -DHI_TSI2_TYPE=2 -DHI_TSI2_BIT_SELECTOR=0 -DHI_TSI3_TYPE=2 -DHI_TSI3_BIT_SELECTOR=0
OPTS += -DHI_TSI4_TYPE=2 -DHI_TSI4_BIT_SELECTOR=0 -DHI_TSI5_BIT_SELECTOR=0 -DHI_TSO0_CLK=0 -DHI_TSO0_CLK_MODE=0
OPTS += -DHI_TSO0_VALID_MODE=0 -DHI_TSO0_SERIAL=1 -DHI_TSO0_BIT_SELECTOR=0 -DHI_TSO1_CLK=0 -DHI_TSO1_CLK_MODE=0
OPTS += -DHI_TSO1_VALID_MODE=0 -DHI_TSO1_SERIAL=1 -DHI_TSO1_BIT_SELECTOR=0 -DHI_TUNER_NUMBER=1 -DHI_TUNER0_ID=0
OPTS += -DHI_TUNER_SIGNAL_TYPE=0 -DHI_TUNER_TYPE=17 -DHI_TUNER_DEV_ADDR=0xc0 -DHI_DEMOD_TYPE=256 -DHI_DEMOD_DEV_ADDR=0xa0
OPTS += -DHI_DEMOD_TS_MODE=1 -DHI_DEMOD_I2C_CHANNEL=5 -DHI_DEMOD_RESET_GPIO=0x1 -DHI_DEMUX_PORT=0 -DHI_DEMOD_REF_CLOCK=24000
OPTS += -DHI_TUNER_MAX_LPF=34 -DHI_TUNER_I2C_CLOCK=400 -DHI_LNBCTRL_DEV_ADDR=0x0 -DHI_TUNER1_ID=1 -DHI_TUNER1_SIGNAL_TYPE=0
OPTS += -DHI_TUNER1_TYPE=4 -DHI_TUNER1_DEV_ADDR=0xc0 -DHI_DEMOD1_TYPE=256 -DHI_DEMOD1_DEV_ADDR=0xa0 -DHI_DEMOD1_TS_MODE=1
OPTS += -DHI_DEMOD1_I2C_CHANNEL=3 -DHI_DEMOD1_RESET_GPIO=0x1 -DHI_DEMUX1_PORT=5 -DHI_DEMOD1_REF_CLOCK=24000
OPTS += -DHI_TUNER1_MAX_LPF=34 -DHI_TUNER1_I2C_CLOCK=400 -DHI_LNBCTRL1_DEV_ADDR=0x0 -DHI_TUNER2_ID=2 -DHI_TUNER2_SIGNAL_TYPE=0
OPTS += -DHI_TUNER2_TYPE=4 -DHI_TUNER2_DEV_ADDR=0xc0 -DHI_DEMOD2_TYPE=256 -DHI_DEMOD2_DEV_ADDR=0xa0 -DHI_DEMOD2_TS_MODE=1
OPTS += -DHI_DEMOD2_I2C_CHANNEL=5 -DHI_DEMOD2_RESET_GPIO=0x0 -DHI_DEMUX2_PORT=0 -DHI_DEMOD2_REF_CLOCK=24000
OPTS += -DHI_TUNER2_MAX_LPF=34 -DHI_TUNER2_I2C_CLOCK=400 -DHI_LNBCTRL2_DEV_ADDR=0x0 -DHI_TUNER3_ID=3
OPTS += -DHI_TUNER3_SIGNAL_TYPE=0 -DHI_TUNER3_TYPE=4 -DHI_TUNER3_DEV_ADDR=0xc0 -DHI_DEMOD3_TYPE=256
OPTS += -DHI_DEMOD3_DEV_ADDR=0xa0 -DHI_DEMOD3_TS_MODE=1 -DHI_DEMOD3_I2C_CHANNEL=5 -DHI_DEMOD3_RESET_GPIO=0x0
OPTS += -DHI_DEMUX3_PORT=0 -DHI_DEMOD3_REF_CLOCK=24000 -DHI_TUNER3_MAX_LPF=34 -DHI_TUNER3_I2C_CLOCK=400
OPTS += -DHI_LNBCTRL3_DEV_ADDR=0x0
OPTS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
CFLAGS+=$(OPTS)
CFLAGS+= -DDUMP_AUDIO_PCM  
#CFLAGS+= -DSOUND_XFYUN
CFLAGS+= -I"../Src/Record/" -I"../Src/ComMsg" -I"../Src/SoundCmd"  -I"../Src/CommDef" -I"../Src/CommApi" -I"../Src/SQL" -I"../../CommonLib/Include" -I"../../ImgFileOp/Include" -I"../../Enjoy3DEngine/Src" -I"../../XMLParser/Include" -I"../../MicroFramework/Include" -I"../../CloudSongs/include" -I"../../ShareApi/include" -I"../../WifiCtrl/include" -I"../../DLNA/include" -I"../../mw/include" -I"../../BCM97231Api/src" -I"../../libs/hisi/include" -I"../../libs/curl/include" -I"../../libs/score/include"  -I"../../libs/dlna/include" -I"../../libs/xfyun/include" -I"../../libs/arec/include"  -I"../../libs/sqlite3/include" -I"../../libs/tslib/include" -I"../../libs/al/include" -I"../../libs/hisi/include/freetype2" 
# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ktvapp

# Tool invocations
ktvapp: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic   -L"/opt/hisi-linux/x86-arm/arm-hisiv200-linux/target/usr/lib" -L"../../libs/decrypt/lib" -L"../../libs/arec/lib"  -L"../../libs/xfyun/lib" -L"../../CommonLib/hisi3719Debug" -L"../../ImgFileOp/hisi3719Debug" -L"../../XMLParser/haisi3791cDebug" -L"../../Enjoy3DEngine/hisi3719Debug" -L"../../MicroFramework/hisi3719Debug" -L"../../CloudSongs/hisi3719Debug" -L"../../ShareApi/hisi3719Debug" -L"../../BCM97231Api/hisi3719Debug" -L"../../libs/hisi/lib/share" -L"../../libs/hisi/lib/extern" -L"../../libs/hisi/lib/share/alsa_bluez" -L"../../libs/al" -L"../../libs/hw" -L"../../libs/curl/lib" -L"../../libs/tslib/lib" -L"../../libs/sqlite3/lib" -L"../../WifiCtrl/haisi3791cDebug" -L"../../mw/haisi3791cDebug" -L"../../libs/score/lib" -L"../../libs/dlna/lib"  -L"../../libs/" -o ktvapp $(OBJS) $(USER_OBJS) -Wl,--start-group  $(LIBS) -Wl,--end-group	-Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) ktvapp
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
