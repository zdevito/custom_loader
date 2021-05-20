#include "loader.h"
#include <iostream>
#include <thread>
#include <sstream>

using namespace loader;

struct PythonAPI {
  PythonAPI() {
    auto global = SystemLibrary::create();
    find_shared_function_ = CustomLibrary::create("libfind_shared_function.so");
    find_shared_function_->add_search_library(global);
    find_shared_function_->load();

    python_ = CustomLibrary::create(PYTHON_SO_PATH);
    python_->add_search_library(find_shared_function_);
    python_->add_search_library(global);
    python_->load();

    auto find_shared_python_ref = (CustomLibraryPtr*) find_shared_function_->sym("the_python_library").value();
    *find_shared_python_ref = python_;

    python_runner_ = CustomLibrary::create("libpython_runner.so");
    python_runner_->add_search_library(python_);
    python_runner_->add_search_library(global);
    python_runner_->load();
  }
  void run(const char* code) {
    auto run = (void(*)(const char* code)) python_runner_->sym("run").value();
    run(code);
  }
  CustomLibraryPtr find_shared_function_;
  CustomLibraryPtr python_;
  CustomLibraryPtr python_runner_;
};

auto example_src = R"end(
print("I think None is", id(None))
from time import time

def fib(x):
  if x <= 1:
    return 1
  return fib(x - 1) + fib(x - 2)

def do_fib():
  s = time()
  fib(30)
  e = time()
  print(e - s)

)end";

int main(int argc, const char **argv) {
  PythonAPI a;
  PythonAPI b;
  a.run(example_src);
  b.run(example_src);

  std::cout << "fib(30) for single interpreter\n";
  std::thread t0([&]{
    a.run("do_fib()");
  });
  std::thread t1([&]{
    a.run("do_fib()");
  });
  t0.join();
  t1.join();

  std::cout << "fib(30) for 2 interpreters\n";
  std::thread t2([&]{
    a.run("do_fib()");
  });
  std::thread t3([&]{
    b.run("do_fib()");
  });
  t2.join();
  t3.join();

  a.run("import regex");
}