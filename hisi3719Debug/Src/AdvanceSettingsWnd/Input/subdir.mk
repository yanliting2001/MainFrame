################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/AdvanceSettingsWnd/Input/BKHandWriteInputWnd.cpp \
../Src/AdvanceSettingsWnd/Input/BKInputPanel.cpp \
../Src/AdvanceSettingsWnd/Input/BKSpellInputWnd.cpp 

OBJS += \
./Src/AdvanceSettingsWnd/Input/BKHandWriteInputWnd.o \
./Src/AdvanceSettingsWnd/Input/BKInputPanel.o \
./Src/AdvanceSettingsWnd/Input/BKSpellInputWnd.o 

CPP_DEPS += \
./Src/AdvanceSettingsWnd/Input/BKHandWriteInputWnd.d \
./Src/AdvanceSettingsWnd/Input/BKInputPanel.d \
./Src/AdvanceSettingsWnd/Input/BKSpellInputWnd.d 


# Each subdirectory must supply rules for building sources it contributes
Src/AdvanceSettingsWnd/Input/%.o: ../Src/AdvanceSettingsWnd/Input/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


