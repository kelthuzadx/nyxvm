# language reference
language syntax and everything

## Control flow
```
var = foo()
if(var){
    println("ok)
} else {
    println("ok)
}
```

## Closure
```
func(){ println("Immediate function call")}()

func add(a,b) { return a+b }
func add3(b) { 
    return func(){ 
        return add(3,b) 
    }
}
```
