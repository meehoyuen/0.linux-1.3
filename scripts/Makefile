HOSTCFLAGS:=$(HOSTCFLAGS) -g 
HEADER=header.tk
TAIL=tail.tk

SOUNDSCRIPT=${TOPDIR}/drivers/sound/Config.in

${SOUNDSCRIPT}: ${TOPDIR}/drivers/sound/configure.c
	make -C ${TOPDIR}/drivers/sound mkscript

kconfig.tk: ${SOUNDSCRIPT} ${TOPDIR}/Makefile ../arch/${ARCH}/config.in \
		tkparse ${HEADER} ${TAIL}
	./tkparse < ../arch/${ARCH}/config.in  > kconfig.tmp
	@if [ -f /usr/local/bin/wish ];	then \
		echo '#!'"/usr/local/bin/wish -f" > kconfig.tk; \
	else \
		echo '#!'"/usr/bin/wish -f" > kconfig.tk; \
	fi
	cat ${HEADER} >> ./kconfig.tk
	cat kconfig.tmp >> kconfig.tk
	rm -f kconfig.tmp
	echo "set defaults \"arch/${ARCH}/defconfig\"" >> kconfig.tk
	cat ${TAIL} >> kconfig.tk
	chmod 755 kconfig.tk

tkparse: tkparse.o tkcond.o tkgen.o
	${HOSTCC} -o tkparse tkparse.o tkcond.o tkgen.o

tkparse.o: tkparse.c tkparse.h
	$(HOSTCC) $(HOSTCFLAGS) -c -o tkparse.o tkparse.c

tkcond.o: tkcond.c tkparse.h
	$(HOSTCC) $(HOSTCFLAGS) -c -o tkcond.o tkcond.c

tkgen.o: tkgen.c tkparse.h
	$(HOSTCC) $(HOSTCFLAGS) -c -o tkgen.o tkgen.c

clean:
	rm -f *~ kconfig.tk *.o tkparse

include $(TOPDIR)/Rules.make
