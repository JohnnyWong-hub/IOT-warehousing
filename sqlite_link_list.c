#include "sqlite_link_list.h"
#include "data_global.h"

//定义了全局变量头结点，尾结点
extern slinklist slinkHead, slinkTail;

slinklist sqlite_CreateEmptyLinklist ()
{
    slinklist h = (slinklist)malloc (sizeof (slinknode));
    if (NULL == h)
    {
        printf("Malloc failed\n", );
        return NULL;
    }
    return h;
}

//判断链表是否为空
int sqlite_EmptyLinklist (slinklist h)
{
    return NULL == h->next;
}

//取出链表的一个节点
slinklist sqlite_GetLinknode (slinklist h)
{
    if (1 == sqlite_EmptyLinklist(h))
    {
        return NULL;
    }
    slinklist p = h->next;
    if (p->next == NULL)
        slinkTail = h;
    return p;
}

//插入一个节点
int sqlite_InsertLinknode (int operation, struct env_info_clien_addr y,
                            int storageNum_1, int goodsKinds_1)
{
    struct sqlite_operation sqlite_operation_t;
    switch(operation & 0xff)
    {
        case 0x00:
            sqlite_operation_t.table_select_mask = 0;
            sqlite_operation_t.env_operation_mask = 0;
            break;
            case 0x01:
    		sqlite_operation_t.table_select_mask = 0;
    		sqlite_operation_t.env_operation_mask = 1;
    		break;
    	case 0x10:
    		sqlite_operation_t.table_select_mask = 1;
    		sqlite_operation_t.table_operation_mask = 0;
    		break;
    	case 0x11:
    		sqlite_operation_t.table_select_mask = 1;
    		sqlite_operation_t.table_operation_mask = 1;
    		break;
    	case 0x12:
    		sqlite_operation_t.table_select_mask = 1;
    		sqlite_operation_t.table_operation_mask = 2;
    		break;
    	case 0x20:
    		sqlite_operation_t.table_select_mask = 2;
    		sqlite_operation_t.goods_operation_mask = 0;
    		break;
    	case 0x21:
    		sqlite_operation_t.table_select_mask = 2;
    		sqlite_operation_t.goods_operation_mask = 1;
    		break;
    	case 0x22:
    		sqlite_operation_t.table_select_mask = 2;
    		sqlite_operation_t.goods_operation_mask = 2;
    		break;
    	case 0x23:
    		sqlite_operation_t.table_select_mask = 2;
    		sqlite_operation_t.goods_operation_mask = 3;
    		break;
    }

    slinklist q = (slinklist)malloc (sizeof (slinknode));
    if (NULL == q)
    {
        printf("Malloc failed\n");
        return -1;
    }
    slinkTail->next = q;
    slinkTail = q;
    q->data = sqlite_operation_t;
    q->data_link = y;
    q->storageNum = storageNum_1;
    q->goodsKinds = goodsKinds_1;
    q->next = NULL;
    return 0;
}
