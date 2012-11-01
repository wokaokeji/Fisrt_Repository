#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int merge(int dst[], int src[], int pre, int mid, int tail);
int merge_sort(int dst[], int src[], int p, int q);

int shuffer_array(int array[], int a_size);
int swap(int *x, int *y);
int disp_array(int array[], int a_size);


int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

int main()
{
	srand((unsigned int)time(NULL));
	int a_len = sizeof(a)/sizeof(a[0]);
	int *dst_a = malloc(sizeof(a));
	
	printf("Original array:\t\t");
	disp_array(a, a_len);
	
	shuffer_array(a, a_len);
	printf("Disordered array:\t");
	disp_array(a, a_len);
	
	merge_sort(dst_a, a, 0, a_len-1);
	printf("Sorted array:\t\t");
	disp_array(dst_a, a_len);	
	
	int i, ite = 100000;	
	while(ite--)
	{
		shuffer_array(a, a_len);
		merge_sort(dst_a, a, 0, a_len-1);
		int tmp = 0;
		for(i = 0; i < a_len; i++)
		{
			assert(dst_a[i] - tmp == 1);//check the sorted array
			tmp = dst_a[i];
		}
	}

	printf("END\n");
	return 0;
}

/**
 * @brief merge a[i] and b[j], i=[pre, mid], j=(mid,tial]
 *
 * @param dst[]
 * @param src[]
 * @param pre
 * @param mid
 * @param tail
 *
 * @return 
 */
int merge(int dst[], int src[], int pre, int mid, int tail)
{
	int i = pre;
	int j = mid + 1;
	int k = pre;
	while(i <= mid || j <= tail)
	{
		
		if(i >  mid || j > tail)
		{
			if(i > mid)
			{
				dst[k++] = src[j++];
			}
			else if(j > tail)
			{
				dst[k++] = src[i++];
			}
		}
		else	if(src[i] < src[j])
		{
			dst[k++] = src[i++];
		}
		else
		{
			dst[k++] = src[j++];
		}

	}
	
}

/**
 * @brief sort a[i], i=[p ,q]
 *
 * @param array[]
 * @param p
 * @param q
 *
 * @return 
 */
int merge_sort(int dst[], int src[], int p, int q)
{
	if(p >= q)
	{
		dst[q] = src[q];
		return 0;
	}
	int mid = (q - p)/2 + p;
	merge_sort(dst, src, p, mid);
	merge_sort(dst, src, mid + 1, q);
	int i;
	for(i = p ; i <=q; i++)
	{
		swap(&src[i], &dst[i]);
	}
	merge(dst, src, p, mid, q);
	
	return 1;
}


int shuffer_array(int a[], int a_size)
{
	int i, index;
	for(i = 0; i < a_size; i++)
	{
		index = rand()%(a_size - i) + i;
		swap(&a[i], &a[index]);
	}
}


int swap(int *x, int *y)
{
	int tmp;
	tmp = *x;
	*x = *y;
	*y = tmp;

	return 1;
}

int disp_array(int a[], int a_size)
{
	int i;
	for(i = 0; i < a_size; i++)
	{
		printf("%d ", a[i]);
	}
	printf("\n");
}
