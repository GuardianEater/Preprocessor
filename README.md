## Idea
- I want to create a program that can be ran on source code prior to compilation to allow for more powerful code generation.
- I want to have a friendly user interface so there are no nasty macros wrapping code everywhere pretending to be reflection.

## Usage
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

main()
{
  ExampleClass obj;
  gep::print(obj);

  obj.SetData(42);
  gep::print(obj);
}
```
```
ExampleClass obj:
{
  mData = 7
}
ExampleClass obj:
{
  mData = 42
}
```
