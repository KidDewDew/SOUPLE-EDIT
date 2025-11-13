#include "obj.h"
#include "souplemanager.h"
#include "anchorobj_flowtext.h"
#include "anchorobj_glue.h"
#include "anchorobj_hline.h"
Obj::Obj() {   id = s_all_id++;    }

Obj::~Obj() {
    //
}

Obj* Obj::unserialize_all(const char* bytes,int& at) {
    int16_t obj_type = *(int16_t*)&bytes[at];
    switch(obj_type) {
    case Serialize_AnchorObj_FlowText:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_Glue:
        return (new AnchorObj_Glue)->unserialize(bytes,at);
    case Serialize_AnchorObj_HLine:
        return (new AnchorObj_HLine)->unserialize(bytes,at);
    case Serialize_AnchorObj_Image:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_JZRect:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_LatexFormula:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_Path:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_PHLeft:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_PHRect:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_PHRight:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_Rich:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_AnchorObj_VLine:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_BlockInner_HorLine:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_Frame_ofHLines:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_Free_Image:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_Free_Path:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_Free_TableUnit:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_Free_Text:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_HorLine_Base:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_Obj_Page:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_TableLine:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    case Serialize_WordPage_VLine:
        return (new AnchorObj_FlowText)->unserialize(bytes,at);
    }
    return nullptr;
}
