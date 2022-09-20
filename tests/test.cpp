#include "logger.h"

int main(){
	char i=5;
	logger.clog(E, "foo%c\n", i);
	logger.setFile("test_file.txt");
	logger.flog(F_DBG_C, "foo%llu",10);
	F_DBG<<3<<'\n'<<"bar1\n";
	operator<<<const char*, true>(F,"bar3\n");

}
