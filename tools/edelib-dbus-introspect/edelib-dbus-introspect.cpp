#include <stdio.h>
#include <string.h>
#include <edelib/EdbusConnection.h>

#define INTROSPECTABLE_INTERFACE "org.freedesktop.DBus.Introspectable"
#define INTROSPECTABLE_METHOD    "Introspect"

EDELIB_NS_USE;

void help(void) {
	puts("Usage: edelib-dbus-introspect [--system] <object-path> <destination>");
}

int main(int argc, char** argv) {
	if(argc != 3 && argc != 4) {
		help();
		return 1;
	}

	EdbusConnectionType t;
	const char* object;
	const char* destination;

	if(strcmp(argv[1], "--system") == 0) {
		t = EDBUS_SYSTEM;
		object = argv[2];
		destination = argv[3];
	} else {
		t = EDBUS_SESSION;
		object = argv[1];
		destination = argv[2];
	}

	EdbusConnection con;
	if(!con.connect(t)) {
		printf("Failed to connect to %s bus!\n", (t == EDBUS_SYSTEM ? "system" : "session"));
		printf("Make sure you have correctly running dbus-daemon\n");
		return 1;
	}

	EdbusMessage msg, reply;
	msg.create_method_call(destination, object , INTROSPECTABLE_INTERFACE, INTROSPECTABLE_METHOD);

	if(!con.send_with_reply_and_block(msg, 1000, reply)) {
		printf("Failed to get reply from bus!\n");
		return 1;
	}

	if(reply.size() == 1) {
		EdbusMessage::const_iterator it = reply.begin();
		if(!(*it).is_string()) {
			printf("Expected string, but received another type. This is error in service\n");
			return 1;
		}

		printf("%s\n", (*it).to_string());
	}

	return 0;
}
