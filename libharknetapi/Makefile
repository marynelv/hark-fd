#
# Network API
#
#

CC          = gcc

PREFIX		= /usr
INS_LIB		= $(DESTDIR)/$(PREFIX)/lib
INS_HDR		= $(DESTDIR)/$(PREFIX)/include

CFLAGS      = $(WARN_FLAGS) $(DEBUG_FLAGS) $(C_INCLUDES)

C_INCLUDES  =

#WARN_FLAGS  = 
WARN_FLAGS  = -Wall

DEBUG_FLAGS = $(DEBUG_OPT)
#DEBUG_FLAGS = -DDEBUG

INSTALL = install

SRC_NAME		= hark-netapi.c
OBJ_NAME	    = ${SRC_NAME:.c=.o}
LIB_NAME        = libhark-netapi

SONAME          = $(LIB_NAME)
MAJOR_VER       = 2
MINOR_VER       = 1



all:shared static

shared:
	@echo ""
	@echo "<< Compile RASP NetAPI Shared Library >>"
	@echo ""
	$(CC) $(CFLAGS) -fPIC -c $(SRC_NAME)
	@echo ""
	$(CC) -shared -Wl,-soname,$(SONAME).so.$(MAJOR_VER) -o $(SONAME).so.$(MAJOR_VER).$(MINOR_VER) $(OBJ_NAME)
	@echo ""
	ln -sf $(SONAME).so.$(MAJOR_VER).$(MINOR_VER) $(SONAME).so.$(MAJOR_VER)
	ln -sf $(SONAME).so.$(MAJOR_VER).$(MINOR_VER) $(SONAME).so
	@echo ""

static:
	@echo ""
	@echo "<< Compile RASP NetAPI Static Library >>"
	@echo ""
	$(CC) $(CFLAGS) -fPIC -c $(SRC_NAME) -o $(OBJ_NAME)
	@echo ""
	ar rv $(LIB_NAME).a $(OBJ_NAME)
	@echo ""

install:
	@echo "Install [ RASP NetAPI Library ]"
	@echo ""
#	mkdir -p $(INS_LIB)
#	mkdir -p $(INS_HDR)
	$(INSTALL) -d $(INS_LIB)
	$(INSTALL) -d $(INS_HDR)

	install -p -v -m 0644 hark-netapi.h $(INS_HDR)
	install -p -v -m 0755 $(SONAME).so.$(MAJOR_VER).$(MINOR_VER)  $(INS_LIB)
	install -p -v -m 0664 $(LIB_NAME).a  $(INS_LIB)
#	ldconfig $(INS_LIB)
	cd $(INS_LIB) ; \
	ln -sf $(SONAME).so.$(MAJOR_VER).$(MINOR_VER) $(SONAME).so 
	cd $(INS_LIB) ; \
	ln -sf $(SONAME).so.$(MAJOR_VER).$(MINOR_VER) $(SONAME).so.$(MAJOR_VER)






uninstall:
	rm -v $(INS_HDR)/hark-netapi.h
	rm -v $(INS_LIB)/$(SONAME).so*
	rm -v $(INS_LIB)/$(LIB_NAME).a


#	cp -d $(SONAME).so.$(MAJOR_VER)  $(INS_LIB)
#	cp -d $(SONAME).so  $(INS_LIB)

clean:
	rm -fv *.o
	rm -fv *.a
	rm -fr *.so*
	rm -fv *~


# EOF #
