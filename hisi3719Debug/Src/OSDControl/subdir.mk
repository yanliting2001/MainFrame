################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/OSDControl/BCM_Control.cpp \
../Src/OSDControl/BCM_OSD_CTRL.cpp \
../Src/OSDControl/BCM_PLT_CTRL.cpp \
../Src/OSDControl/AudioCompare_OSD.cpp \
../Src/OSDControl/FreeTypeApi.cpp 

OBJS += \
./Src/OSDControl/BCM_Control.o \
./Src/OSDControl/BCM_OSD_CTRL.o \
./Src/OSDControl/BCM_PLT_CTRL.o \
./Src/OSDControl/AudioCompare_OSD.o \
./Src/OSDControl/FreeTypeApi.o 

CPP_DEPS += \
./Src/OSDControl/BCM_Control.d \
./Src/OSDControl/BCM_OSD_CTRL.d \
./Src/OSDControl/BCM_PLT_CTRL.d \
./Src/OSDControl/AudioCompare_OSD.d \
./Src/OSDControl/FreeTypeApi.d 


# Each subdirectory must supply rules for building sources it contributes
Src/OSDControl/%.o: ../Src/OSDControl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


