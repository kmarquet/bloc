all: expe html

expe:
	./expe.sh && ./generate_perf_tab.sh

html:
	for file in `ls PERFS*.tex`
	do
		latex2html $file
	done
firefox:
	firefox PERFS-O0/index.html PERFS-O1/index.html PERFS-O2/index.html PERFS-O3/index.html

