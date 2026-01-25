#ifndef TURNBACK_TEMPLATE_HPP
#define TURNBACK_TEMPLATE_HPP

#include "souplemanager.h"
#include "objutil.h"

class TurnbackManager;

template<typename T,typename _Func>
    requires requires(_Func func,T* obj,int start_i,int len){
        {func(obj,start_i,len)}->std::same_as<void>;
    }
auto TurnbackManager::new_template_walker(_Func func,int contentLength,int offset) {
    return [=](Turnback*tb,Obj*obj)->void {
        HorLine_Base* hline = (HorLine_Base*)(((AnchorObj*)obj)->hline);
        if(! hline) return;
        //qDebug() << "template_walker: " << obj->__dstr();
        for(auto[obj,start_i,len] : hline->getWalker((AnchorObj*)obj,tb->flow_position+offset,contentLength)) {
            T* t = obj->as<T*>();
            if(! t) return;
            func(t,start_i,len);
        }
    };
}

template<bool isRemoval,bool objs_sorted>
void TurnbackManager::addTurnback_of_obj_remove_or_insert(Obj* obj)
{
    bool isAnchorObj = obj->canBe<AnchorObj>();
    Turnback* tb = addTurnback();
    if(isAnchorObj) {
        tb->action = Turnback::AC_Content_Flow;

        //插入流附着符
        tb->flow_position = 0;
        obj->be<AnchorObj*>()->addFlowAttacher(tb);

        QByteArray bytes = souple::serialization::serialize(obj);
        tb->undo = new_template_walker<AnchorObj>([bytes](AnchorObj* obj,int start_pos,int len)
            {
                QByteArray bytes2 = bytes;
                AnchorObj* new_obj = souple::serialization::unserialize_one<AnchorObj>(bytes2);
                SoupleManager::registerObj(new_obj);
                obj->slice(0,start_pos+len)->insertOnRight(new_obj);
            },1);
        tb->redo = new_template_walker<AnchorObj>([](AnchorObj* obj,int start_pos,int len){
                auto to_remove_obj = obj->slice(start_pos,len);
                to_remove_obj->removeSelf(true);
            },obj->contentLength()); //offset = 1
        if constexpr(isRemoval) {
            obj->removeSelf(true);
        }
    } else {
        if constexpr(isRemoval) {
            obj->removeSelf(true);
        }
        tb->action = Turnback::AC_Single_Obj;
        tb->attach_obj_id = -1;
        //使用tuple包装，避免指针释放。
        std::shared_ptr<std::tuple<Obj*>> obj_ptr =
                std::make_shared<std::tuple<Obj*>>();
        if constexpr(!isRemoval) {
            std::get<0>(*obj_ptr) = obj;
        }
        QByteArray bytes = souple::serialization::serialize(obj);
        tb->undo = [bytes,obj_ptr](Turnback* tb,Obj*) {
            QByteArray bytes2 = bytes;
            auto obj = souple::serialization::unserialize_one<Obj>(bytes2);
            SoupleManager::registerObj(obj);
            std::get<0>(*obj_ptr) = obj;
        };
        tb->redo = [obj_ptr](Turnback* tb,Obj*) {
            std::get<0>(*obj_ptr)->removeSelf(true);
        };
    }
    if constexpr(!isRemoval) {
        std::swap(tb->undo,tb->redo);
    }
}

