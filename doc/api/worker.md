# Worker

Worker is a synchronous process that consumes
one task, issues multiple intermediate updates
and returns result.

## Methods

### ConsumeTask()

```
ConsumeTask() -> Task
```

### Status()

May be called multiple times.
Delivery is not guaranteed.

```
Status(bytes status)
```

### Result()

Must be called exactly once. If not called
task will be returned to queue.

```
Result(bytes data)
```
