/*
 *	analylsis server traffic applying trie tree
 *
 * input.txt:(format: ip"\t"traffic"\t"time)
 * 10.128.81.197	0.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.67	5.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.32	16.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.88	9.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.96	8.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.65	6.00	Thu Oct 18 22:15:00 2012
 * 10.130.81.56	19.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.107	0.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.101	10.00	Thu Oct 18 22:15:00 2012
 * 10.128.81.193	14.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.40	26.00	Thu Oct 18 22:15:00 2012
 * 10.130.81.51	20.00	Thu Oct 18 22:15:00 2012
 * 10.128.81.140	3.00	Thu Oct 18 22:15:00 2012
 * 10.132.81.31	20.00	Thu Oct 18 22:15:05 2012
 * 10.130.81.38	14.00	Thu Oct 18 22:15:05 2012
 * 10.132.81.32	19.00	Thu Oct 18 22:15:05 2012
 *
*/
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
struct traff_info{
	char server_ip[20];
	float server_traff;
	char log_time[32];
};

#define MAX_CHILD 256
#define TREE_HIGH sizeof(struct in_addr)
struct stat_info{
	char server_ip[20];
	float sum_traff;
	float sum_count;
};

struct trie_node{
	int tree_high;
	int sum_leaf;
	struct trie_node *child[MAX_CHILD];
	struct stat_info *stat_if;
};

int insert_trie(struct trie_node *trie_root, struct traff_info *trf_if);
int travel_trie(struct trie_node *trie_root, int (*callback_fun)());
int destry_trie(struct trie_node *trie_root);
int disp_stat(struct stat_info *stat_if);


float sum_traff_g;

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("Usage: <command> filename\n");
		return -1;
	}
	char *filename = argv[1];
	printf("filename=%s\n",filename);
	
	
	struct trie_node *trie_root = calloc(1, sizeof(struct trie_node));
	trie_root->tree_high = TREE_HIGH;
	trie_root->sum_leaf = 0;
	
	struct traff_info trf_if;
	FILE *fp_stat = fopen(filename, "r");
        if(NULL == fp_stat)
        {
            return -1;
        }
	while(EOF != fscanf(fp_stat, "%s\t%f%[^\n\r]", trf_if.server_ip, &trf_if.server_traff, trf_if.log_time))
	{
		//printf("%s %f\n", trf_if.server_ip, trf_if.server_traff, trf_if.log_time);
		insert_trie(trie_root, &trf_if);
	}
	
	travel_trie(trie_root, disp_stat);	
	fprintf(stderr, "\nserver_num: %d, sum_traff: %f\n", trie_root->sum_leaf, sum_traff_g);
	destry_trie(trie_root);	
	return 0;
	
}

int insert_trie(struct trie_node *trie_root, struct traff_info *trf_if)
{
	struct in_addr ip_addr;
	inet_pton(AF_INET, trf_if->server_ip, (void*)&ip_addr);
	char *index = (char*)&ip_addr;
	struct trie_node *curr = trie_root;
	struct trie_node *next;
	int new_node_flag = 0;
	int i = 0;
	for(i = 0; i < trie_root->tree_high; i++)
	{
		next = curr->child[*(index + i)];
		if(NULL == next )
		{
			next = calloc(1, sizeof(struct trie_node));
			curr->child[*(index + i)] = next;
			new_node_flag = 1;
		}
		curr = next;
	}
	if(1 == new_node_flag)
	{
		curr->stat_if = calloc(1, sizeof(struct stat_info));
		trie_root->sum_leaf++;
	}

	struct stat_info *stat = curr->stat_if;
	memcpy(stat->server_ip, trf_if->server_ip, sizeof(trf_if->server_ip));
	stat->sum_traff += trf_if->server_traff;
	stat->sum_count++;
	return 0;
}


int travel_trie(struct trie_node *trie_root, int (*callback_fun)()) 
{
	struct trie_node *curr = trie_root;
	struct trie_node *next;
	int i, j;
	if(NULL != curr->stat_if)
	{
		callback_fun(curr->stat_if);
		return 0;
	}
	for(j = 0; j < MAX_CHILD; j++)
	{
		next = curr->child[j];
		if(NULL != next)
		{
			travel_trie(next, callback_fun);
		}
	}
	return 0;
}

int destry_trie(struct trie_node *trie_root)
{
	struct trie_node *curr, *next;
	curr = trie_root;
	if(NULL != curr->stat_if)
	{
		free(curr->stat_if);
	}
	int j;
	for(j = 0; j < MAX_CHILD; j++)
	{
		next = curr->child[j];
		if(NULL != next)
		{
			destry_trie(next);
		}
	}
	free(curr);
		
}

int disp_stat(struct stat_info *stat_if)
{
	float avg_traff = stat_if->sum_traff/stat_if->sum_count;
	//printf("server_ip:%s\tavg_traff:%f\n", stat_if->server_ip, stat_if->sum_traff/stat_if->sum_count);
	printf("%s\t%f\t%f\t%f\n", 
		stat_if->server_ip, 
		stat_if->sum_traff/stat_if->sum_count, 
		stat_if->sum_traff, 
		stat_if->sum_count);
	sum_traff_g += avg_traff;
	return 0;
}