################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/PlayerApi/PlayerApi.cpp \
../Src/PlayerApi/PlayerInfo.cpp \
../Src/PlayerApi/PlayerServer.cpp 

OBJS += \
./Src/PlayerApi/PlayerApi.o \
./Src/PlayerApi/PlayerInfo.o \
./Src/PlayerApi/PlayerServer.o 

CPP_DEPS += \
./Src/PlayerApi/PlayerApi.d \
./Src/PlayerApi/PlayerInfo.d \
./Src/PlayerApi/PlayerServer.d 


# Each subdirectory must supply rules for building sources it contributes
Src/PlayerApi/%.o: ../Src/PlayerApi/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


