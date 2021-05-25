#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <pybind11/embed.h> // everything needed for embedding
#include <pybind11/pybind11.h>

namespace py = pybind11;


struct PythonGuard {
  PythonGuard() {
      Py_Initialize();
      // this has to occur on the thread that calls finalize
      // otherwise 'assert tlock.locked()' fails
      py::exec("import threading");
      // release GIL after startup, we will acquire on each call to run
      PyEval_SaveThread();
  }
  ~PythonGuard() {
      PyGILState_Ensure();
      Py_Finalize();
  }
};

static PythonGuard runner;

extern "C" void run(const char * code) {
  py::gil_scoped_acquire guard_;
  py::exec(code);
}