#ifndef PLUG_H
#define PLUG_H

#define LIST_OF_PLUGS \
    PLUG(plug_update, void, void) \
    PLUG(plug_init, void, void) \
    PLUG(plug_pre_reload, void*, void) \
    PLUG(plug_post_reload, void, void*)
#define PLUG(name, ret, ...) ret (*name)(__VA_ARGS__);
LIST_OF_PLUGS
#undef PLUG
#endif // PLUG_H
