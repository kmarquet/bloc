all: expe html

expe:
	./expe.sh && ./generate_perf_tab.sh

html:
	./generate_html.sh

firefox:
	( cd results && firefox	 PERFS-O0/index.html PERFS-O1/index.html PERFS-O2/index.html PERFS-O3/index.html )

