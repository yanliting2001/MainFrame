################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/MultiMedia/ConfirmWnd.cpp \
../Src/MultiMedia/MovieControlWnd.cpp \
../Src/MultiMedia/MovieFragment.cpp \
../Src/MultiMedia/MultiMediaBottom.cpp \
../Src/MultiMedia/MultiMediaListCtrl.cpp \
../Src/MultiMedia/MultiMediaTop.cpp \
../Src/MultiMedia/MusicLibLoginWnd.cpp \
../Src/MultiMedia/MusicLibSetInfoWnd.cpp \
../Src/MultiMedia/ResultPromptWnd.cpp \
../Src/MultiMedia/SaveWnd.cpp 

OBJS += \
./Src/MultiMedia/ConfirmWnd.o \
./Src/MultiMedia/MovieControlWnd.o \
./Src/MultiMedia/MovieFragment.o \
./Src/MultiMedia/MultiMediaBottom.o \
./Src/MultiMedia/MultiMediaListCtrl.o \
./Src/MultiMedia/MultiMediaTop.o \
./Src/MultiMedia/MusicLibLoginWnd.o \
./Src/MultiMedia/MusicLibSetInfoWnd.o \
./Src/MultiMedia/ResultPromptWnd.o \
./Src/MultiMedia/SaveWnd.o 

CPP_DEPS += \
./Src/MultiMedia/ConfirmWnd.d \
./Src/MultiMedia/MovieControlWnd.d \
./Src/MultiMedia/MovieFragment.d \
./Src/MultiMedia/MultiMediaBottom.d \
./Src/MultiMedia/MultiMediaListCtrl.d \
./Src/MultiMedia/MultiMediaTop.d \
./Src/MultiMedia/MusicLibLoginWnd.d \
./Src/MultiMedia/MusicLibSetInfoWnd.d \
./Src/MultiMedia/ResultPromptWnd.d \
./Src/MultiMedia/SaveWnd.d 


# Each subdirectory must supply rules for building sources it contributes
Src/MultiMedia/%.o: ../Src/MultiMedia/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


