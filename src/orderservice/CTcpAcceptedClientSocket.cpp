#include "../../include/orderservice/CTcpAcceptedClientSocket.h"
#include "../../include/orderservice/CServer.h"
#include "../../include/orderservice/orderserviceglobal.h"
#define USER_NUMBER 10
#define SHOP_NUMBER 10
#define PRODUCT_NUMBER 20;
CTcpAcceptedClientSocket::CTcpAcceptedClientSocket(int fd) : CSocket("", -1, -1, -1, 0, fd)
{
    LOG_DEBUG("In CTcpAcceptedClientSocket  CTcpAcceptedClientSocket fd:%d", fd);
    LOG_DEBUG("Out CTcpAcceptedClientSocket  CTcpAcceptedClientSocket fd:%d", fd);
}

CTcpAcceptedClientSocket::CTcpAcceptedClientSocket() : CSocket("", -1, -1, -1, 0, -1)
{
/*     LOG_DEBUG("In CTcpAcceptedClientSocket  CTcpAcceptedClientSocket fd:%d", -1);
    LOG_DEBUG("Out CTcpAcceptedClientSocket  CTcpAcceptedClientSocket fd:%d", -1); */
}

CTcpAcceptedClientSocket::~CTcpAcceptedClientSocket()
{
    LOG_DEBUG("In CTcpAcceptedClientSocket  ~CTcpAcceptedClientSocket");
    LOG_DEBUG("Out CTcpAcceptedClientSocket  ~CTcpAcceptedClientSocket");
}

void CTcpAcceptedClientSocket::RecvHandler(int fd, void *privdata, int mask)
{
    LOG_DEBUG("In CTcpAcceptedClientSocket  RecvHandler fd:%d, mask:%d", fd, mask);

     CTcpAcceptedClientSocket &client = globalServer.GetClient(fd);
    if (client.m_fd > 0)
    {
        char line[MAXLINE] = {0};
        int recvNum = client.Recv(line, MAXLINE);
        if (recvNum > 0)
        {
            LOG_WARN("In CTcpAcceptedClientSocket  RecvHandler recv recvNum:%d, line:%s", recvNum, line);
            CPlaceOrderTask *task = new CPlaceOrderTask("CPlaceOrderTask", "");
            Order *orderData = (Order *)malloc(sizeof(struct Order));
            orderData->clientFd = fd;
            orderData->productID = rand() % PRODUCT_NUMBER + 1;
            orderData->userID = rand() % USER_NUMBER + 1;
            orderData->shopID = rand() % SHOP_NUMBER + 1;
            orderData->number = rand() % 10 + 1;
            task->setData(orderData);
            orderThreadPool.AddTask(task);
        }
        else
        {
            LOG_WARN("In CTcpAcceptedClientSocket  RecvHandler recv client closed!");
            globalServer.RemoveClient(fd);
        }
    }
    else
    {
        LOG_WARN("In CTcpAcceptedClientSocket  RecvHandler pClient is empty");
    } 

    LOG_DEBUG("Out CTcpAcceptedClientSocket  RecvHandler");
}
