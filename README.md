## Idea
- I want to create a program that can be ran on source code prior to compilation to allow for more powerful code generation.
- It will not modify any written source code, it will only modify code inside its own include.
- I want to have a friendly user interface so there are no nasty macros wrapping code everywhere pretending to be reflection.

## Usage
### main.hpp
```cpp
#include <Reflection.hpp>

class ExampleClass
{
public:
  ExampleClass() : mData(7), mMoreData(10) {}

  void SetData(int newData)
  {
    mData = newData;
  }

private:
  serializable int mData;
  int mMoreData;
};

// the name of the file followed by ".meta". for example foo.cpp would need #include <.meta/foo.meta> 
#include <.meta/main.meta>
```
### main.cpp
```cpp
#include "main.hpp"

int main()
{
  ExampleClass obj;
  gep::print(obj);

  obj.SetData(42);
  gep::print(obj);
}
```
### output
```
ExampleClass:
{
  mData = 7
}
ExampleClass:
{
  mData = 42
}
```

## Setup
- Download preprocessor-installer.exe
- Run the installer
- Open any workspace and run:
  `> preprocessor -getfiles`
- This will generate an include folder with the needed include files
- Include the generated include files as shown in the example above
- Run:
  `> preprocessor myfile.hpp`
- Compile and run:
  `> g++ myfile.cpp -o a.exe`
  `> a.exe`

