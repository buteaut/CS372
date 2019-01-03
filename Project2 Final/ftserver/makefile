# Makefile for CS372 projects
# Author: Thomas Buteau
# Date: 10-29-17
CC = gcc
CXX = g++
CFLAGS = -std=c99
CXXFLAGS = -std=c++0x
LDFLAGS = 
#Optional additions
#CXXFLAGS += -g
#CXXFLAGS += -Wall
#CXXFLAGS += -pedantic-errors

OBJS = ftserver.o

SRCS = *.c

HEADERS = *.h

EXEC = "ftserver"

#all: Lab3

Program: ${OBJS} ${HEADERS}
	${CXX} ${LDFLAGS} ${OBJS} -o ${EXEC}

${OBJS}: ${SRCS}
	${CXX} ${CXXFLAGS} -c $(@:.o=.c)

clean:
	rm -f ${OBJS} a.out ${EXEC}

