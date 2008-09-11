#include <stdio.h>
#include <edelib/Sipc.h>

EDELIB_NS_USE

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: %s <server-name> <message>\n", argv[0]);
		return 1;
	}

	SipcClient c;
	if(!c.connect(argv[1])) {
		printf("Can't connect to %s name\n", argv[1]);
		return 1;
	}

	c.send(argv[2]);
	return 0;
}
