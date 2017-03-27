CFLAGS = -Wall
BUILDDIR = build

all: $(BUILDDIR) $(BUILDDIR)/myworm

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/myworm: alinghi_worm.c $(BUILDDIR)
	$(CC) $(CFLAGS) -o$@ $<

clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
