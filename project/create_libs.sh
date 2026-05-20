#!/bin/sh
set -e

echo "-> A criar a pasta project/lib/..."
mkdir -p project/lib

echo "-> A compilar as 5 bibliotecas externas para project/lib/..."

# Compila cada uma e deita fora o .o, deixando apenas o .a na pasta certa
clang -c libs/timer.c -o project/lib/timer.o && ar rcs project/lib/libtimer.a project/lib/timer.o
clang -c libs/keyboard.c -o project/lib/keyboard.o && ar rcs project/lib/libkeyboard.a project/lib/keyboard.o
clang -c libs/rtc.c -o project/lib/rtc.o && ar rcs project/lib/librtc.a project/lib/rtc.o
clang -c libs/mouse.c -o project/lib/mouse.o && ar rcs project/lib/libmouse.a project/lib/mouse.o
clang -c libs/video-card.c -o project/lib/video-card.o && ar rcs project/lib/libvideo.a project/lib/video-card.o
clang -c libs/utils.c -o project/lib/utils.o && ar rcs project/lib/libutils.a project/lib/utils.o

# Limpar os ficheiros .o temporários para não deixar lixo
rm -f project/lib/*.o

echo "========================================="
echo " Bibliotecas prontas em project/lib/ ! "
echo " Agora podes usar o comando 'make'       "
echo "========================================="