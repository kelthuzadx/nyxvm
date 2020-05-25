<p align="center"><img height="20%" width="30%" src="./doc/logo.png"></p>

This project extends [nyx](https://github.com/kelthuzadx/nyx). Instead of an AST based interpreter, **nyxvm** transforms
AST to bytecode, and interprets bytecode on VM.

# First Impression
```go
func hello(){
    return func(){ return "world" }
}

w = hello()
w()
```

# Documentation
+ [Bytecode reference](doc/bytecode.md)
+ [Builtin functions](doc/builtin.md)

# License
All code licensed under the [MIT License](LICENSE). Note that during the development, license is subject to change without further notifications.