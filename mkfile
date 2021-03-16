</$objtype/mkfile

ALL=tsp worker

all:V: $ALL

tsp: tsp.$O
	$O^l -o tsp tsp.$O

worker: worker.$O
	$O^l -o worker worker.$O

tsp.$O: tsp.c
	$O^c -FTVw tsp.c

worker.$O: worker.c
	$O^c -FTVw worker.c

clean:V:
	rm -f *.[$OS] $ALL

