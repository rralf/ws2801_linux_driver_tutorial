DEMOS = ws2801-demo cpu-load rgb-demo

CFLAGS += -Wall -Wstrict-prototypes -Wtype-limits \
	  -Wmissing-declarations -Wmissing-prototypes
CFLAGS += -O2

all: $(DEMOS)

$(DEMOS): ws2801.o

clean:
	rm -f *.o
	rm -f $(DEMOS)
