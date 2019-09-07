################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Src/CommControl/AlbumSongFragment.cpp \
../Src/CommControl/AmbienceWnd.cpp \
../Src/CommControl/AttunementWnd.cpp \
../Src/CommControl/BaseListCtrl.cpp \
../Src/CommControl/BottomPierlPanel.cpp \
../Src/CommControl/CloudDownLoadPromptWnd.cpp \
../Src/CommControl/CloudFragment.cpp \
../Src/CommControl/HomePageFragment.cpp \
../Src/CommControl/InputWifiPasswdWnd.cpp \
../Src/CommControl/LangSongFragment.cpp \
../Src/CommControl/LeftPierPanel.cpp \
../Src/CommControl/MainCtrlPage.cpp \
../Src/CommControl/NetInfoWnd.cpp \
../Src/CommControl/PageManager.cpp \
../Src/CommControl/PhantomPage.cpp \
../Src/CommControl/SingerListCtrl.cpp \
../Src/CommControl/SingerListFragment.cpp \
../Src/CommControl/SingerTypeFragment.cpp \
../Src/CommControl/SongListCtrl.cpp \
../Src/CommControl/SongListFragment.cpp \
../Src/CommControl/SelectedFragment.cpp \
../Src/CommControl/TopPierPanel.cpp \
../Src/CommControl/ServerWnd.cpp \
../Src/CommControl/SwitchPromptWnd.cpp \
../Src/CommControl/DownloadListFragment.cpp \
../Src/CommControl/WiFiWnd.cpp \
../Src/CommControl/RecordWnd.cpp \
../Src/CommControl/DLNAWnd.cpp \
../Src/CommControl/DlnaList.cpp \
../Src/CommControl/YiLang.cpp \
../Src/CommControl/PromptMsg.cpp \
../Src/CommControl/ShopWnd.cpp \
../Src/CommControl/FoodWnd.cpp \
../Src/CommControl/SearchTypeBar.cpp \
../Src/CommControl/WaitingWnd.cpp

OBJS += \
./Src/CommControl/AlbumSongFragment.o \
./Src/CommControl/AmbienceWnd.o \
./Src/CommControl/AttunementWnd.o \
./Src/CommControl/BaseListCtrl.o \
./Src/CommControl/BottomPierlPanel.o \
./Src/CommControl/CloudDownLoadPromptWnd.o \
./Src/CommControl/CloudFragment.o \
./Src/CommControl/HomePageFragment.o \
./Src/CommControl/InputWifiPasswdWnd.o \
./Src/CommControl/LangSongFragment.o \
./Src/CommControl/LeftPierPanel.o \
./Src/CommControl/MainCtrlPage.o \
./Src/CommControl/NetInfoWnd.o \
./Src/CommControl/PageManager.o \
./Src/CommControl/PhantomPage.o \
./Src/CommControl/SingerListCtrl.o \
./Src/CommControl/SingerListFragment.o \
./Src/CommControl/SingerTypeFragment.o \
./Src/CommControl/SongListCtrl.o \
./Src/CommControl/SongListFragment.o \
./Src/CommControl/SelectedFragment.o \
./Src/CommControl/TopPierPanel.o \
./Src/CommControl/ServerWnd.o \
./Src/CommControl/SwitchPromptWnd.o \
./Src/CommControl/DownloadListFragment.o \
./Src/CommControl/WiFiWnd.o \
./Src/CommControl/RecordWnd.o \
./Src/CommControl/DLNAWnd.o \
./Src/CommControl/DlnaList.o \
./Src/CommControl/YiLang.o \
./Src/CommControl/PromptMsg.o \
./Src/CommControl/ShopWnd.o \
./Src/CommControl/FoodWnd.o \
./Src/CommControl/SearchTypeBar.o \
./Src/CommControl/WaitingWnd.o

CPP_DEPS += \
./Src/CommControl/AlbumSongFragment.d \
./Src/CommControl/AmbienceWnd.d \
./Src/CommControl/AttunementWnd.d \
./Src/CommControl/BaseListCtrl.d \
./Src/CommControl/BottomPierlPanel.d \
./Src/CommControl/CloudDownLoadPromptWnd.d \
./Src/CommControl/CloudFragment.d \
./Src/CommControl/HomePageFragment.d \
./Src/CommControl/InputWifiPasswdWnd.d \
./Src/CommControl/LangSongFragment.d \
./Src/CommControl/LeftPierPanel.d \
./Src/CommControl/MainCtrlPage.d \
./Src/CommControl/NetInfoWnd.d \
./Src/CommControl/PageManager.d \
./Src/CommControl/PhantomPage.d \
./Src/CommControl/SingerListCtrl.d \
./Src/CommControl/SingerListFragment.d \
./Src/CommControl/SingerTypeFragment.d \
./Src/CommControl/SongListCtrl.d \
./Src/CommControl/SongListFragment.d \
./Src/CommControl/SelectedFragment.d \
./Src/CommControl/TopPierPanel.d \
./Src/CommControl/ServerWnd.d \
./Src/CommControl/SwitchPromptWnd.d \
./Src/CommControl/DownloadListFragment.d \
./Src/CommControl/WiFiWnd.d \
./Src/CommControl/RecordWnd.d \
./Src/CommControl/DLNAWnd.d \
./Src/CommControl/DlnaList.d \
./Src/CommControl/YiLang.d \
./Src/CommControl/PromptMsg.d \
./Src/CommControl/ShopWnd.d \
./Src/CommControl/FoodWnd.d \
./Src/CommControl/SearchTypeBar.d \
./Src/CommControl/WaitingWnd.d


# Each subdirectory must supply rules for building sources it contributes
Src/CommControl/%.o: ../Src/CommControl/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-hisiv200-linux-gnueabi-g++ -rdynamic $(CFLAGS) -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<" -Darm -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
	@echo 'Finished building: $<'
	@echo ' '


