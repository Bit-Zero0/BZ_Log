bz-log:main.cpp
	g++ -o $@ $^ -std=c++11

.PHONY:clean
clean:
	rm -f bz-log 