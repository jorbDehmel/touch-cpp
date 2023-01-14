touch-cpp.out:	main.cpp
	clang++ -pedantic -Wall -o touch-cpp.out main.cpp
	sudo chmod +x ./touch-cpp.out

install: touch-cpp.out
	sudo cp ./touch-cpp.out /usr/bin/touch-cpp

uninstall:
	sudo rm -f /usr/bin/touch-cpp

pclean:
	rm -f *.out *.o