template<bool isRemoval,bool objs_sorted>
void TurnbackManager::addTurnback_of_obj_remove_or_insert(
        const std::vector<Obj*>& objs)
{
    if(objs.empty()) return;
    bool isAnchorObj = objs[0]->canBe<AnchorObj>();
    Turnback* tb = addTurnback();

    qDebug() << "isAnchorObj: " << isAnchorObj;

    //注意，我们知道，撤回/重做也是有最大数量的。超出数量后，队尾的turnback将被销毁。
    //销毁时，是否应该把这些obj* delete掉？

    // auto deleter = [](std::vector<Obj*>* vec) {
    //     //当Turnback销毁时，本deleter被调用。
    //     auto& objs = *vec;
    //     if(objs[0]->hidden_sign) { //如果这些对象还处于隐藏状态则应该彻底删除
    //         for(auto obj : objs) {
    //             if(SoupleManager::getObjById(obj->id))
    //                 obj->removeSelf(true);
    //             else delete obj;
    //         }
    //     }
    //     delete vec;
    // };

    if(isAnchorObj) {
        tb->action = Turnback::AC_Content_Flow;
        std::vector<AnchorObj*> pass_objs;
        if constexpr (! objs_sorted) {
            //强制类型转换，可写。
            sort_anchorobj(
                    (const std::vector<AnchorObj*>&)objs,
                    (std::vector<AnchorObj*>&)pass_objs
            );
            if(pass_objs.empty()) return;
        } else {
            pass_objs = *(std::vector<AnchorObj*>*)&objs;
        }

        //qDebug() << "sorted_objs.size: " << pass_objs.size();

        //统计pass_objs的内容长度
        int full_length = 0;
        for(auto obj : pass_objs) {
            full_length += obj->contentLength();
        }


        tb->flow_position = 0;

        //addFlowAttacher将自动把tb放到左边的桩上。
        pass_objs[0]->be<AnchorObj*>()->addFlowAttacher(tb);

        //利用流附着符定位(FlowAttacher)
        //Notice: 由于对象之间的合并，这里必须使用序列化来存储这些对象。

        QByteArray bytes;
        QDataStream ds(&bytes,QIODevice::ReadWrite);

        for(auto obj : pass_objs) {
            souple::serialization::serialize(obj,ds);
        }

        tb->undo = new_template_walker<AnchorObj>([bytes](AnchorObj* obj,int start_pos,int len)
                {
                //首先，得从bytes中恢复原本的对象。
                qDebug() << "undo: " << obj->__dstr() << start_pos << len;
                std::vector<Obj*> ori_objs;
                QByteArray bytes2 = bytes;
                QDataStream ds(&bytes2,QIODevice::ReadOnly);
                while(! ds.atEnd()) {
                    ori_objs.push_back(
                        (Obj*)souple::serialization::unserialize(ds)
                    );
                }
                auto left = obj->slice(start_pos,len);
                //逆序向右边插入即可。
                for(auto obj : ori_objs | std::views::reverse) {
                    SoupleManager::registerObj(obj);
                    qDebug() << "unserialize: " << obj->__dstr();
                    left->insertOnRight((AnchorObj*)obj);
                }
            },1);

        tb->redo = new_template_walker<AnchorObj>([](AnchorObj* obj,int start_pos,int len){
                auto to_remove_obj = obj->slice(start_pos,len);
                to_remove_obj->removeSelf(true);
            },full_length);

        if constexpr(isRemoval) { //真删除，因为有序列化。
            for(auto obj : objs) {
                obj->removeSelf(true);
            }
        }

    } else {
        tb->action = Turnback::AC_Single_Obj;
        tb->attach_obj_id = -1; //-1表示不附着。

        QByteArray bytes = souple::serialization::serialize<Obj>(objs);

        if constexpr(isRemoval) { //对于这种非anchorobj，假删除即可
            for(auto obj : objs) {
                obj->removeSelf(true);
            }
        }

        std::shared_ptr<std::vector<Obj*>> objs_ptr =
                std::make_shared<std::vector<Obj*>>();

        if constexpr(!isRemoval) {
            for(auto obj : objs) {
                objs_ptr->push_back(obj);
            }
        }

        tb->undo = [bytes,objs_ptr](Turnback* tb,Obj* obj) {
            QByteArray bytes2 = bytes;
            QDataStream ds(&bytes2,QIODevice::ReadOnly);
            while(!ds.atEnd()) {
                auto obj = (Obj*)souple::serialization::unserialize(ds);
                SoupleManager::registerObj(obj);
                objs_ptr->push_back(obj); //新增记录
            }
        };

        tb->redo = [objs_ptr](Turnback* tb,Obj* obj) {
            for(auto obj : *objs_ptr.get()) {
                obj->removeSelf(true); //删除新增
            }
        };
    }
    //以上按照tb->undo: 恢复objs redo:删除objs
    //若非isRemoval，应交换
    if constexpr(!isRemoval) {
        std::swap(tb->undo,tb->redo);
    }
}

#endif // TURNBACK_TEMPLATE_HPP
