################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Handwrite/data/hwdata-b5-R.c \
../Src/Handwrite/data/hwdata-gb-R.c 

OBJS += \
./Src/Handwrite/data/hwdata-b5-R.o \
./Src/Handwrite/data/hwdata-gb-R.o 

C_DEPS += \
./Src/Handwrite/data/hwdata-b5-R.d \
./Src/Handwrite/data/hwdata-gb-R.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Handwrite/data/%.o: ../Src/Handwrite/data/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-hisiv200-linux-gnueabi-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


