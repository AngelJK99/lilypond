
$(outdir)/%.dvi: %.mf
	mf $<
	gftodvi  $(basename $<)
	mv   $(basename $<).dvi $(outdir)
	rm $(basename $<).*gf

$(outdir)/%.log: %.mf
	mf $<
	mv $(@F) $@
	rm $(basename $< ).*gf


$(outdir)/%.tfm: %.mf
	mf '\mode:=ljfour;  input $<;'
	mv $(@F) $(outdir)
	rm $(basename $<).*gf $(basename $<).*log
