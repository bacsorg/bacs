High-level design
=================

Design is split into three main parts
- [Client](client.md)
- [Broker](broker.md)
- [Worker](worker.md)

**Client** only interacts with **broker**.
Broker interacts **client** and **worker**.
**Worker** only interacts with **broker**.

**Client** may connect to multiple **brokers** based
on resource it need to consume.

Task
----
**Task** is a blob with attributes sent by a **client**.
```
message Task {
    bytes data
    Unspecified criteria
    uint32 timeout
    uint32 tries
}
```

**Client** specifies criteria for **worker** selection.
If there is no **worker** found matching specified criteria
task is queued for at most *timeout*.
If no worker is found *error* is returned.
If **worker** is found *task* is sent for processing.
**Worker** returns *result*. If **worker** loses connection
to **broker** during *task* execution, *task*
is rescheduled. Rescheduling happens no more than *tries* times.
```
message Result {
    oneof {
        bytes data
        Error error
    }
}
```
