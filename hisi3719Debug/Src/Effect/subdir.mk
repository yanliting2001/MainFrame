################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/Effect/AnimationWnd.cpp \
../Src/Effect/FlyWnd.cpp \
../Src/Effect/PopupSlideWnd.cpp \
../Src/Effect/SlideWnd.cpp 

OBJS += \
./Src/Effect/AnimationWnd.o \
./Src/Effect/FlyWnd.o \
./Src/Effect/PopupSlideWnd.o \
./Src/Effect/SlideWnd.o 

CPP_DEPS += \
./Src/Effect/AnimationWnd.d \
./Src/Effect/FlyWnd.d \
./Src/Effect/PopupSlideWnd.d \
./Src/Effect/SlideWnd.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Effect/%.o: ../Src/Effect/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


