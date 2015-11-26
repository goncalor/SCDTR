
# this is a makefile
# run with 'make -f filename'

# the baud rate the programmer is using
BAUD=19200

AVRDUDE=avrdude
AVRDUDE_PROGRAMMERID=usbtiny
MCU=m328p	# ATmega328P
AVRDUDE_PORT=/dev/ttyACM0

############### READ ##################
# read fuses. fuse values are written to specified files
read:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
		-p $(MCU)                           \
		-U lfuse:r:low_fuse_val.hex:h -U hfuse:r:high_fuse_val.hex:h

############### WRITE #################
# high and low fuse values. be careful!
FUSE_H=
FUSE_L=

# change fuse values of the device
write:
	@[ "$(FUSE_H)" != "" -a "$(FUSE_L)" != "" ] || \
		{ echo "*** Please edit Makefile and choose values for FUSE_L and FUSE_H!"; exit 1; }
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
		-p $(MCU)                           \
		-U lfuse:w:$(FUSE_L):m -U hfuse:w:$(FUSE_H):m


#$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
	#	-p $(MCU)                \
	#	-P $(AVRDUDE_PORT)                  \
	#	-b $(BAUD)                          \
	#	-U lfuse:w:$(FUSE_L):m -U hfuse:w:$(FUSE_H):m

