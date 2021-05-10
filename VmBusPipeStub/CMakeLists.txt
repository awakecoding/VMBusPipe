
add_library(vmbuspipe SHARED
	Helpers.cpp
	Helpers.h
	VMBusPipeStub.cpp
	VMBusPipeStub.h
	vmbuspipe.def)

set_target_properties(vmbuspipe PROPERTIES OUTPUT_NAME "vmbuspipe")
target_link_libraries(vmbuspipe VMBusPipeCore)

add_library(vmbuspiper SHARED
	Helpers.cpp
	Helpers.h
	VMBusPipeStub.cpp
	VMBusPipeStub.h
	vmbuspiper.def)

target_compile_definitions(vmbuspiper PRIVATE VMBUS_PIPER_STUB=1)
set_target_properties(vmbuspiper PROPERTIES OUTPUT_NAME "vmbuspiper")
target_link_libraries(vmbuspiper VMBusPipeCore)
