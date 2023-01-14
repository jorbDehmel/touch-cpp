touch-cpp.out:	main.cpp
	clang++ -pedantic -Wall -o touch-cpp.out main.cpp

install: touch-cpp.out
	sudo cp ./touch-cpp.out /bin/touch-cpp

uninstall:
	sudo rm /bin/touch-cpp

pclean:
	rm -f *.out *.o