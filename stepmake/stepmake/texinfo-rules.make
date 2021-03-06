
.SUFFIXES: .html .info .texi .texinfo

# "makeinfo --info" MUST be able to read PNGs from CWD for info images
# to work, hence $(INFO_IMAGES_DIR) -> $(outdir)/ symlink.
# $(outdir)/$(INFO_IMAGES_DIR)/*.png symlinks are only needed to view
# out-www/*.info with Emacs -- HTML docs no longer need these
# symlinks, see replace_symlinks_urls in
# python/auxiliar/postprocess_html.py.

# make dereferences symlinks, and $(INFO_IMAGES_DIR) is a symlink
# to $(outdir), so we can't use directly $(INFO_IMAGES_DIR) as a
# prerequisite, otherwise %.info are always outdated (because older
# than $(outdir)), hence this .dep file

$(outdir)/$(INFO_IMAGES_DIR).info-images-dir-dep: $(OUT_TEXI_FILES)
	$(call ly_progress,Making,$@,(symlinks))
ifneq ($(INFO_IMAGES_DIR),)
	rm -f $(INFO_IMAGES_DIR)
	ln -s $(outdir) $(INFO_IMAGES_DIR)
	mkdir -p $(outdir)/$(INFO_IMAGES_DIR)
	rm -f $(outdir)/$(INFO_IMAGES_DIR)/[a-f0-9][a-f0-9]
	cd $(outdir)/$(INFO_IMAGES_DIR) && $(PYTHON) $(buildscript-dir)/mass-link.py symbolic .. . [a-f0-9][a-f0-9]
endif
	touch $@

# Copy files while tracking their dependencies.
$(outdir)/%.texi: %.texi $(outdir)/version.itexi
	$(call ly_progress,Making,$@,(copy))
	mkdir -p $(dir $@)
	$(DO_TEXI_DEP) cp -f $< $@

$(outdir)/%.itexi: %.itexi
	$(call ly_progress,Making,$@,(copy))
	mkdir -p $(dir $@)
	$(DO_TEXI_DEP) cp -f $< $@

$(outdir)/%.info: $(outdir)/%.texi $(outdir)/$(INFO_IMAGES_DIR).info-images-dir-dep $(outdir)/version.itexi $(outdir)/weblinks.itexi | $(OUT_TEXINFO_MANUALS)
	$(call ly_progress,Making,$@,< texi)
	$(buildscript-dir)/run-and-check.sh "$(MAKEINFO) -I$(src-dir) -I$(outdir) --output=$@ $<" "$(outdir)/$*.makeinfo.log"

$(outdir)/%-big-page.html: $(outdir)/%.texi $(outdir)/version.itexi $(outdir)/weblinks.itexi | $(OUT_TEXINFO_MANUALS)
	$(call ly_progress,Making,$@,< texi)
	$(buildscript-dir)/run-and-check.sh "DEPTH=$(depth) $(TEXI2HTML) $(TEXI2HTML_FLAGS) -D bigpage -D web_version --output=$@ $<"  "$(outdir)/$*.bigtexi.log"

$(outdir)/%.html: $(outdir)/%.texi $(outdir)/version.itexi $(outdir)/weblinks.itexi | $(OUT_TEXINFO_MANUALS)
	$(call ly_progress,Making,$@,< texi)
	$(buildscript-dir)/run-and-check.sh "DEPTH=$(depth) $(TEXI2HTML) $(TEXI2HTML_FLAGS) --output=$@ $<"  "$(outdir)/$*.texilog.log"

$(outdir)/%/index.html: $(outdir)/%.texi $(outdir)/version.itexi $(outdir)/weblinks.itexi | $(OUT_TEXINFO_MANUALS)
	$(call ly_progress,Making,$@,< texi)
	mkdir -p $(dir $@)
	$(buildscript-dir)/run-and-check.sh "DEPTH=$(depth)/../ $(TEXI2HTML) $(TEXI2HTML_SPLIT) $(TEXI2HTML_FLAGS) --output=$(dir $@) $<"  "$(outdir)/$*.splittexi.log"

$(outdir)/%.pdf: $(outdir)/%.texi $(outdir)/version.itexi $(outdir)/weblinks.itexi | $(OUT_TEXINFO_MANUALS)
	$(call ly_progress,Making,$@,< texi)
	TEX=$(PDFTEX) PDFTEX=$(PDFTEX) PDFLATEX=$(PDFLATEX) \
		$(buildscript-dir)/run-and-check.sh \
			"cd $(outdir); \
				texi2pdf --batch $(TEXI2PDF_FLAGS) \
					$(TEXI2PDF_QUIET) \
					-I $(abs-src-dir) \
					$(TEXINFO_PAPERSIZE_OPTION) \
					-o $*.tmp.pdf \
					$(<F) \
					< /dev/null" \
			"$(outdir)/$*.texi2pdf.log"
ifeq ($(USE_EXTRACTPDFMARK),yes)
	$(EXTRACTPDFMARK) -o $(outdir)/$*.pdfmark $(outdir)/$*.tmp.pdf
	$(GS920) -dBATCH \
                 -dNOSAFER \
                 -dNOPAUSE \
                 $(TEXINFO_GS_QUIET) \
                 -sDEVICE=pdfwrite \
                 -dAutoRotatePages=/None \
                 -dPrinted=false \
                 -sOutputFile=$@ \
                 -c "30000000 setvmthreshold" \
                 -I $(top-build-dir)/out-fonts \
                 -I $(top-build-dir)/out-fonts/Font \
                 $(outdir)/$*.pdfmark \
                 $(outdir)/$*.tmp.pdf
	rm $(outdir)/$*.tmp.pdf
else
	mv $(outdir)/$*.tmp.pdf $@
endif

$(outdir)/%.txt: $(outdir)/%.texi $(outdir)/version.itexi $(outdir)/weblinks.itexi | $(OUT_TEXINFO_MANUALS)
	$(call ly_progress,Making,$@,< texi)
	$(buildscript-dir)/run-and-check.sh "$(MAKEINFO) -I$(src-dir) -I$(outdir) --no-split --no-headers --output $@ $<"  "$(outdir)/$*.makeinfotxt.log"

$(outdir)/version.itexi: $(top-src-dir)/VERSION
	$(call ly_progress,Making,$@,)
	$(PYTHON) $(buildscript-dir)/create-version-itexi.py > $@

$(outdir)/weblinks.itexi: $(top-src-dir)/VERSION
	$(call ly_progress,Making,$@,)
	$(PYTHON) $(buildscript-dir)/create-weblinks-itexi.py > $@
