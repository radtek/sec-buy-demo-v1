CC=g++
DAO_OBJ=CMysqlDB.o CDiskDBConnPool.o CTableUserMessageQueueInfo.o CTUserMessageQueueTask.o CTableUser.o \
CTableUserProductRelation.o CTableBuyOrder.o CTableProduct.o
EVENT_OBJ=CSocket.o CEventLoop.o  CEventEpoll.o
IPC_OBJ=sharemutex.o
CFG_OBJ=CConfig.o
MEM_DB_OBJ=CRedis.o CMemDBConnectionPool.o idhelper.o
MESSAGE_QUEUE_OBJ=CKafka.o CMsgProducer.o CMsgProducerPool.o
THREAD_OBJ=CThreadPool.o CThreadTask.o
USERSERVICE_TASK_OBJ=CUserConsumerProcessTask.o CClearProcessedTaskStatus.o CUserServiceTaskCheckTask.o
ORDERSERVICE_TASK_OBJ=CPlaceOrderTask.o
USERSERVICE_OBJ=userservicemain.o userserviceglobal.o log.o 
ORDERSERVICE_OBJ=orderservicemain.o orderserviceglobal.o CServer.o CTcpAcceptedClientSocket.o CTcpServerSocket.o  log.o  CAcceptClientTask.o
USER_SERVICE=userservice
ORDER_SERVICE=orderservice
CXXFLAGS=-g -std=c++11
MYSQL_FLAGE=`mysql_config --cflags --libs`
INCLUDE=-I/usr/local/include/librdkafka/ \
-I/usr/local/include/librdkafka/
LIBS=/usr/local/lib/librdkafka++.a /usr/local/lib/librdkafka.a -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu/ \
-lhiredis -lpthread -lz -ldl -lssl -lcrypto
   
all:$(USER_SERVICE) $(ORDER_SERVICE)
$(USER_SERVICE): $(USERSERVICE_OBJ) $(DAO_OBJ) $(MESSAGE_QUEUE_OBJ) $(THREAD_OBJ)\
 $(USERSERVICE_TASK_OBJ) $(MEM_DB_OBJ) $(CFG_OBJ)
	$(CC) -o $(ROOT_DIR)/$(BIN_DIR)/$(USER_SERVICE)  CConnectionPool.o $(USERSERVICE_OBJ) $(DAO_OBJ) $(MESSAGE_QUEUE_OBJ) \
	$(CFG_OBJ) $(THREAD_OBJ) $(USERSERVICE_TASK_OBJ) $(MEM_DB_OBJ) $(CXXFLAGS) $(MYSQL_FLAGE) $(INCLUDE) $(LIBS)
$(ORDER_SERVICE): $(ORDERSERVICE_OBJ) $(DAO_OBJ) $(MESSAGE_QUEUE_OBJ) $(THREAD_OBJ)  $(CFG_OBJ)\
$(ORDERSERVICE_TASK_OBJ) $(EVENT_OBJ) $(MEM_DB_OBJ) $(IPC_OBJ)
	$(CC) -o $(ROOT_DIR)/$(BIN_DIR)/$(ORDER_SERVICE) $(ORDERSERVICE_OBJ) $(DAO_OBJ) $(MESSAGE_QUEUE_OBJ) $(IPC_OBJ) \
	$(CFG_OBJ) $(THREAD_OBJ) $(MEM_DB_OBJ) $(ORDERSERVICE_TASK_OBJ) $(EVENT_OBJ) $(CXXFLAGS) $(MYSQL_FLAGE) $(INCLUDE) $(LIBS)