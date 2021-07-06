TARGET = main

NUCLEI_SDK_ROOT = ./cores/arduino/nuclei_sdk

SRCDIRS := .\
		   .\cores\arduino\
		   .\cores\arduino\avr\
		   .\cores\arduino\gd32v\
		   .\variants\RVSTAR\
		   .\libraries\SPI\
		   .\libraries\Wire\
		   .\libraries\Wire\utility\

INCDIRS := .\
		   .\cores\arduino\
		   .\cores\arduino\avr\
		   .\cores\arduino\gd32v\
		   .\variants\RVSTAR\
		   .\libraries\SPI\
		   .\libraries\Wire\
		   .\libraries\Wire\utility\


include $(NUCLEI_SDK_ROOT)/Build/Makefile.base