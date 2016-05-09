# Normally this makefile shouldn't be called directly and we expect the output
# path to be on a certain location to fit together with the other OP-TEE
# gits and helper scripts.
ifeq ($O,)
$(error output path should be specified when calling this makefile)
endif

include $(TA_DEV_KIT_DIR)/host_include/conf.mk

# By default we expect optee_client exported folder to be on a certain relative
# path, but if the client specifies the OPTEE_CLIENT_EXPORT then that path will
# be used instead.
OPTEE_CLIENT_EXPORT ?= ../../../optee_client/out/export

CC		?= $(CROSS_COMPILE)gcc
CPP		?= $(CROSS_COMPILE)cpp
LD		?= $(CROSS_COMPILE)ld
AR		?= $(CROSS_COMPILE)ar
NM		?= $(CROSS_COMPILE)nm
OBJCOPY		?= $(CROSS_COMPILE)objcopy
OBJDUMP		?= $(CROSS_COMPILE)objdump
READELF		?= $(CROSS_COMPILE)readelf

srcs		:= smaf.c smaf_api.c
objs		:= $(patsubst %.c,$(O)/%.o, $(srcs))

CFLAGS		+= -I./
CFLAGS		+= -I$(OPTEE_CLIENT_EXPORT)/include
CFLAGS		+= -I$(TA_DEV_KIT_DIR)/host_include
CFLAGS		+= -I../../ta/smaf/include

# by default, the client application is compiled as the kernel of optee-os
ifeq ($(CFG_ARM32),y)
COMPILE_NS_USER ?= 32
else
COMPILE_NS_USER ?= 64
endif

LDFLAGS += -L$(OPTEE_CLIENT_EXPORT)/lib -lteec
LDFLAGS += -lpthread

.PHONY: all
all: smaf

smaf: $(objs)
	@echo "  LD      $(O)/$@"
	$(q)@$(CC) -o $(O)/$@ $+ $(LDFLAGS)

$(O)/%.o: $(CURDIR)/%.c
	$(q)mkdir -p $(O)
	@echo '  CC      $<'
	$(q)$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo '  CLEAN $(O)'
	$(q)rm -f $(O)/smaf
	$(q)$(foreach obj,$(objs), rm -f $(obj))
