################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/Settings/SettingsFragment.cpp \
../Src/Settings/SettingInputWnd.cpp \
../Src/Settings/SettingPlayWnd.cpp \
../Src/Settings/SettingMusicLibWnd.cpp \
../Src/Settings/SettingSystemWnd.cpp\
../Src/Settings/SettingPublicWnd.cpp

OBJS += \
./Src/Settings/SettingsFragment.o \
./Src/Settings/SettingInputWnd.o \
./Src/Settings/SettingPlayWnd.o \
./Src/Settings/SettingMusicLibWnd.o \
./Src/Settings/SettingSystemWnd.o \
./Src/Settings/SettingPublicWnd.o
CPP_DEPS += \
./Src/Settings/SettingsFragment.p \
./Src/Settings/SettingInputWnd.p \
./Src/Settings/SettingPlayWnd.p \
./Src/Settings/SettingMusicLibWnd.p \
./Src/Settings/SettingSystemWnd.p\
./Src/Settings/SettingPublicWnd.p
# Each subdirectory must supply rules for building sources it contributes
Src/Settings/%.o: ../Src/Settings/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


