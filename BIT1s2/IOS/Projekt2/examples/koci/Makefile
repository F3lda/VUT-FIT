#**************************************************
# Makefile pro ukazkove kody do predmetu IOS na FIT VUT.
# Prace se semafory a sdilenou pameti.
# Kody jsou poskytnuty tak jak jsou.
# (C) Radek Koci, 2016
#************************************************

.PHONY: all

all:
	for d in */; do $(MAKE) -C $$d; $(MAKE) -C $$d clean; done

clean:
	for d in */; do $(MAKE) -C $$d clean; done

cleanall:
	for d in */; do $(MAKE) -C $$d cleanall; done

