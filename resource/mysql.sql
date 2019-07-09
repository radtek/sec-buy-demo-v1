CREATE TABLE `user` (  
    `user_id` bigint(20) not null primary key,  
    `user_name` varchar(50) not NULL,  
    `money` float not NULL  
) ENGINE=INNODB DEFAULT CHARSET=utf8;
insert into user(user_id, user_name, money) values(1, "userName1", 100),(2, "userName2", 100),(3, "userName3", 100),(4, "userName4", 100), (5, "userName5", 100),(6, "userName6", 100),(7, "userName7", 100),(8, "userName8", 100),(9, "userName9", 100),(10, "userName10", 100)

CREATE TABLE `product` (  
    `product_id` bigint(20) not null primary key,  
    `product_name` varchar(50) not NULL,  
    `price` float not NULL
) ENGINE=INNODB DEFAULT CHARSET=utf8;
insert into product(product_id, product_name, price) values(1, "productName1", 1),(2, "productName2", 2),(3, "productName3", 3),(4, "productName4", 4), (5, "productName5", 5),(6, "productName6", 6),(7, "productName7", 7),(8, "productName8", 8),(9, "productName9", 9),(10, "productName10", 10),(11, "productName11", 1),(12, "productName12", 2),(13, "productName13", 3),(14, "productName14", 4), (15, "productName15", 5),(16, "productName16", 6),(17, "productName17", 7),(18, "productName18", 8),(19, "productName19", 9),(20, "productName20", 10)



CREATE TABLE `shop` (  
    `shop_id` bigint(20) not null primary key,  
    `shop_name` varchar(50) not NULL,  
    `money` float not NULL
) ENGINE=INNODB DEFAULT CHARSET=utf8;
insert into shop(shop_id, shop_name, money) values(1, "shopName1", 100),(2, "shopName2", 100),(3, "shopName3", 100),(4, "shopName4", 100), (5, "shopName5", 100),(6, "shopName6", 100),(7, "shopName7", 100),(8, "shopName8", 100),(9, "shopName9", 100),(10, "shopName10", 100)

CREATE TABLE `shop_product_relation` (  
    `relation_id` bigint(20) not null primary key, 
    `product_id` bigint(20) not null,
    `shop_id` bigint(20) not null,  
    `amount` bigint(20) not null
) ENGINE=INNODB DEFAULT CHARSET=utf8;
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(1, 1, 1, 100),(2, 11, 1, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(3, 2, 2, 100),(4, 12, 2, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(5, 3, 3, 100),(6, 13, 3, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(7, 4, 4, 100),(8, 14, 4, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(9, 5, 5, 100),(10, 15, 5, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(11, 6, 6, 100),(12, 16, 6, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(13, 7, 7, 100),(14, 17, 7, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(15, 8, 8, 100),(16, 18, 8, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(17, 9, 9, 100),(18, 19, 9, 100);
insert into shop_product_relation(relation_id, product_id, shop_id, amount) values(19, 10, 10, 100),(20, 20, 10, 100);




CREATE TABLE `user_product_relation` (  
    `relation_id` bigint(20) not null primary key, 
    `product_id` bigint(20) not null,
    `user_id` bigint(20) not null,  
    `amount` bigint(20) not null
) ENGINE=INNODB DEFAULT CHARSET=utf8;

CREATE TABLE `buy_order` (  
    `order_id` bigint(20) not null primary key, 
    `product_id` bigint(20) not null,
    `user_id` bigint(20) not null,  
    `shop_id` bigint(20) not null,  
    `amount` bigint(20) not null,
    `total_money` float not NULL
) ENGINE=INNODB DEFAULT CHARSET=utf8;

CREATE TABLE `user_message_queue_task` ( 
    `umsg_task_id` bigint(20) not null primary key, 
    `order_id` bigint(20) not null , 
    `product_id` bigint(20) not null,
    `user_id` bigint(20) not null,  
    `shop_id` bigint(20) not null,  
    `amount` bigint(20) not null,
    `total_money` float not NULL,
    `message_offset` bigint(20) not null,
    `status` bigint(20) not null
) ENGINE=INNODB DEFAULT CHARSET=utf8;

CREATE TABLE `shop_message_queue_task` ( 
    `smsg_task_id` bigint(20) not null primary key, 
    `order_id` bigint(20) not null , 
    `product_id` bigint(20) not null,
    `user_id` bigint(20) not null,  
    `shop_id` bigint(20) not null,  
    `amount` bigint(20) not null,
    `total_money` float not NULL,
    `message_offset` bigint(20) not null,
    `status` bigint(20) not null 
) ENGINE=INNODB DEFAULT CHARSET=utf8;


CREATE TABLE `user_message_queue_info` (  
    `umsgq_id` bigint(20) not null primary key, 
    `accepted_offset` bigint(20) not null
) ENGINE=INNODB DEFAULT CHARSET=utf8;

CREATE TABLE `shop_message_queue_info` (  
    `smsgq_id` bigint(20) not null primary key, 
    `accepted_offset` bigint(20) not null
) ENGINE=INNODB DEFAULT CHARSET=utf8;


alter table user_message_queue_info change  column processed_offset accepted_offset bigint(20) not null
insert into buy_order(order_id, product_id, user_id, shop_id, amount, total_money) values(1, 1 ,1 ,1 ,1 ,1.0);


update user_message_queue_info set accepted_offset = 0;