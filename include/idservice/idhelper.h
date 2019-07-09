
#ifndef ID_HELPER_H
#define ID_HELPER_H
enum idTypeEnum
{
    order_id,
    user_product_relation_id,
    user_message_queue_task_id,
    shop_message_queue_task_id,
    user_message_queue_info_id,
    shop_message_queue_info_id
}; // 定义枚举类型color_set1
extern char szIdTypeName[6][30];

unsigned long getId(int idType);

#endif
