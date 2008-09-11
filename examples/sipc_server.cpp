#include <stdio.h>
#include <string.h>

#include <edelib/Sipc.h>
#include <edelib/Listener.h>

EDELIB_NS_USE

static bool should_quit;

static void message_cb(const char *msg, void *argv) {
	if(strcmp(msg, "quit") == 0)
		should_quit = true;

	printf("<client> %s\n", msg);
}

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: %s <name-to-listen-on>\n", argv[0]);
		return 1;
	}

	SipcServer s;
	if(!s.request_name(argv[1])) {
		printf("Can't listen on %s name\n", argv[1]);
		return 1;
	}

	puts("<note> Send 'quit' message to exit");

	should_quit = false;
	s.callback(message_cb, 0);

	while(!should_quit) {
		listener_wait();
		puts("tick");
	}

	return 0;
}
