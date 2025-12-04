#ifndef NOREF_SOUPLEMANAGER_H
#define NOREF_SOUPLEMANAGER_H

class Obj;

class NoRef_SoupleManager {
public:
    static Obj* getObjById(int id) noexcept;
};

#endif // NOREF_SOUPLEMANAGER_H
