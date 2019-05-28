MODELS = patinete
patinete_SPEC = spec_buffer spec_send spec_lowbat spec_rent spec_red_led spec_green_led spec_blue_led
patinete_PANARGS = -m200
PROCESS_FLAGS = -DNFAIR=3

all: $(patsubst %, %.verify, $(MODELS))

%.pan: %.pml
	@echo "__________ GENERATING VERIFIER $(*F) __________"
	spin -a $< && gcc $($(*F)_CFLAGS) -o $@ pan.c $(PROCESS_FLAGS)
	rm -f *trail
%.verify: %.pan
	@echo "__________ VERIFYING $(*F) __________"
	for f in $($(*F)_SPEC); do ./$< -a -f $($(*F)_PANARGS) -N $$f; \
		if [ -f *.trail ]; then \
			spin -t -r -s -l -g $(*F).pml; \
		fi; \
	done

clean:
	rm -f *trail *pan pan* *tmp *tcl
	@echo "CLEAN DONE"