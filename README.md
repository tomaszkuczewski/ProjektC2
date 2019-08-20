# ProjektC2
C - Network sockets TCP sender with editor

# Uses own implementation of
 - Link list
 - Split string
 
# Usage
 - set ipv4 <name> <value>
 - set tcp <name> <value>
 - set random bytes <value>     - For payload
 - set output interface <name>
 - exit
  
# Field names for IPv4
		{"Header length", "hdl",},
		{"Total length", "totl",},
		{"Identification", "id",},
		{"Time to live", "ttl",},
		{"Header checksum", "ipcs",},
		{"Source IP", "sip",},
		{"Destination IP", "dip",},

# Field names for TCP
		{"Source port", "sprt",},
		{"Destination port", "dprt",},
		{"Sequence number", "seq",},
		{"Acknow. number", "ack",},
		{"Window size", "wnds",},
		{"Checksum", "tcpcs",},
		{"Urgent pointer", "urgp",},
		