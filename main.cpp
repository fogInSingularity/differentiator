#include "difcode/include/dif.h"

int main(const int argc, const char** argv) {
  DifError error = DifError::kSuccess;

  Diffirentiator dif = {};
  error = dif.Ctor(argc, argv);

  if (error == DifError::kSuccess) {
     error = dif.Diffirentiate();
  }

  if (error == DifError::kSuccess) {
    error = dif.Simplify();
  }

  dif.ThrowError(error);
  dif.Dtor();

  return 0;
}
