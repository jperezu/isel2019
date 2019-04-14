MODELS = model
model_SPEC = spec_state spec_alarm_on spec_alarm_off spec_code spec_light
model_PANARGS = -m100000

all: $(patsubst %, %.verify, $(MODELS))

%.pan: %.pml
	@echo "__________ GENERATING VERIFIER $(*F) __________"
	spin -a $< && gcc $($(*F)_CFLAGS) -o $@ pan.c
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