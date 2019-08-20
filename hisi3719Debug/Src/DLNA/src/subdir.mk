################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/DLNA/src/DlnaApi.cpp \
../Src/DLNA/src/DlnaMdrApi.cpp 
#../Src/DLNA/src/PadControl.cpp \
#../Src/DLNA/src/control.cpp \
#../Src/DLNA/src/xmlanalytic.cpp 

OBJS += \
./Src/DLNA/src/DlnaApi.o \
./Src/DLNA/src/DlnaMdrApi.o 
#./Src/DLNA/src/PadControl.o \
#./Src/DLNA/src/control.o \
#./Src/DLNA/src/xmlanalytic.o 

CPP_DEPS += \
./Src/DLNA/src/DlnaApi.d \
./Src/DLNA/src/DlnaMdrApi.d 
#./Src/DLNA/src/PadControl.d \
#./Src/DLNA/src/control.d \
#./Src/DLNA/src/xmlanalytic.d 


# Each subdirectory must supply rules for building sources it contributes
Src/DLNA/src/%.o: ../Src/DLNA/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


