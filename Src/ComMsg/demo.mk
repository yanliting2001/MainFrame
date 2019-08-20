################################################################################
# Automatically-generated file. Do not edit!
################################################################################


RM := rm -rf

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
CFLAGS+= -I"../Src/Record/"  -I"../Src/CommDef" -I"../Src/CommApi" -I"../Src/SQL" -I"../../CommonLib/Include" -I"../../ImgFileOp/Include" -I"../../Enjoy3DEngine/Src" -I"../../XMLParser/Include" -I"../../MicroFramework/Include" -I"../../CloudSongs/include" -I"../../ShareApi/include" -I"../../WifiCtrl/include" -I"../../DLNA/include" -I"../../mw/include" -I"../../BCM97231Api/src" -I"../../libs/hisi/include" -I"../../libs/curl/include" -I"../../libs/score/include"  -I"../../libs/dlna/include" -I"../../libs/sqlite3/include" -I"../../libs/tslib/include" -I"../../libs/al/include" -I"../../libs/hisi/include/freetype2"
# Add inputs and outputs from these tool invocations to the build variables 

OBJS=SerDrive.o  main.o

CPP_SRCS += SerDrive.cpp main.cpp


CPP_DEPS +=SerDrive.d main.d

%.o: %.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '

# All Target
all: comdemo

# Tool invocations
comdemo: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic -L"/opt/hisi-linux/x86-arm/arm-hisiv200-linux/target/usr/lib" -L"../../libs/decrypt/lib" -L"../../CommonLib/hisi3719Debug" -L"../../ImgFileOp/hisi3719Debug" -L"../../XMLParser/haisi3791cDebug" -L"../../Enjoy3DEngine/hisi3719Debug" -L"../../MicroFramework/hisi3719Debug" -L"../../CloudSongs/hisi3719Debug" -L"../../ShareApi/hisi3719Debug" -L"../../BCM97231Api/hisi3719Debug" -L"../../libs/hisi/lib/share" -L"../../libs/hisi/lib/extern" -L"../../libs/hisi/lib/share/alsa_bluez" -L"../../libs/al" -L"../../libs/hw" -L"../../libs/curl/lib" -L"../../libs/tslib/lib" -L"../../libs/sqlite3/lib" -L"../../WifiCtrl/haisi3791cDebug" -L"../../mw/haisi3791cDebug" -L"../../libs/score/lib" -L"../../libs/dlna/lib" -o comdemo $(OBJS) -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CC_DEPS) $(C++_DEPS) $(EXECUTABLES) $(OBJS) $(C_UPPER_DEPS) $(CXX_DEPS) $(C_DEPS) $(CPP_DEPS) comdemo
	-@echo ' '

.PHONY: all clean
.SECONDARY:

