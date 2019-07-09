#ifndef CKAFKA_H
#define CKAFKA_H
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <unistd.h>

/*
 * Typically include path in a real application would be
 * #include <librdkafka/rdkafkacpp.h>
 */
#include <librdkafka/rdkafkacpp.h>

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb
{
  public:
    void dr_cb(RdKafka::Message &message);
};

class ExampleEventCb : public RdKafka::EventCb
{
  public:
    void event_cb(RdKafka::Event &event);
};

class ExampleConsumeCb : public RdKafka::ConsumeCb
{
  public:
    void consume_cb(RdKafka::Message &msg, void *opaque);
};


void msg_consume(RdKafka::Message *message, void *opaque);

#endif