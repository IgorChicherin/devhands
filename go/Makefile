NAME=miniserver
VESION=0.0.1

export PROJ_ROOT := ${CURDIR}
export APP_NAME := ${NAME}

go-compile: build

.PHONY: build
build:
	 @go build -o ${PROJ_ROOT}/${APP_NAME} ${PROJ_ROOT}/main.go

.PHONY: clean 
clean:
	@rm -f ./${APP_NAME}

.PHONY: help
all: help

# help: show this help message
help: Makefile
	@echo
	@echo " Choose a command to run in "$(APP_NAME)":"
	@echo
	@sed -n 's/^##//p' $< | column -t -s ':' |  sed -e 's/^/ /'
	@echo

