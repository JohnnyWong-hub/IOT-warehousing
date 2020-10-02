#include "uart_cache.h"

//创建节点
uart_cache_list CreateEmptyCacheList()
{
  uart_cache_list h;
  h = (uart_cache_list) malloc (sizeof(uart_cache_list));
  h->next = NULL;
  return h;
}

//判断头结点是否为空
int EmptyCacheList (uart_cache_list h)
{
  return h->next;
}


//取出节点    h为头结点    t为尾结点
//取出头结点后一个节点
uart_cache_list GetCacheNode (uart_cache_list h, uart_cache_list *t)
{
  if (1 == EmptyCacheList (h))
  {
    return NULL;
  }

  uart_cache_list p = h->next;
  h->next = p->next;
  //如果p为唯一的节点则将头结点的地址存到t中     为什么？
  if (p->next == NULL)
  {
    *t = h;
  }
  return p;
}

//尾插法
int InsertCacheNode(uart_cache_list *t, uart_cache_data x)
{
  //分配内存空间
  uart_cache_list q = (uart_cache_list)malloc(sizeof(uart_cache_node));
  if (NULL == q)
  {
    printf ("InsertCacheNode Error\n");
    return -1;
  }

  (*t)->next = q;
  *t = q;
  q->data = x;
  q->next = NULL;

  return 0;
}
