#include <iostream>
#include "HIAIMixModel.h"

using namespace std;

int main() {
  HIAI_MixModelManager* modelMang = HIAI_MixModelManager_Create(nullptr);

  if (modelMang) {
    cout << "HIAI_MixModelManager_Create success" << endl;
  }

  HIAI_MixModelManager_Destroy(modelMang);

  return 0;
}
