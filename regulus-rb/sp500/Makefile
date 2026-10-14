TOOLCHAIN_BASE := /opt/crosstools/mobilint/2.0.0
SHELL := /bin/bash
TOOLCHAIN_ROOT ?=
TOOLCHAIN_GOALS := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),all)
TARGETS := sp500_letterbox
CLEAN_FILES := $(TARGETS) outputs/letterbox_out.png

ifeq ($(TOOLCHAIN_ENV_ACTIVE),)
ifeq ($(MAKECMDGOALS),clean)
.PHONY: clean

clean:
	$(RM) $(CLEAN_FILES)
else
.DEFAULT_GOAL := toolchain-build

.PHONY: toolchain-build $(MAKECMDGOALS)

toolchain-build:
	@set -e; \
		toolchain_root="$(TOOLCHAIN_ROOT)"; \
		if [[ -z "$$toolchain_root" ]]; then toolchain_root=$$(find "$(TOOLCHAIN_BASE)" -mindepth 2 -maxdepth 2 -type f -path '$(TOOLCHAIN_BASE)/v*/environment-setup-cortexa53-mobilint-linux' -printf '%h\n' 2>/dev/null | sort -V | tail -n 1); fi; \
		if [[ -z "$$toolchain_root" ]]; then echo "Error: no valid toolchain version was found under $(TOOLCHAIN_BASE)." >&2; exit 2; fi; \
		toolchain_env="$$toolchain_root/environment-setup-cortexa53-mobilint-linux"; \
		if [[ ! -f "$$toolchain_env" ]]; then echo "Error: toolchain environment file was not found: $$toolchain_env" >&2; exit 2; fi; \
		unset LD_LIBRARY_PATH; \
		source "$$toolchain_env"; \
		$(MAKE) --no-print-directory TOOLCHAIN_ENV_ACTIVE=1 TOOLCHAIN_ROOT="$$toolchain_root" $(TOOLCHAIN_GOALS)

ifneq ($(strip $(MAKECMDGOALS)),)
$(MAKECMDGOALS): toolchain-build
	@:
endif
endif
else

CXXFLAGS += -std=c++17 -Wall -Wextra
CPPFLAGS += -I$(SDKTARGETSYSROOT)/usr/include/opencv4
LDLIBS += -lsp500 -lopencv_imgcodecs -lopencv_core

.PHONY: all clean $(TARGETS)

all: $(TARGETS)

$(TARGETS): sp500_letterbox.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDLIBS)

clean:
	$(RM) $(CLEAN_FILES)
endif
