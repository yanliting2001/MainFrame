################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/AdvanceSettingsWnd/AddSongWnd.cpp \
../Src/AdvanceSettingsWnd/AdvanceSettingApi.cpp \
../Src/AdvanceSettingsWnd/UDiskSongInfoWnd.cpp \
../Src/AdvanceSettingsWnd/CalibrateWnd.cpp \
../Src/AdvanceSettingsWnd/MusicLibItemWnd.cpp \
../Src/AdvanceSettingsWnd/MusicLibWnd.cpp \
../Src/AdvanceSettingsWnd/SelectMusicItem.cpp \
../Src/AdvanceSettingsWnd/SetMusicLibWnd.cpp \
../Src/AdvanceSettingsWnd/SetScrollTextWnd.cpp \
../Src/AdvanceSettingsWnd/UDiskSongInfoWnd.cpp 

OBJS += \
./Src/AdvanceSettingsWnd/AddSongWnd.o \
./Src/AdvanceSettingsWnd/AdvanceSettingApi.o \
./Src/AdvanceSettingsWnd/CalibrateWnd.o \
./Src/AdvanceSettingsWnd/MusicLibItemWnd.o \
./Src/AdvanceSettingsWnd/MusicLibWnd.o \
./Src/AdvanceSettingsWnd/SelectMusicItem.o \
./Src/AdvanceSettingsWnd/SetMusicLibWnd.o \
./Src/AdvanceSettingsWnd/SetScrollTextWnd.o \
./Src/AdvanceSettingsWnd/UDiskSongInfoWnd.o 

CPP_DEPS += \
./Src/AdvanceSettingsWnd/AddSongWnd.d \
./Src/AdvanceSettingsWnd/AdvanceSettingApi.d \
./Src/AdvanceSettingsWnd/MusicLibWnd.d \
./Src/AdvanceSettingsWnd/CalibrateWnd.d \
./Src/AdvanceSettingsWnd/SelectMusicItem.d \
./Src/AdvanceSettingsWnd/SetMusicLibWnd.d \
./Src/AdvanceSettingsWnd/SetScrollTextWnd.d \
./Src/AdvanceSettingsWnd/UDiskSongInfoWnd.d 


# Each subdirectory must supply rules for building sources it contributes
Src/AdvanceSettingsWnd/%.o: ../Src/AdvanceSettingsWnd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


