################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/Sharing/ClientSharing.cpp \
../Src/Sharing/RepayWnd.cpp \
../Src/Sharing/SharingErr.cpp \
../Src/Sharing/SharingLogoutWnd.cpp \
../Src/Sharing/SharingIndexWnd.cpp \
../Src/Sharing/CClientSocket.cpp \
../Src/Sharing/ActivityContentWnd.cpp

OBJS += \
./Src/Sharing/ClientSharing.o \
./Src/Sharing/RepayWnd.o \
./Src/Sharing/SharingErr.o \
./Src/Sharing/SharingLogoutWnd.o \
./Src/Sharing/SharingIndexWnd.o \
./Src/Sharing/CClientSocket.o \
./Src/Sharing/ActivityContentWnd.o 

CPP_DEPS += \
./Src/Sharing/ClientSharing.d \
./Src/Sharing/RepayWnd.d \
./Src/Sharing/SharingErr.d \
./Src/Sharing/SharingLogoutWnd.d \
./Src/Sharing/SharingIndexWnd.d \
./Src/Sharing/CClientSocket.d \
./Src/Sharing/ActivityContentWnd.d

# Each subdirectory must supply rules for building sources it contributes
Src/Sharing/%.o: ../Src/Sharing/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


