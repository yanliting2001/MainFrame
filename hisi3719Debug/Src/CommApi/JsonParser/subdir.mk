################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/CommApi/JsonParser/cJSON.c 

CPP_SRCS += \
../Src/CommApi/JsonParser/cJsonParser.cpp 

OBJS += \
./Src/CommApi/JsonParser/cJSON.o \
./Src/CommApi/JsonParser/cJsonParser.o 

C_DEPS += \
./Src/CommApi/JsonParser/cJSON.d 

CPP_DEPS += \
./Src/CommApi/JsonParser/cJsonParser.d 


# Each subdirectory must supply rules for building sources it contributes
Src/CommApi/JsonParser/%.o: ../Src/CommApi/JsonParser/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-hisiv200-linux-gnueabi-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Src/CommApi/JsonParser/%.o: ../Src/CommApi/JsonParser/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


