# Common classes

## Credentials

```
[optional] string method
[optional] string login
[optional] string password
[optional] bytes certificate
[optional] bytes key
```

## Constraints

Empty list means match any.
Result matcher is conjunction of specified properties.

```
[optional] string resources[]
```

## Task

```
[required] string type,
[required] string package,
[required] bytes data
```

- `type` type of package, how it should be executed
- `package` package name for [bunsan::pm](/bunsanorg/pm)
- `data` data for processing
