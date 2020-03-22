#include "register_types.h"
#include "core/class_db.h"
#include "RuntimeMeshLoader.h"

void register_rml_types() {
    ClassDB::register_class<RuntimeMeshLoader>();
}

void unregister_rml_types() {
}
