################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utils/linked_list.c \
../src/utils/random_gen.c \
../src/utils/string_utils.c 

OBJS += \
./src/utils/linked_list.o \
./src/utils/random_gen.o \
./src/utils/string_utils.o 

C_DEPS += \
./src/utils/linked_list.d \
./src/utils/random_gen.d \
./src/utils/string_utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


