#include "../../include/messagequeue/CKafka.h"
#include "../../include/log/log.h"
void ExampleDeliveryReportCb::dr_cb(RdKafka::Message &message)
{
    LOG_DEBUG("In ExampleDeliveryReportCb::dr_cb");
    std::string status_name;
    switch (message.status())
    {
    case RdKafka::Message::MSG_STATUS_NOT_PERSISTED:
        status_name = "NotPersisted";
        break;
    case RdKafka::Message::MSG_STATUS_POSSIBLY_PERSISTED:
        status_name = "PossiblyPersisted";
        break;
    case RdKafka::Message::MSG_STATUS_PERSISTED:
        status_name = "Persisted";
        *((int *)message.msg_opaque()) = 1;
        break;
    default:
        status_name = "Unknown?";
        break;
    }
    LOG_DEBUG("ExampleDeliveryReportCb::dr_cb Message delivery for (%d) bytes): %s: %s", message.len(), status_name.c_str(), message.errstr().c_str());
    if (message.key())
    {
        LOG_DEBUG("ExampleDeliveryReportCb::dr_cb Key: %s", *(message.key()));
    }
    if (message.msg_opaque())
    {
        LOG_DEBUG("ExampleDeliveryReportCb::dr_cb msg_opaque: %d\n", *((int *)message.msg_opaque()));
    }

    LOG_DEBUG("Out ExampleDeliveryReportCb::dr_cb");
}

void ExampleEventCb::event_cb(RdKafka::Event &event)
{
    LOG_DEBUG("In ExampleEventCb::event_cb");
    switch (event.type())
    {
    case RdKafka::Event::EVENT_ERROR:
        if (event.fatal())
        {
            LOG_FATAL("ExampleEventCb::event_cb FATAL");
        }
        //std::cerr << "ERROR (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
        LOG_ERROR("ExampleEventCb::event_cb ERROR (%s):%s", RdKafka::err2str(event.err()).c_str(), event.str().c_str());
        break;

    case RdKafka::Event::EVENT_STATS:
        //std::cerr << "\"STATS\": " << event.str() << std::endl;
        LOG_DEBUG("ExampleEventCb::event_cb STATS :%s", event.str().c_str());
        break;

    case RdKafka::Event::EVENT_LOG:
        /*         fprintf(stderr, "LOG-%i-%s: %s\n",
                event.severity(), event.fac().c_str(), event.str().c_str()); */
        LOG_DEBUG("ExampleEventCb::event_cb LOG-%i-%s: %s", event.severity(), event.fac().c_str(), event.str().c_str());

        break;

    default:
        std::cerr << "EVENT " << event.type() << " (" << RdKafka::err2str(event.err()) << "): " << event.str() << std::endl;
        // LOG_DEBUG("ExampleEventCb::event_cb LOG-%i-%s: %s", event.severity(), event.fac().c_str(), event.str().c_str());
        break;
    }
    LOG_DEBUG("Out ExampleEventCb::event_cb");
}

void msg_consume(RdKafka::Message *message, void *opaque)
{
    LOG_DEBUG("In msg_consume");
    const RdKafka::Headers *headers;

    switch (message->err())
    {
    case RdKafka::ERR__TIMED_OUT:
        break;

    case RdKafka::ERR_NO_ERROR:
        /* Real message */
        //std::cout << "Read msg at offset " << message->offset() << std::endl;
        LOG_DEBUG("msg_consume Read msg at offset:%d", message->offset());
        if (message->key())
        {
            //std::cout << "Key: " << *message->key() << std::endl;
            LOG_DEBUG("msg_consume Key:%d", (*message->key()).c_str());
        }
        headers = message->headers();
        if (headers)
        {
            std::vector<RdKafka::Headers::Header> hdrs = headers->get_all();
            for (size_t i = 0; i < hdrs.size(); i++)
            {
                const RdKafka::Headers::Header hdr = hdrs[i];

                if (hdr.value() != NULL){
/*                     printf(" Header: %s = \"%.*s\"\n",
                           hdr.key().c_str(),
                           (int)hdr.value_size(), (const char *)hdr.value()); */
                    LOG_DEBUG(" Header: %s = \"%.*s\"\n",
                           hdr.key().c_str(),
                           (int)hdr.value_size(), (const char *)hdr.value());
                }
                else
                {
                    //printf(" Header:  %s = NULL\n", hdr.key().c_str());
                    LOG_DEBUG(" Header:  %s = NULL\n", hdr.key().c_str());
                }
            }
        }
        /*         printf("%.*s\n",
               static_cast<int>(message->len()),
               static_cast<const char *>(message->payload())); */
        LOG_DEBUG("%.*s\n",
                  static_cast<int>(message->len()),
                  static_cast<const char *>(message->payload()));
        break;

    case RdKafka::ERR__PARTITION_EOF:
        /* Last message */
        break;

    case RdKafka::ERR__UNKNOWN_TOPIC:
    case RdKafka::ERR__UNKNOWN_PARTITION:
        //std::cerr << "Consume failed: " << message->errstr() << std::endl;
        LOG_DEBUG("msg_consume Consume failed:%s", message->errstr().c_str());
        break;

    default:
        /* Errors */
        //std::cerr << "Consume failed: " << message->errstr() << std::endl;
        LOG_DEBUG("msg_consume Consume failed:%s", message->errstr().c_str());
    }
    LOG_DEBUG("Out msg_consume");
}

void ExampleConsumeCb::consume_cb(RdKafka::Message &msg, void *opaque)
{
    LOG_DEBUG("In ExampleEventCb::consume_cb");
    msg_consume(&msg, opaque);
    LOG_DEBUG("Out ExampleEventCb::consume_cb");
}
