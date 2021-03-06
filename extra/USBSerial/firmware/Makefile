PROGRAM = USBSerial

MCU = atmega644p
PARTNO = m644p

ASMDIR = asm
OBJDIR = obj
SRCDIR = src

SRCEXT = cpp
CSRCEXT = c
ASMEXT = S
OBJEXT = o
LIBEXT = a
BINEXT = elf
BOOTEXT = boot
HEXEXT = hex

BOOTLOADER = usbasp
PROGRAMMER = C232HM


SRC += ${shell find ${SRCDIR}/ -type f -name '*.${SRCEXT}'}
CSRC += ${shell find ${SRCDIR}/ -type f -name '*.${CSRCEXT}'}
ASMSRC += ${shell find ${SRCDIR}/ -type f -name '*.${ASMEXT}'}

OBJ += 	${SRC:${SRCDIR}/%.${SRCEXT}=${OBJDIR}/%.${OBJEXT}} \
		${CSRC:${SRCDIR}/%.${CSRCEXT}=${OBJDIR}/%.${OBJEXT}} \
		${ASMSRC:${SRCDIR}/%.${ASMEXT}=${OBJDIR}/%.${OBJEXT}}

ASM += 	${SRC:${SRCDIR}/%.${SRCEXT}=${ASMDIR}/%.${ASMEXT}} \
		${CSRC:${SRCDIR}/%.${CSRCEXT}=${ASMDIR}/%.${ASMEXT}}

WARNINGS += all \
			pedantic \
			extra \
			shadow \
			no-overflow \
			no-volatile
STANDARD = c++20
OPTIMISATION = s
DEFINES += 	__AVR_ATmega644P__ \
			F_CPU=12000000 \
			${CUSTOM_DEFINES}
WHOLE_LIBRARIES += multiprocess
LIBRARIES += usb++ c++ c 
#add any file for which only symbols - no code - should be included
#IE to allow bootloader code from application
SYMBOLS += 

FLAGS_ALL = -mmcu=${MCU}

INCLUDEPATH += ./ \
				${PRJDIR}/libc++/include/ \
				/usr/lib/avr/include/ \
				${PRJDIR}/libusb++/include/ \
				${PRJDIR}/libmultiprocess/include/
LIBDIR += . /usr/local/avr/lib/avr5/ \
			${PRJDIR}/libc++/ \
			${PRJDIR}/libusb++/ \
			${PRJDIR}/libmultiprocess/
INC = ${INCLUDEPATH:%=-I%}
LIB += 	${LIBDIR:%=-L%} \
		${WHOLE_LIB_FLAGS} \
		${LIBRARIES:%=-l%}
WHOLE_LIB_FLAGS =  	-Wl,--whole-archive \
					${WHOLE_LIBRARIES:%=-l%} \
					-Wl,--no-whole-archive
SYMBOLS_FLAGS = ${SYMBOLS:%=-Wl,-R%}


CXX_COMPILE_FLAGS = ${FLAGS_ALL} -c -std=${STANDARD} ${WARNINGS:%=-W%} -O${OPTIMISATION} ${INC} ${DEFINES:%=-D%} #-ggdb 
C_COMPILE_FLAGS =  ${FLAGS_ALL} -c ${WARNINGS:%=-W%} -O${OPTIMISATION} ${INC} ${DEFINES:%=-D%} #-ggdb 
ASSEMBLE_FLAGS = ${FLAGS_ALL} -c ${INC} ${DEFINES:%=-D%}
ARCHIVE_FLAGS = rcs
LINK_FLAGS = ${FLAGS_ALL} ${LIB}
BOOT_LINK_FLAGS = ${FLAGS_ALL} ${LIB} -Ttext=0xD000

COMPILE = avr-gcc-10.1.0
ASSEMBLE = avr-as
ARCHIVE = avr-ar
LINK = ${COMPILE}



all: ${PROGRAM}


debug: debug_src debug_asm debug_obj
debug_src:
	@echo "SRC:"
	@echo ${SRC}
debug_obj:
	@echo "OBJ:"
	@echo ${OBJ}


#Make object files!
obj: ${OBJ}
#Make individual object file from .cpp
${OBJDIR}/%.${OBJEXT}: ${SRCDIR}/%.${SRCEXT}
	@mkdir -p "${@D}"
	${COMPILE} ${CXX_COMPILE_FLAGS} -o $@ $<

#Make individual object file from .c
${OBJDIR}/%.${OBJEXT}: ${SRCDIR}/%.${CSRCEXT}
	@mkdir -p "${@D}"
	${COMPILE} ${C_COMPILE_FLAGS} -o $@ $<

#Make individual object file from .S
${OBJDIR}/%.${OBJEXT}: ${SRCDIR}/%.${ASMEXT}
	@mkdir -p "${@D}"
	${COMPILE} ${ASSEMBLE_FLAGS} -o $@ $<

#make assembly files
asm: ${ASM}
${ASMDIR}/%.${ASMEXT}: ${SRCDIR}/%.${SRCEXT}
	@mkdir -p "${@D}"
	${COMPILE} -S ${CXX_COMPILE_FLAGS} -o $@ $<
${ASMDIR}/%.${ASMEXT}: ${SRCDIR}/%.${CSRCEXT}
	@mkdir -p "${@D}"
	${COMPILE} -S ${C_COMPILE_FLAGS} -o $@ $<


#make executeable file
${PROGRAM}.${BINEXT}: ${OBJ}
	${LINK}  -o $@ $^ ${LINK_FLAGS}

#make bootloader executeable file
${PROGRAM}.${BOOTEXT}.${BINEXT}: ${OBJ}
	${LINK}  -o $@ $^ ${BOOT_LINK_FLAGS}

#make static library file
lib${PROGRAM}.${LIBEXT} : ${filter-out ${OBJDIR}/main.${OBJEXT}, ${OBJ}}
	${ARCHIVE} ${ARCHIVE_FLAGS} $@ $^


%.${HEXEXT} : %.${BINEXT}
	avr-objcopy -O ihex $< $@

# ${PROGRAM}.${BOOTEXT}.${HEXEXT} : ${PROGRAM}.${BOOTEXT}.${BINEXT}


${PROGRAM}: ${PROGRAM}.${BINEXT}

program: ${PROGRAM}.${BINEXT}
bootloader: ${PROGRAM}.${BOOTEXT}.${BINEXT}
lib: lib${PROGRAM}.${LIBEXT}

hex: ${PROGRAM}.${HEXEXT}
boothex: ${PROGRAM}.${BOOTEXT}.${HEXEXT}

upload: ${PROGRAM}.${HEXEXT}
	avrdude -c ${BOOTLOADER} -p ${PARTNO} -U flash:w:$<

bootload: ${PROGRAM}.${BOOTEXT}.${HEX}
	avrdude -c ${PROGRAMMER} -p ${PARTNO} -U flash:w:$<

clean:
	rm -rf ${ASMDIR}/ ${OBJDIR}/ *${PROGRAM}.*

size: ${PROGRAM}.${BINEXT}
	avr-size --mcu=${MCU} -C $<

size_boot: ${PROGRAM}.${BOOTEXT}
	avr-size --mcu=${MCU} -C $<
	
nm: ${PROGRAM}.${BINEXT}
	avr-nm -C -S --size-sort -t d $<

nm_boot: ${PROGRAM}.${BOOTEXT}
	avr-nm -C -S --size-sort -t d $<