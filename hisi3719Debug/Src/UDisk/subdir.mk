################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/UDisk/AddUDiskFileWnd.cpp \
../Src/UDisk/UDiskFileInfo.cpp \
../Src/UDisk/UDiskFragment.cpp

OBJS += \
./Src/UDisk/AddUDiskFileWnd.o \
./Src/UDisk/UDiskFileInfo.o \
./Src/UDisk/UDiskFragment.o
CPP_DEPS += \
./Src/UDisk/AddUDiskFileWnd.p \
./Src/UDisk/UDiskFileInfo.p \
./Src/UDisk/UDiskFragment.p
# Each subdirectory must supply rules for building sources it contributes
Src/UDisk/%.o: ../Src/UDisk/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


