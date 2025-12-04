
//在这里实现所有 NoRef_xxx.h

#include "NoRef_SoupleManager.h"

#include "souplemanager.h"

Obj* NoRef_SoupleManager::getObjById(int id) noexcept {
    return SoupleManager::getObjById(id);
}
