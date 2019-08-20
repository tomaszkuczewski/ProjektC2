#include "linkpack.h"

int merge_packet(struct ether_header eh, struct iphdr ih, struct tcphdr th, int random_data, void** output_data)
{
	struct pseudo_hdr
	{
	    unsigned int ip_src;
	    unsigned int ip_dst;
	    unsigned char protocol;
	    unsigned char reserved;
	    unsigned short size;
	};

	char* generated_data = NULL;
	int size_header = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct tcphdr);

	if(random_data > 0)
	{
		generated_data = malloc(random_data);
		for(int i = 0; i < random_data; i++) { generated_data[i] = xorshf96(); }
	}

	if(ih.check == 0)
	{
		ih.check = short_checksum((unsigned short*) &ih, sizeof(struct iphdr));
	}
	if(th.check == 0)
	{
		struct pseudo_hdr pseudo;
		memset(&pseudo, 0, sizeof(struct pseudo_hdr));
		pseudo.ip_src = ih.saddr;
		pseudo.ip_dst = ih.daddr;
		pseudo.reserved = 0;
		pseudo.protocol = 6;
		pseudo.size = htons(sizeof(struct tcphdr) + random_data);
		th.check = htons(short_checksum((unsigned short*) &pseudo, sizeof(struct pseudo_hdr)));

		int check_size = sizeof(struct pseudo_hdr) + sizeof(struct tcphdr) + random_data;
		unsigned char * checksum_data = malloc(check_size);
		memcpy(checksum_data, &pseudo, sizeof(struct pseudo_hdr));
		memcpy(checksum_data + sizeof(struct pseudo_hdr), &th, sizeof(struct tcphdr));
		if(random_data > 0)
		{
			memcpy(checksum_data + check_size - random_data, generated_data, random_data);
		}
		th.check = short_checksum((unsigned short*) checksum_data, check_size);
		free(checksum_data);
	}

	char* data = malloc(size_header + random_data);
	memcpy(data, &eh, sizeof(struct ether_header));
	memcpy(data + sizeof(struct ether_header), &ih, sizeof(struct iphdr));
	memcpy(data + sizeof(struct ether_header) + sizeof(struct iphdr), &th, sizeof(struct tcphdr));
	if(random_data > 0)
	{
		memcpy(data + size_header, generated_data, random_data);
		free(generated_data);
	}
	*output_data = data;
	return size_header + random_data;
}

unsigned short short_checksum(unsigned short * buffer, int size)
{
    unsigned long cksum=0;
    while(size >1)
    {
        cksum+=*buffer++;
        size -=sizeof(unsigned short);
    }
    if(size)
        cksum += *(unsigned char*)buffer;

    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >>16);
    return (unsigned short)(~cksum);
}
