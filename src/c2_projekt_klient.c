/*
 ============================================================================
 Name        : c2_projekt_klient.c
 Author      : Tomasz Kuczewski
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <ifaddrs.h>
#include <dlfcn.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/ether.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>

#include "./utils/string_utils.h"
#include "./utils/linked_list.h"

const int CONSOLE_WIDTH = 80;

const char g_szIpHeaderName[7][2][20] = {
		{"Header length", "hdl",},
		{"Total length", "totl",},
		{"Identification", "id",},
		{"Time to live", "ttl",},
		{"Header checksum", "ipcs",},
		{"Source IP", "sip",},
		{"Destination IP", "dip",},
};

const char g_szTcpHeaderName[7][2][20] =
{
		{"Source port", "sprt",},
		{"Destination port", "dprt",},
		{"Sequence number", "seq",},
		{"Acknow. number", "ack",},
		{"Window size", "wnds",},
		{"Checksum", "tcpcs",},
		{"Urgent pointer", "urgp",},
};

//Wysylanie pakietu, wykorzystanie funkcji bilbiotecznej do enkapsulacji, laczenie w liste i wyslanie socketem
void send_packet(struct iphdr ih, struct tcphdr th, char * int_name, unsigned int packet_count, unsigned int random_count);

//Pobieranie tekstu pol naglowkow ze zmiennych globalnych i zwracanie rozmiaru tekstu
void get_field(const char type, const void* const hdr, unsigned int id, char * out_buff, unsigned int * out_len);

//Wypisywanie zformatowanej tabeli
void print_headers(const struct iphdr* const ih, const struct tcphdr* const th, const int random_bytes);

//Wpisanie wartosci do pola w naglowku
void set_field(char type, void* hdr, unsigned int id, char* str);

//Pobieranie tekstu od uzytkownika (zwracanie danych z wewnatrz [0x00 oznacza blad])
char get_command(struct iphdr* const ih, struct tcphdr* const th, unsigned int * random_bytes, char * int_name);

//Wyszukiwanie oraz ustawianie nazwy i IP interfejsu o danej nazwie
char set_interface(struct iphdr * ih, char* output_name);

//Wyszukiwanie skroconej nazwy pola w naglowku oraz zwracanie id pola jesli istnieje
int is_valid_shortname(const char type, const char * const str);

//int (*merge_packet) (struct ether_header* eh, struct iphdr* ih, struct tcphdr* th, int random_data, void** output_data);
int (*merge_packet) (struct ether_header eh, struct iphdr ih, struct tcphdr th, int random_data, void** output_data);
void (*set_ipv4_field) (void* hdr, unsigned int id, char* str);
void (*set_tcp_field) (void* hdr, unsigned int id, char* str);

int main(void) {
	//Pola naglowka IP oraz TCP
	struct iphdr ipheader;
	struct tcphdr tcpheader;

	memset(&tcpheader, 0, sizeof(struct tcphdr));
	memset(&ipheader, 0, sizeof(struct iphdr));

	//Ilosc losowych danych
	unsigned int random_bytes = 0;

	//Nazwa interfejsu (lo = local loopback)
	char interface_name[20] = "lo";

	//Otwieranie biblioteki
	void* hlib = dlopen("./liblink.so", RTLD_NOW);
	if(hlib == NULL)
	{
		printf("Nie mozna otworzyc biblioteki!\n");
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	//Ladowanie symbolu
	merge_packet = dlsym(hlib, "merge_packet");
	if(merge_packet == NULL)
	{
		printf("Nie mozna zaladowac funkcji merge!\n");
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	//Otwieranie biblioteki
	void* hiplib = dlopen("./ipv4lib.so", RTLD_NOW);
	if(hiplib == NULL)
	{
		printf("Nie mozna otworzyc biblioteki!\n");
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	//Ladowanie symbolu
	set_ipv4_field = dlsym(hiplib, "set_ipv4_field");
	if(set_ipv4_field == NULL)
	{
		printf("Nie mozna zaladowac funkcji IPv4!\n");
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	//Otwieranie biblioteki
	void* htcplib = dlopen("./tcplib.so", RTLD_NOW);
	if(htcplib == NULL)
	{
		printf("Nie mozna otworzyc biblioteki!\n");
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}

	//Ladowanie symbolu
	set_tcp_field = dlsym(htcplib, "set_tcp_field");
	if(set_tcp_field == NULL)
	{
		printf("Nie mozna zaladowac funkcji TCP!\n");
		fprintf(stderr, "%s\n", dlerror());
		return -1;
	}


	//Ustawianie domyslnego interfejsu na loopback
	if(set_interface(&ipheader, interface_name) == 0x00)
	{
		printf("Nie mozna odnalezc interfejsu zwrotnego");
		return -2;
	}

	//Domyslne wartosci RFC
	ipheader.ihl = 5;
	tcpheader.source = htons(80);
	tcpheader.dest = htons(80);

	//Wykonywanie seri wyswietlen pol naglowka oraz wysylanie zapytan o komende do uzytkownika
	char return_code = 0x01;
	while(return_code != 0x00)
	{
		print_headers(&ipheader, &tcpheader, random_bytes);
		return_code = get_command(&ipheader, &tcpheader, &random_bytes, interface_name);
	}

	return EXIT_SUCCESS;
}

char get_command(struct iphdr* ih, struct tcphdr* th, unsigned int * random_bytes, char * int_name)
{
	int arguments = 0;
	char** args;
	char input[100];

	printf("Komenda: ");
	fgets(input, 100, stdin);
	split_string(input, strlen(input), &args, &arguments);
	//Dostepne komendy
	//set ipv4 <name> <value>
	//set tcp <name> <value>
	//set random bytes <value>
	//set output interface <name>
	//exit

	if(arguments == 1)
	{
		if(strncmp("exit", args[0], 4) == 0)
		{
			split_string_free(&args, arguments);
			return 0x00;
		}
	}
	if(arguments == 2)
	{
		if(strncmp("send", args[0], 4) == 0)
		{
			send_packet(*ih, *th, int_name, atoi(args[1]), *random_bytes);

			split_string_free(&args, arguments);
			return 0x01;
		}
	}
	if(arguments == 4)
	{
		if(strncmp("set", args[0], 3) == 0)
		{
			if(strncmp("ipv4", args[1], 4) == 0)
			{
				int field = is_valid_shortname(0x00, args[2]);
				if(field != -1) { set_field(0x00, ih, field, args[3]); }
				else {
					printf("Nie poprawny skrot pola naglowka! (Nacisnij przycisk)");
					getchar();
					split_string_free(&args, arguments);
					return 0x01;
				}
				split_string_free(&args, arguments);
				return 0x01;
			}
			if(strncmp("tcp", args[1], 4) == 0)
			{
				int field = is_valid_shortname(0x01, args[2]);
				if(field != -1) { set_field(0x01, th, field, args[3]); }
				else {
					printf("Nie poprawny skrot pola naglowka! (Nacisnij przycisk)"); getchar();
					split_string_free(&args, arguments);
					return 0x01;
				}
				split_string_free(&args, arguments);
				return 0x01;

			}
			if ( (strncmp("random", args[1], 6) == 0) && (strncmp("bytes", args[2], 5) == 0) )
			{
				*random_bytes = (unsigned int) atoi(args[3]);
				split_string_free(&args, arguments);
				return 0x01;
			}
			if( (strncmp("output", args[1], 6) == 0) && (strncmp("interface", args[2], 9) == 0) )
			{
				if(strlen(args[3]) > 19)
				{
					printf("Zbyt dluga nazwa interfejsu! (Nacisnij przycisk)");
					getchar();
					split_string_free(&args, arguments);
					return 0x01;
				}
				if(set_interface(ih, args[3]) == 0x00)
				{
					printf("Nie poprawna nazwa interfejsu! (Nacisnij przycisk)");
					getchar();
					split_string_free(&args, arguments);
					return 0x01;
				}
				strncpy(int_name, args[3], strlen(args[3]));
				split_string_free(&args, arguments);
				return 0x01;
			}
		}
	}
	printf("\n");
	split_string_free(&args, arguments);
	return 0x01;
}

void send_packet(struct iphdr ih, struct tcphdr th, char * int_name, unsigned int packet_count, unsigned int random_count)
{
	int sockfd = -1;
	struct ether_header eh;
	memset(&eh, '\0', sizeof(struct ether_header));

	if ((sockfd = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW)) == -1) { perror("socket"); }

	//ID interfejsu
	struct ifreq if_idx;
	memset(&if_idx, 0, sizeof(struct ifreq));
	strncpy(if_idx.ifr_name, int_name, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
	    perror("SIOCGIFINDEX");
	//MAC
	struct ifreq if_mac;
	memset(&if_mac, 0, sizeof(struct ifreq));
	strncpy(if_mac.ifr_name, int_name, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
	    perror("SIOCGIFHWADDR");

	eh.ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
	eh.ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
	eh.ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
	eh.ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
	eh.ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
	eh.ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
	eh.ether_dhost[0] = 0x00; eh.ether_dhost[1] = 0x11;
	eh.ether_dhost[2] = 0x22; eh.ether_dhost[3] = 0x33;
	eh.ether_dhost[4] = 0x44; eh.ether_dhost[5] = 0x55;
	eh.ether_type = htons(ETH_P_IP);

	struct sockaddr_ll socket_address;
	socket_address.sll_ifindex = if_idx.ifr_ifindex;
	socket_address.sll_halen = ETH_ALEN;
	socket_address.sll_addr[0] = 0x00; socket_address.sll_addr[1] = 0x11;
	socket_address.sll_addr[2] = 0x22; socket_address.sll_addr[3] = 0x33;
	socket_address.sll_addr[4] = 0x44; socket_address.sll_addr[5] = 0x55;

	//Rest
	ih.version = 4;
	ih.protocol = 6;
	th.doff = 5;

	if(th.urg_ptr != 0) { th.urg = 1; }
	if(th.ack_seq != 0) { th.ack = 1; }
	if(ih.tot_len == 0)
	{
		ih.tot_len = htons(sizeof(struct iphdr)
				+ sizeof(struct tcphdr) + random_count);
	}

	//Wypelnianie listy
	linked_list* list = create_list();
	for(int i = 0; i < packet_count; i++)
	{
		unsigned char* packet_data = NULL;
		int size = merge_packet(eh, ih, th, random_count, (void *)&packet_data);
		add_element(list, packet_data, size);
	}

	//Usuwanie listy
	while(list->size > 0)
	{
		unsigned int size = 0;
		unsigned char* packet_data = get_element(list, list->size - 1, &size);
		if (sendto(sockfd, packet_data, size, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
			printf("Send failed\n");

		free(packet_data);
		remove_last_element(list);
	}
}

void print_headers(const struct iphdr* const ih, const struct tcphdr* const th, const int random_bytes)
{
	for(int i = 0; i < 20; i++) { printf("\n"); }
	unsigned ipstrlen = 0, tcpstrlen = 0;
	char ipstrbuff[64], tcpstrbuff[64];
	//Gorny wiersz
	for(int i = 0; i < CONSOLE_WIDTH; i++) {printf("#");} printf("\n");
	printf("# IP HEADER"); for(int i = 0; i < ((CONSOLE_WIDTH/2)-12); i++) {printf(" ");}
	printf("# TCP HEADER"); for(int i = 0; i < ((CONSOLE_WIDTH/2)-12); i++) {printf(" ");} printf("#\n");
	for(int i = 0; i < CONSOLE_WIDTH; i++) {printf("#");} printf("\n");
	//Pisanie kolejnych wierszy
	for(int i = 0; i < 7; i++)
	{
		//NULLowanie tablic
		memset(ipstrbuff, '\0', 64);
		memset(tcpstrbuff, '\0', 64);
		//Pobieranie stringu dla wybranego pola
		get_field(0x00, ih, i, ipstrbuff, &ipstrlen);
		get_field(0x01, th, i, tcpstrbuff, &tcpstrlen);
		printf("# %s", ipstrbuff);
		for(int i = 0; i < ((CONSOLE_WIDTH/2) - ipstrlen - 3); i++) { printf(" "); }
		printf("# %s", tcpstrbuff);
		for(int i = 0; i < ((CONSOLE_WIDTH/2) - tcpstrlen - 2); i++) { printf(" "); } printf("#\n");
	}
	//Dolny wiersz
	for(int i = 0; i < CONSOLE_WIDTH; i++) {printf("#");} printf("\n");
	printf("Ilosc danych losowych: %d", random_bytes);
	printf("\nKomendy: \n\n");
	printf("\tset <ipv4/tcp> <skrocona_nazwa_pola_z_nawiasu> <wartosc>\n");
	printf("\tset random bytes <ilosc_danych_losowych>\n");
	printf("\tset output interface <nazwa_interfejsu>\n");
	printf("\texit\n");

}

void set_field(char type, void* hdr, unsigned int id, char* str)
{
	if(type == 0x00)
	{
		set_ipv4_field(hdr, id, str);
//		struct iphdr * iph = (struct iphdr *) hdr;
//		if(id == 0) { iph->ihl = atoi(str); }
//		else if(id == 1) { iph->tot_len = htons(atoi(str)); }
//		else if(id == 2) { iph->id = htons(atoi(str)); }
//		else if(id == 3) { iph->ttl = atoi(str); }
//		else if(id == 4) { iph->check = htons(atoi(str)); }
//		else if(id == 5)
//		{
//			struct sockaddr_in addr;
//			inet_pton(AF_INET, str, &(addr.sin_addr));
//			iph->saddr = addr.sin_addr.s_addr;
//		}
//		else if(id == 6)
//		{
//			struct sockaddr_in addr;
//			inet_pton(AF_INET, str, &(addr.sin_addr));
//			iph->daddr = addr.sin_addr.s_addr;
//		}
	}
	if(type == 0x01)
	{
		set_tcp_field(hdr, id, str);
//		struct tcphdr * tcph = (struct tcphdr *) hdr;
//		if(id == 0) { tcph->source = htons(atoi(str)); }
//		else if(id == 1) { tcph->dest = htons(atoi(str)); }
//		else if(id == 2) { tcph->seq = htonl(atoi(str)); }
//		else if(id == 3) { tcph->ack_seq = htonl(atoi(str)); }
//		else if(id == 4) { tcph->window = htons(atoi(str)); }
//		else if(id == 5) { tcph->check = htons(atoi(str)); }
//		else if(id == 6) { tcph->urg_ptr = htons(atoi(str)); }
	}
}

void get_field(const char type, const void* const hdr, unsigned int id, char * out_buff, unsigned int * out_len)
{
	//Typ IP
	if(type == 0x00)
	{
		const struct iphdr * const iph = (const struct iphdr * const) hdr;
		*out_len = sprintf(out_buff, "%s(%s)", g_szIpHeaderName[id][0], g_szIpHeaderName[id][1]);
		if(id == 0){*out_len = sprintf(out_buff, "%s: %d", out_buff, iph->ihl);}
		else if(id == 1) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohs(iph->tot_len));}
		else if(id == 2) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohs(iph->id));}
		else if(id == 3) {*out_len = sprintf(out_buff, "%s: %d", out_buff, iph->ttl);}
		else if(id == 4) {*out_len = sprintf(out_buff, "%s: %02x", out_buff, ntohs(iph->check));}
		else if(id == 5)
		{
			struct in_addr ip_addr;
			ip_addr.s_addr = iph->saddr;
			*out_len = sprintf(out_buff, "%s: %s", out_buff, inet_ntoa(ip_addr) );
		}
		else if(id == 6)
		{
			struct in_addr ip_addr;
			ip_addr.s_addr = iph->daddr;
			*out_len = sprintf(out_buff, "%s: %s", out_buff, inet_ntoa(ip_addr) );
		}
	}
	//Typ TCP
	if(type == 0x01)
	{
		const struct tcphdr * const tcph = (const struct tcphdr * const) hdr;
		*out_len = sprintf(out_buff, "%s(%s)", g_szTcpHeaderName[id][0], g_szTcpHeaderName[id][1]);
		if(id == 0){*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohs(tcph->source));}
		else if(id == 1) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohs(tcph->dest));}
		else if(id == 2) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohl(tcph->seq));}
		else if(id == 3) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohl(tcph->ack_seq));}
		else if(id == 4) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohs(tcph->window));}
		else if(id == 5) {*out_len = sprintf(out_buff, "%s: %02x", out_buff, ntohs(tcph->check));}
		else if(id == 6) {*out_len = sprintf(out_buff, "%s: %d", out_buff, ntohs(tcph->urg_ptr));}
	}
}

int is_valid_shortname(const char type, const char * const str)
{
	const char (*compare)[2][20] = g_szIpHeaderName;
	if(type == 0x01) { compare = g_szTcpHeaderName; }

	for(int i = 0; i < 7; i++)
	{
		if(strncmp(compare[i][1], str,
				strlen(compare[i][1])) == 0)
		{
			return i;//OK
		}
	}
	return -1; //Nie znaleziono
}

char set_interface(struct iphdr * ih, char* input_name)
{
    struct ifaddrs *ifaddr, *ifa;
    int s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
    	return 0x00;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL)
            continue;

        s = getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        if((strcmp(ifa->ifa_name, input_name)==0)
        		&& (ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
            	return  0x00;
            }
			struct sockaddr_in addr;
			inet_pton(AF_INET, host, &(addr.sin_addr));
			ih->saddr = addr.sin_addr.s_addr;
			ih->daddr = addr.sin_addr.s_addr;
			return 0x01;
        }
    }
	return 0x00;
}



