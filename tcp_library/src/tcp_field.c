#include "tcp_field.h"

void set_tcp_field(void* hdr, unsigned int id, char* str) {
	struct tcphdr * tcph = (struct tcphdr *) hdr;
	if (id == 0) {
		tcph->source = htons(atoi(str));
	} else if (id == 1) {
		tcph->dest = htons(atoi(str));
	} else if (id == 2) {
		tcph->seq = htonl(atoi(str));
	} else if (id == 3) {
		tcph->ack_seq = htonl(atoi(str));
	} else if (id == 4) {
		tcph->window = htons(atoi(str));
	} else if (id == 5) {
		tcph->check = htons(atoi(str));
	} else if (id == 6) {
		tcph->urg_ptr = htons(atoi(str));
	}
}
