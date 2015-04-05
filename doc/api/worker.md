# Worker

Worker is a synchronous process that consumes
one task, issues multiple intermediate updates
and returns result.

## Related classes

### ConnectionOptions

```
[required] Constraints constraints
[optional] string connection
[optional] Credentials credentials
```

Constraints are used to specify what backend provides.
*Subject to refactoring.*

See [Credentials](common.md#credentials).

## Methods

### Open()

```
Open(ConnectionOptions)
```

### Close()

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
