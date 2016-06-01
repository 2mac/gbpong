CC=sdcc
CFLAGS=-mgbz80 -Ilibstdgb -Llibstdgb
ROMGEN=./libstdgb/gbromgen --verbose

COMPILE=$(CC) $(CFLAGS) -c

all: pong

pong: stdgb
	$(COMPILE) src/gamemode.c
	$(COMPILE) src/gamemode_game.c
	$(COMPILE) src/gamemode_splash.c
	$(COMPILE) src/text.c
	$(CC) $(CFLAGS) src/pong.c gamemode.rel gamemode_game.rel \
		gamemode_splash.rel text.rel -lstdgb
	$(ROMGEN) rom-spec.json

stdgb:
	$(MAKE) -C libstdgb/

clean:
	rm -f *.asm *.gb *.lk *.map *.rel *.ihx *.lst *.noi *.sym
