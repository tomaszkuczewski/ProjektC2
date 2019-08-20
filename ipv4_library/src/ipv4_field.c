#include "ipv4_field.h"

void set_ipv4_field(void* hdr, unsigned int id, char* str) {
	struct iphdr * iph = (struct iphdr *) hdr;
	if (id == 0) {
		iph->ihl = atoi(str);
	} else if (id == 1) {
		iph->tot_len = htons(atoi(str));
	} else if (id == 2) {
		iph->id = htons(atoi(str));
	} else if (id == 3) {
		iph->ttl = atoi(str);
	} else if (id == 4) {
		iph->check = htons(atoi(str));
	} else if (id == 5) {
		struct sockaddr_in addr;
		inet_pton(AF_INET, str, &(addr.sin_addr));
		iph->saddr = addr.sin_addr.s_addr;
	} else if (id == 6) {
		struct sockaddr_in addr;
		inet_pton(AF_INET, str, &(addr.sin_addr));
		iph->daddr = addr.sin_addr.s_addr;
	}
}
