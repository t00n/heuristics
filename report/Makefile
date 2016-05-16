FIGURES = $(subst .dia,.eps,$(wildcard *.dia))

all: Implementation1.pdf Implementation2.pdf

%.pdf: %.tex ${FIGURES}
	pdflatex $<
	bibtex $(subst .tex,,$<)
	pdflatex $<
	pdflatex $<

%.eps: %.dia
	dia -e $@ $<

%.tex: %.md
	lunamark -t latex $< | sed -E 's/\\includegraphics(\{.+\})/\\begin{figure}[H]\\includegraphics[width=\\textwidth]\1\\end{figure}/' > $@

clean:
	rm -f *.aux *.log report.tex

mrproper: clean
	rm -f *.pdf
