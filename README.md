# Using Pre-existing preprocessor
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

// The name of the file followed by ".meta". For example, "foo.hpp" would need "#include <.meta/foo.meta>" 
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

# Creating custom preprocessor
```cpp
#include <gep/preprocessor>

int main()
{
  // create the preprocessor object
  gep::preprocessor proc;

  // loads target files as well as all dependent files
  // ie if myfirstfile.h includes <vector> in will also load <vector>
  proc.load("myfirstfile.h");
  proc.load("mysecondfile.h");

  gep::reflect::rnamespace global_namespace = proc.get_global_namespace();

  // getting a specific variable out of a specific class
  gep::reflect::variable testvariable = global_namespace.get_class("TestClass").get_variable("testvariable");

  // getting information from that variable
  testvariable.get_type(); // returns the type of the variable as a string
  testvariable.is_const(); // returns true if const
  testvariable.is_public(); // returns true if varaible is public facing
  // ...

  // looping through classes
  for (gep::reflect::rclass& rclass : global_namespace.get_classes())
  {
    // looping through variables in a class
    for (gep::reflect::variable& variable : rclass.get_variables())
    {
      variable.get_name(); // returns the name of the current variable
    }

    // looping through functions in a class
    for (gep::reflect::function& function : rclass.get_functions())
    {
      function.get_name(); // returns the name of the current function

      for (gep::reflect::variable& arg : function.get_arguments())
      {
        arg.get_type(); // returns the type of the current arg
      }
    } 
  }
}
```
