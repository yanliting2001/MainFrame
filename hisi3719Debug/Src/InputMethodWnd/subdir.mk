################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/InputMethodWnd/HandWriteInputWnd.cpp \
../Src/InputMethodWnd/InputMethodPanel.cpp \
../Src/InputMethodWnd/SearchInputWnd.cpp \
../Src/InputMethodWnd/SpellInputWnd.cpp \
../Src/InputMethodWnd/DigitInputWnd.cpp \
../Src/InputMethodWnd/WordLengthInputWnd.cpp 

OBJS += \
./Src/InputMethodWnd/HandWriteInputWnd.o \
./Src/InputMethodWnd/InputMethodPanel.o \
./Src/InputMethodWnd/SearchInputWnd.o \
./Src/InputMethodWnd/SpellInputWnd.o \
./Src/InputMethodWnd/DigitInputWnd.o \
./Src/InputMethodWnd/WordLengthInputWnd.o 

CPP_DEPS += \
./Src/InputMethodWnd/HandWriteInputWnd.d \
./Src/InputMethodWnd/InputMethodPanel.d \
./Src/InputMethodWnd/SearchInputWnd.d \
./Src/InputMethodWnd/SpellInputWnd.d \
./Src/InputMethodWnd/DigitInputWnd.d \
./Src/InputMethodWnd/WordLengthInputWnd.d 


# Each subdirectory must supply rules for building sources it contributes
Src/InputMethodWnd/%.o: ../Src/InputMethodWnd/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


