################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/TreasureBox/FavoritSubBoxWnd.cpp \
../Src/TreasureBox/NetInfoBaseWnd.cpp \
../Src/TreasureBox/NumInputWnd.cpp 

OBJS += \
./Src/TreasureBox/FavoritSubBoxWnd.o \
./Src/TreasureBox/NetInfoBaseWnd.o \
./Src/TreasureBox/NumInputWnd.o 

CPP_DEPS += \
./Src/TreasureBox/FavoritSubBoxWnd.d \
./Src/TreasureBox/NetInfoBaseWnd.d \
./Src/TreasureBox/NumInputWnd.d 

# Each subdirectory must supply rules for building sources it contributes
Src/TreasureBox/%.o: ../Src/TreasureBox/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


