High-level design
=================

Design is split into three main parts
- [Client](client.md)
- [Broker](broker.md)
- [Worker](worker.md)

Client only interacts with Broker.
Broker interacts Client and Worker.
Worker only interacts with Broker.

Client may connect to multiple Brokers based
on resource it need to consume.
